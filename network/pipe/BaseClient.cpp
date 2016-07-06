#include "BaseClient.h"

using namespace corteli::network::pipe;


BaseClient::BaseClient(corteli::network::IoService* ioService, bool enabledDebugStream) : BaseObject(enabledDebugStream), _pipeStreamHandle(ioService->getIoSevice())
{
	_ioService = ioService;
	_status = status::INITED;
}


BaseClient::~BaseClient()
{
	close(0, 1);
	waitClose();
	while (_countCloseWaiting)
	{
		Sleep(1);
	}
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
		_setStatus(status::CONNECT);
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
				_setStatus(status::CONNECTED);
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

		_setStatus(status::END_WORK);
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

bool BaseClient::waitConnect(time_t time)
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
		_setStatus(status::RECV_STARTED);

		_recvBuff.resize(size);
		_pipeStreamHandle.async_read_some(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()),
			
			boost::bind(&BaseClient::_recvHandle, this, _1, _2)
			);

		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

unsigned long long BaseClient::send(corteli::base::container::Buffer<char> buffer)
{
	if (_status >= status::CONNECTED && _status<status::END_WORK && !_error)
	{
		_countSendWaiting++;

		
		_pipeStreamHandle.async_write_some(boost::asio::buffer(buffer.getBuff(), buffer.getSize()),
			
			boost::bind(&BaseClient::_sendHandle, this, _1, _2, ++_lastSentMessageId)	
			);

		return _lastSentMessageId;
	}
	else
	{
		return 0;
	}
}



int BaseClient::close(bool setEndWorkStatus, bool closingFlag)
{
	if (setEndWorkStatus)
	{
		_setStatus(status::END_WORK);
	}

	if (_error != error::CLOSE)
	{
		preClosing(closingFlag);
		_setError(error::CLOSE);


		_pipeStreamHandle.close();

		if (_status >= status::CONNECT)
		{
			while ((_countSendWaiting != 0 || _status < status::END_WORK) && _status != status::CONNECTED && _ioService->getStatus() != IoService::status::STOPPED)
			{
				BaseObject::write(_status);
				BaseObject::write(_countSendWaiting);
				Sleep(1);
			}
		}

		_setStatus(status::CLOSED);
		afterClosing(closingFlag);

		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

int BaseClient::closeT(bool setEndWorkStatus, bool closingFlag)
{
	if (setEndWorkStatus)
	{
		_setStatus(status::END_WORK);
	}

	if (_error != error::CLOSE)
	{
		std::thread(&BaseClient::close, this, setEndWorkStatus, closingFlag).detach();
		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

bool BaseClient::waitClose(time_t time)
{
	_countCloseWaiting++;

	time_t startTime = clock();

	while (clock() - startTime<time || time == 0)
	{
		if (_status == status::CLOSED)
		{
			_countCloseWaiting--;
			return true;
		}
		else
		{
			Sleep(1);
		}
	}

	_countCloseWaiting--;
	return false;
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

bool BaseClient::recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	//error.value() != 234)//err and err not 'more data'(234)

	if ((!(ec.value() == 0 || ec.value() == 234)) || bytes_transferred <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseClient::recvMessage(corteli::base::container::Buffer<char> buffer)
{
	BaseObject::write("recv=", 0);
	BaseObject::write(buffer.getSize(), 0);
	BaseObject::write(" ", 0);
	BaseObject::write(buffer.getBuff());
}

void BaseClient::recvError(const boost::system::error_code & ec)
{
	BaseObject::write("recv err==", 0);
	BaseObject::write(ec);
	BaseObject::write("recv err mess==", 0);
	BaseObject::write(ec.message());



	_setError(error::RECV_ERR);
	BaseObject::write("recvErrorSigal");
	closeT(true);
	BaseObject::write(ec.message());
}

bool BaseClient::sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId)
{
	//error.value() != 234)//err and err not 'more data'(234)

	if ((!(ec.value() == 0 || ec.value() == 234)) || bytes_transferred <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseClient::sentMessage(unsigned long long messageId)
{
	BaseObject::write("sent ok, sent id=", 0);
	BaseObject::write(messageId);
}

void BaseClient::sendError(const boost::system::error_code & ec, unsigned long long messageId)
{
	BaseObject::write("write err==", 0);
	BaseObject::write(ec);
	BaseObject::write("write err mess==", 0);
	BaseObject::write(ec.message());


	BaseObject::write("sendErrorSigal");
	_setError(error::SEND_ERR);
	closeT(true);
}

void BaseClient::preClosing(bool closingFlag)
{
	BaseObject::write("preClosing");
}

void BaseClient::afterClosing(bool closingFlag)
{
	BaseObject::write("afterClosing");
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

void BaseClient::reload()
{
	close();
	waitClose();

	while (_countCloseWaiting)
	{
		Sleep(1);
	}

	_status = status::INITED;
	_error = error::NO_ERR;
}

void corteli::network::pipe::BaseClient::_acceptorSetPipeHandle(HANDLE pipe)
{
	_pipeStreamHandle.assign(pipe);

	_setStatus(status::CONNECTED);
}

void BaseClient::_recvHandle(const boost::system::error_code& error, std::size_t bytes_transferred)
{
	if (!_error)
	{
		if (recvContainsError(error, bytes_transferred))//err and err not 'more data'(234)
		{
			recvError(error);
		}
		else {
			_lastRecvMessageTime = clock();

			recvMessage({ _recvBuff.getBuff(), bytes_transferred });

			_pipeStreamHandle.async_read_some(
				boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()),
				boost::bind(&BaseClient::_recvHandle, this, _1, _2)
				);
		}
	}
	else
	{
		_setStatus(status::END_WORK);
	}
}

void BaseClient::_sendHandle(const boost::system::error_code& error, std::size_t bytes_transferred, unsigned long long writeMessageId)
{

	if (sendContainsError(error, bytes_transferred, writeMessageId))
	{
		sendError(error, writeMessageId);
	}
	else {
		sentMessage(writeMessageId);
	}

	_countSendWaiting--;
}

void BaseClient::_setError(error err)
{
	if (_error != error::CLOSE)
	{
		_error = err;
	}
}

void BaseClient::_setStatus(status status)
{
	_status = status;
}
