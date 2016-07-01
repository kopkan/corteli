#include "Socket.h"


using namespace corteli::network::socket::udp;

Socket::Socket(IoService * ioService, bool enableDebugMessage) : BaseObject(enableDebugMessage), _socket(ioService->getIoSevice())
{
	_ioService = ioService;
	_status = status::INITED;
}

Socket::~Socket()
{
	close();
}

int Socket::getStatus()
{
	return _status;
}

int Socket::getError()
{
	return _error;
}

int Socket::bind(boost::asio::ip::udp::endpoint localAddr)
{
	if (_status >= status::INITED && _status < status::RECV_STARTED && !_error)
	{
		boost::system::error_code err;

		_socket.open(boost::asio::ip::udp::v4(), err);
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

int Socket::startRecv(std::size_t size)
{
	if (_status >= status::BOUND && _status < status::RECV_STARTED && !_error)
	{

		_recvBuff = new char[size];

		_status = status::RECV_STARTED;

		boost::asio::ip::udp::endpoint *_remoteAddr=new boost::asio::ip::udp::endpoint;
		//_recvBuff и remoteAddr возможна перезапись WARNNN!!!!!
		_socket.async_receive_from(boost::asio::buffer(_recvBuff, size), *_remoteAddr,

			boost::bind(&Socket::_recvHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, size, *_remoteAddr)

			);

		return error::NO_ERR;

	}
	else
	{
		return error::CALL_DENIED;
	}
}

unsigned long long Socket::sendTo(char * buff, int size, boost::asio::ip::udp::endpoint remoteAddr, int flag)
{
	if (_status >= status::INITED && _status < status::END_WORK && !_error)
	{

		_socket.async_send_to(boost::asio::buffer(buff, size), remoteAddr, flag,

			boost::bind(&Socket::_sendHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				++_lastSentMessageId
				)
			);

		return _lastSentMessageId;
	}

	return 0;
}

int Socket::close()
{
	if (_error != error::CLOSE)
	{

		_setError(error::CLOSE);
		boost::system::error_code err;

		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		_socket.close(err);

		if (_status >= status::RECV_STARTED)
		{
			while (_status < status::END_WORK && _ioService->getStatus() != IoService::status::STOPPED)
			{
				BaseObject::write(_status);
				Sleep(1);
			}
		}


		if (_recvBuff != NULL)
		{
			delete _recvBuff;
			_recvBuff = NULL;
		}

		endWork();
		_status = status::CLOSED;

		return err.value();
	}
	else
	{
		return error::CALL_DENIED;
	}
}

int Socket::closeT()
{
	if (_error != error::CLOSE)
	{
		std::thread(&Socket::close, this).detach();
		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

boost::asio::ip::udp::socket& Socket::getSocket()
{
	return _socket;
}

void Socket::reload()
{
	close();
	_status = status::INITED;
	_error = error::NO_ERR;
}

void Socket::recvMessage(char * buff, std::size_t size, boost::asio::ip::udp::endpoint remoteAddr)
{
	BaseObject::write("recvSignal=", 0);
	BaseObject::write(buff);
}

void Socket::recvError(const boost::system::error_code & ec, boost::asio::ip::udp::endpoint remoteAddr)
{
	_setError(error::RECV_ERR);
	_status = status::END_WORK;
	close();
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
	BaseObject::write("sendErrorSigal");
	//_setError(error::SEND_ERR);
	//close();
}

void Socket::endWork()
{
	BaseObject::write("endWorkSignal");
}

void Socket::_recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, std::size_t buffSize, boost::asio::ip::udp::endpoint remoteAddr)
{

	if (!_error)
	{
		BaseObject::write("byte recv", 0);
		BaseObject::write(bytes_transferred);
		BaseObject::write(remoteAddr);

		if (!ec.value()
			//&&
			//bytes_transferred != 0
			)
		{


			recvMessage(_recvBuff, bytes_transferred, remoteAddr);//this block call

			_socket.async_receive_from(boost::asio::buffer(_recvBuff, buffSize), remoteAddr,

				boost::bind(&Socket::_recvHandle, this, 
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred,
					buffSize, remoteAddr)

				);
		}
		else
		{
			BaseObject::write("recv err");
			recvError(ec, remoteAddr);
		}
	}
	else
	{
		_status = status::END_WORK;
		close();
	}
}

void Socket::_sendHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId)
{
	BaseObject::write("byte send", 0);
	BaseObject::write(bytes_transferred);

	if (!ec.value() && bytes_transferred != 0)
	{
		if (_status == status::INITED && !_error)
		{
			_status = status::BOUND;
		}

		sendMessage(messageId);
	}
	else
	{
		BaseObject::write("send err");
		sendError(ec);
	}
}

void Socket::_setError(error err)
{
	if (_error != error::CLOSE)
	{
		_error = err;
	}
}
