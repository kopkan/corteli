#include "BaseClient.h"

using namespace corteli::network::pipe;


BaseClient::BaseClient(IoService* ioService, bool enabledDebugStream) : BaseObject(enabledDebugStream), _pipeStreamHandle(ioService->getIoSevice())
{
	_ioService = ioService;
	_status = status::INITED;
}

/*
BaseClient::BaseClient(IoService* ioService, HANDLE hPipe, bool enabledDebugStream) : BaseObject(enabledDebugStream), _pipeStreamHandle(ioService->getIoSevice())
{
	_hPipe = hPipe;
	_ioService = ioService;
	_status = status::INITED;
}
*/

BaseClient::~BaseClient()
{
	//BaseObject::write("destruct");
	close();
}



int BaseClient::getStatus()
{
	return _status;
}

int BaseClient::getError()
{
	return _error;
}

unsigned long long BaseClient::getLastRecvMessageTime()
{
	return _lastRecvMessageTime;
}

unsigned long long BaseClient::getLastSentMessageId()
{
	return _lastSentMessageId;
}




int BaseClient::connect(std::string pipename)
{
	if (_status == status::INITED && !_error)
	{
		_status = status::CONNECT;
		pipename = "\\\\.\\pipe\\" + pipename;

		while (!_error)
		{
			_hPipe = CreateFileA(
				pipename.c_str(),   // pipe name 
				GENERIC_READ |  // read and write access 
				GENERIC_WRITE,
				0,              // no sharing 
				NULL,           // default security attributes
				OPEN_EXISTING,  // opens existing pipe 
				FILE_FLAG_OVERLAPPED,              // default attributes 
				NULL);          // no template file 

			if (_hPipe != INVALID_HANDLE_VALUE)
			{
				_pipeStreamHandle.assign(_hPipe);//_hPipe warning
				_status = status::CONNECTED;
				connected();
				return error::NO_ERR;
			}
			else
			{
				CloseHandle(_hPipe);
				DWORD error = GetLastError();
				connectError(error, pipename);
			}
		}

		_status = status::END_WORK;
		return error::CONNECT_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

int BaseClient::connectT(std::string pipename)
{
	if (_status == status::INITED && !_error)
	{
		std::thread(&BaseClient::connect, this, pipename).detach();
		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

bool BaseClient::waitConnected(time_t time)
{
	time_t startTime = clock();

	while (_status >= status::CONNECT && _status <= status::CONNECTED && !_error)
	{
		if (_status >= status::CONNECTED)
		{
			return true;
		}

		if (clock() - startTime < time || time == 0)
		{
			Sleep(1);
		}
		else
		{
			BaseObject::write("waitConnect end");
			break;
		}
	}
	return false;
}

int BaseClient::startRecv(std::size_t size)
{
	if (_status == status::CONNECTED && !_error)
	{
		_lastRecvMessageTime = clock();
		_status = status::RECV_STARTED;

		_recvBuff = new char[size];
		_pipeStreamHandle.async_read_some(boost::asio::buffer(_recvBuff, size), boost::bind(&BaseClient::_recvHandle, this, _1, _2, size));
		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

unsigned long long BaseClient::send(char* buff, int size)
{
	if (_status >= status::CONNECTED && _status<status::END_WORK && !_error)
	{
		_pipeStreamHandle.async_write_some(boost::asio::buffer(buff, size), boost::bind(&BaseClient::_sendHandle, this, _1, _2, ++_lastSentMessageId));
		return _lastSentMessageId;
	}
	else
	{
		return 0;
	}
}



int BaseClient::close(bool setEndWorkStatus)
{
	if (setEndWorkStatus)
	{
		_status = status::END_WORK;
	}

	if (_error != error::CLOSE)
	{
		_setError(error::CLOSE);


		_pipeStreamHandle.close();

		if (_status >= status::CONNECT)
		{
			while (_status != status::END_WORK && _status != status::CONNECTED)
			{
				Sleep(1);
			}
		}

		if (_recvBuff != NULL)
		{
			delete _recvBuff;
			_recvBuff = NULL;
		}

		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

void BaseClient::connected()
{
	BaseObject::write("connected");
}

void BaseClient::connectError(DWORD ec, std::string pipename)
{
	//close(true);

	if (ec != ERROR_PIPE_BUSY)
	{
		BaseObject::write("Could not open pipe. GLE=", 0);
		BaseObject::write(ec, 1);
		Sleep(100);
	}
	else
	{
		if (!WaitNamedPipe(pipename.c_str(), 100))
		{
			BaseObject::write("Could not open pipe: 0,1 second wait timed out.");
		}
	}

	//_setError(error::CONNECT_ERR);
	//_status = status::END_WORK;
	//close();
}

void BaseClient::recvMessage(char* buff, std::size_t size)
{
	BaseObject::write("recv=", 0);
	BaseObject::write(size, 0);
	BaseObject::write(" ", 0);
	BaseObject::write(buff);
}

void BaseClient::recvError(const boost::system::error_code & ec)
{
	close(true);
}

void BaseClient::sentMessage(unsigned long long messageId)
{
	BaseObject::write("sent ok, sent id=", 0);
	BaseObject::write(messageId);
}

void BaseClient::sendError(const boost::system::error_code & ec)
{
	close();
}

void BaseClient::endWork()
{
	BaseObject::write("BasePipeClient endWork");
}


void BaseClient::connectExpansion()
{
}

void BaseClient::chatExpansion()
{
}

void BaseClient::runExpansion()
{
	switch (_status)
	{
	case status::CONNECT:
	{
		connectExpansion();
		break;
	}
	case status::RECV_STARTED:
	{
		chatExpansion();
		break;
	}
	}
}

void corteli::network::pipe::BaseClient::_acceptorSetPipeHandle(HANDLE pipe)
{
	_pipeStreamHandle.assign(pipe);
	_status = status::CONNECTED;
}

void BaseClient::_recvHandle(const boost::system::error_code& error, std::size_t bytes_transferred, std::size_t buffSize)
{
	if (!_error)
	{
		if ((error.value() != 0 && error.value() != 234)//err and err not 'more data'(234)
			)
		{
			BaseObject::write("recv err==", 0);
			BaseObject::write(error);
			BaseObject::write("recv err mess==", 0);
			BaseObject::write(error.message());

			recvError(error);
			return;
		}
		else {
			_lastRecvMessageTime = clock();

			recvMessage(_recvBuff, bytes_transferred);

			_pipeStreamHandle.async_read_some(
				boost::asio::buffer(_recvBuff, buffSize),
				boost::bind(&BaseClient::_recvHandle, this, _1, _2, buffSize)
				);
		}
	}
	else
	{
		_status = status::END_WORK;
	}
}

void BaseClient::_sendHandle(const boost::system::error_code& error, std::size_t bytes_transferred, unsigned long long writeMessageId)
{
	if (!_error)
	{
		if ((error.value() != 0 && error.value() != 234)//err and err not 'more data'(234)
			)
		{
			BaseObject::write("write err==", 0);
			BaseObject::write(error);
			BaseObject::write("write err mess==", 0);
			BaseObject::write(error.message());

			sendError(error);

			return;
		}
		else {
			sentMessage(writeMessageId);
		}
	}
}

void BaseClient::_setError(error err)
{
	if (_error != error::CLOSE)
	{
		_error = err;
	}
}
