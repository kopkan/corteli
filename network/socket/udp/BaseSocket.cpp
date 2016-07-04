#include "BaseSocket.h"


using namespace corteli::network::socket::udp;
using namespace corteli::network;

BaseSocket::BaseSocket(IoService * ioService, bool enableDebugMessage) : BaseObject(enableDebugMessage), _socket(ioService->getIoSevice())
{
	_ioService = ioService;
	_setStatus(status::INITED);
}

BaseSocket::~BaseSocket()
{
	close(0, 1);
	waitClose();
}

int BaseSocket::getStatus()
{
	return _status;
}

int BaseSocket::getError()
{
	return _error;
}

int BaseSocket::bind(char* ip, unsigned short port)
{
	if (_status >= status::INITED && _status < status::RECV_STARTED && !_error)
	{
		boost::system::error_code err;

		_socket.open(boost::asio::ip::udp::v4(), err);
		//_socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		if (err.value())
		{
			BaseObject::write(err.value());
			BaseObject::write(err.message());
			_setError(error::BIND_ERR);
			return err.value();
		}

		boost::asio::ip::udp::endpoint localAddr(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(ip), port));
		_socket.bind(localAddr, err);

		if (err.value())
		{
			BaseObject::write(err.value());
			BaseObject::write(err.message());
			_setError(error::BIND_ERR);
			return err.value();
		}

		_setStatus(status::BOUND);
		return error::NO_ERR;

	}
	else
	{
		return error::CALL_DENIED;
	}
}

int BaseSocket::startRecv(std::size_t size)
{
	if (_status >= status::BOUND && _status < status::RECV_STARTED && !_error)
	{

		_recvBuff.resize(size);
		_setStatus(status::RECV_STARTED);

		//_recvBuff и remoteAddr возможна перезапись WARNNN!!!!!
		_socket.async_receive_from(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()), _remoteAddr,

			boost::bind(&BaseSocket::_recvHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)

			);

		return error::NO_ERR;

	}
	else
	{
		return error::CALL_DENIED;
	}
}

unsigned long long BaseSocket::sendTo(char * buff, int size, char* ip, unsigned short port, int flag)
{
	if (_status >= status::INITED && _status < status::END_WORK && !_error)
	{
		_countSendWaiting++;

		boost::asio::ip::udp::endpoint remoteAddr(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(ip), port));

		_socket.async_send_to(boost::asio::buffer(buff, size), remoteAddr, flag,

			boost::bind(&BaseSocket::_sendHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				++_lastSentMessageId, remoteAddr
				)
			);

		return _lastSentMessageId;
	}
	else
	{
		return 0;
	}
}

int BaseSocket::close(bool setEndWorkStatus, bool closingFlag)
{
	if (setEndWorkStatus)
	{
		_setStatus(status::END_WORK);
	}

	if (_error != error::CLOSE)
	{
		preClosing(closingFlag);
		_setError(error::CLOSE);

		boost::system::error_code err;

		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		_socket.close(err);


		if (_status >= status::RECV_STARTED)
		{

			while ((_countSendWaiting!=0 || _status < status::END_WORK) && _ioService->getStatus() != IoService::status::STOPPED)
			{
				BaseObject::write(_status);
				Sleep(1);
			}
		}


		_setStatus(status::CLOSED);

		afterClosing(closingFlag);

		return err.value();
	}
	else
	{
		return error::CALL_DENIED;
	}
}

int BaseSocket::closeT(bool setEndWorkStatus, bool closingFlag)
{
	if (setEndWorkStatus)
	{
		_setStatus(status::END_WORK);
	}

	if (_error != error::CLOSE)
	{
		std::thread(&BaseSocket::close, this, setEndWorkStatus, closingFlag).detach();
		return error::NO_ERR;
	}
	else
	{
		return error::CALL_DENIED;
	}
}

bool BaseSocket::waitClose(time_t time)
{
	_countCloseWaiting++;

	time_t startTime = clock();

	while (clock()- startTime<time || time==0)
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

boost::asio::ip::udp::socket& BaseSocket::getSocket()
{
	return _socket;
}

void BaseSocket::reload()
{
	close();
	waitClose();

	while (_countCloseWaiting)
	{
		Sleep(1);
	}

	_setStatus(status::INITED);
	_error = error::NO_ERR;
}

bool BaseSocket::recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, boost::asio::ip::udp::endpoint remoteAddr)
{
	if (ec.value() != 0 || bytes_transferred <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseSocket::recvMessage(char *buff, std::size_t size, boost::asio::ip::udp::endpoint remoteAddr)
{
	BaseObject::write("recvSignal=", 0);
	BaseObject::write(buff);
	BaseObject::write(remoteAddr);
}

void BaseSocket::recvError(const boost::system::error_code & ec, boost::asio::ip::udp::endpoint remoteAddr)
{
	_setError(error::RECV_ERR);
	closeT(true);///////////////////////////////////////////////////
	BaseObject::write("recvErrorSigal");
	BaseObject::write(ec.message());
}

bool BaseSocket::sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
{
	if (ec.value() != 0 || bytes_transferred <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseSocket::sentMessage(unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
{
	BaseObject::write("sentSigal=", 0);
	BaseObject::write(messageId);
}

void BaseSocket::sendError(const boost::system::error_code & ec, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
{
	BaseObject::write("sendErrorSigal");
	//_setError(error::SEND_ERR);
	//close();
}

void BaseSocket::preClosing(bool closingFlag)
{
	BaseObject::write("preClosing");
}

void BaseSocket::afterClosing(bool closingFlag)
{
	BaseObject::write("afterClosing");
}


void BaseSocket::_recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	if (!_error)
	{
		BaseObject::write("byte recv", 0);
		BaseObject::write(bytes_transferred);
		BaseObject::write(_remoteAddr, 0);


		if (!recvContainsError(ec, bytes_transferred, _remoteAddr))
		{
			recvMessage(_recvBuff.getBuff(), bytes_transferred, _remoteAddr);//this block call


			_socket.async_receive_from(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()), _remoteAddr,

				boost::bind(&BaseSocket::_recvHandle, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)

				);
		}
		else
		{
			BaseObject::write("recv err");
			recvError(ec, _remoteAddr);
		}
	}
	else
	{
		close(true);
	}
}

void BaseSocket::_sendHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
{
	BaseObject::write("byte send", 0);
	BaseObject::write(bytes_transferred, 0);
	BaseObject::write(remoteAddr);

	if (!sendContainsError(ec, bytes_transferred, messageId, remoteAddr))
	{
		if (_status == status::INITED && !_error)
		{
			_status = status::BOUND;
		}

		sentMessage(messageId, remoteAddr);
	}
	else
	{
		BaseObject::write("send err");
		sendError(ec, messageId, remoteAddr);
	}


	_countSendWaiting--;
}

void BaseSocket::_setError(error err)
{
	if (_error != error::CLOSE)
	{
		_error = err;
	}
}

void BaseSocket::_setStatus(status st)
{
	_status = st;
}