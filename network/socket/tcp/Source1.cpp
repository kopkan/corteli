#include "Header.h"



Socket::Socket(boost::asio::io_service& ioService, bool enableDebugMessage) : BaseObject(enableDebugMessage), _socket(ioService)
{	
	_status = status::INITED;
}

Socket::Socket(boost::asio::ip::tcp::socket socket, bool enableDebugMessage) : BaseObject(enableDebugMessage), _socket(std::move(socket))
{
	boost::system::error_code ec;

	if (_socket.remote_endpoint(ec).port() != 0)
	{
		BaseObject::write("sock connecter");
		_status = status::CONNECTED;
	}
	else
	{
		BaseObject::write("init end");
		_status = status::INITED;
	}
}

int Socket::getStatus()
{
	return _status;
}

int Socket::getError()
{
	return _error;
}

int Socket::bind(boost::asio::ip::tcp::endpoint localAddr)
{
	if (_status >= status::INITED && _status < status::CONNECTED && !_error)
	{
		boost::system::error_code err;

		_socket.open(boost::asio::ip::tcp::v4(), err);
		_socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		if (err.value())
		{
			BaseObject::write(err.value());
			BaseObject::write(err.message());
			_setError(error::BIND_ERR);
			return err.value();
		}


		_socket.bind(localAddr, err);

		if (err.value())
		{
			BaseObject::write(err.value());
			BaseObject::write(err.message());
			_setError(error::BIND_ERR);
			return err.value();
		}

		_status = status::BOUND;
		return error::NO_ERR;

	}
	else
	{
		return error::CALL_DENIED;
	}
}


int Socket::connect(boost::asio::ip::tcp::endpoint remoteAddr)
{
	if (_status >= status::INITED && _status < status::CONNECTED && !_error)
	{
		_status = status::CONNECT;

		_socket.async_connect(remoteAddr,

			boost::bind(&Socket::_connectHandle, this,
				boost::asio::placeholders::error, remoteAddr
				)
			);

		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

bool Socket::waitConnect(time_t time)
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

int Socket::startRecv(std::size_t size)
{
	if (_status >= status::CONNECTED && _status < status::RECV_STARTED && !_error)
	{

		_recvBuff = new char[size];

		_status = status::RECV_STARTED;

		_socket.async_receive(boost::asio::buffer(_recvBuff, size), ///////

			boost::bind(&Socket::_recvHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, size)

			);

	}
	else
	{
		return error::CALL_DENIED;
	}
}


unsigned long long Socket::send(char* buff, int size, int flag)
{
	if (_status >= status::CONNECTED && _status < status::END_WORK && !_error)
	{

		_socket.async_send(boost::asio::buffer(buff, size), flag,

			boost::bind(&Socket::_sendHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				++_lastMessageId
				)
			);

		return _lastMessageId;
	}

	return 0;
}


int Socket::close(bool noWaitEndWorkSignal)
{
	if (_error != error::CLOSE)
	{
		boost::system::error_code err;
		_setError(error::CLOSE);

		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		_socket.close(err);

		if (_status >= status::CONNECT)
		{
			while (_status != status::END_WORK && _status != status::CONNECTED && !noWaitEndWorkSignal)
			{
				BaseObject::write(_status);
				Sleep(1);
			}
		}
		else
		{
			BaseObject::write("QWERTYUI");
		}

	
		if (_recvBuff != NULL)
		{
			delete _recvBuff;
			_recvBuff = NULL;
		}



		//_status = status::NOT_INIT;
		//_error = error::NO_ERR;
		endWork();

		return err.value();
	}
	else
	{
		return error::CALL_DENIED;
	}
}

void Socket::connected()
{
	BaseObject::write("connectedSignal");
}

void Socket::connectError(const boost::system::error_code & ec, boost::asio::ip::tcp::endpoint remoteAddr)
{
	_setError(error::CONNECT_ERR);
	_status = status::END_WORK;
	close(1);
	BaseObject::write("connectErrorSignal");
}

void Socket::recvMessage(char*buff, std::size_t size)
{
	BaseObject::write("recvSignal=", 0);
	BaseObject::write(buff);
}

void Socket::recvError(const boost::system::error_code & ec)
{
	_setError(error::RECV_ERR);
	_status = status::END_WORK;
	close(1);
	BaseObject::write("recvErrorSigal");
	BaseObject::write(ec.message());
}

void Socket::sendMessage(unsigned long long messageId)
{

	BaseObject::write("sentSigal=", 0);
	BaseObject::write(messageId);
}

void Socket::sendError(const boost::system::error_code & ec)
{
	BaseObject::write("sentErrorSigal");
	_setError(error::SEND_ERR);
}


void Socket::endWork()
{
	BaseObject::write("endWorkSignal");
}

void Socket::_connectHandle(const boost::system::error_code & ec, boost::asio::ip::tcp::endpoint remoteAddr)
{
	if (!_error)
	{
		if (ec.value() != 0)
		{
			BaseObject::write(ec.value());
			BaseObject::write((char*)ec.message().c_str());
	
			connectError(ec, remoteAddr);
		}
		else
		{
			_status = status::CONNECTED;
			connected();
		}
	}
	else
	{
		_status = status::END_WORK;
		close(1);
	}
}

void Socket::_recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, std::size_t buffSize)
{
	if (!_error)
	{
		BaseObject::write("byte recv", 0);
		BaseObject::write(bytes_transferred);

		if (!ec.value()
			//&&
			//bytes_transferred != 0
			)
		{
			recvMessage(_recvBuff, bytes_transferred);//this block call

			_socket.async_receive(boost::asio::buffer(_recvBuff, buffSize),

				boost::bind(&Socket::_recvHandle, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffSize)
				 
				);
		}
		else
		{
			BaseObject::write("recv err");
			recvError(ec);
		}
	}
	else
	{
		_status = status::END_WORK;
		close(1);
	}
}

void Socket::_sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId)
{
	BaseObject::write("byte send", 0);
	BaseObject::write(bytes_transferred);

	if (!ec.value() && bytes_transferred != 0)
	{
		sendMessage(messageId);
	}
	else
	{
		sendError(ec);
		BaseObject::write("recv err");
	}
}

void Socket::_setError(error err)
{
	_error = err;
}