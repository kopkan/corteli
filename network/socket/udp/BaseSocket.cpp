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
	while (_countCloseWaiting)
	{
		Sleep(1);
	}
}

int BaseSocket::getStatus()
{
	return _status;
}

int BaseSocket::getError()
{
	return _error;
}

int BaseSocket::bind(corteli::network::Endpoint localEndpoint)
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

		boost::asio::ip::udp::endpoint localAddr(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(localEndpoint.getCharIp()), localEndpoint.getPort()));
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

unsigned long long BaseSocket::sendTo(corteli::base::container::Buffer<char> buffer, corteli::network::Endpoint remoteEndpoint, int flag)
{
	if (_status >= status::INITED && _status < status::END_WORK && !_error)
	{
		_countSendWaiting++;

		boost::asio::ip::udp::endpoint remoteAddr(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(remoteEndpoint.getCharIp()), remoteEndpoint.getPort()));

		_socket.async_send_to(boost::asio::buffer(buffer.getBuff(), buffer.getSize()), remoteAddr, flag,

			boost::bind(&BaseSocket::_sendHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				++_lastSentMessageId, remoteEndpoint
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

bool BaseSocket::recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, corteli::network::Endpoint remoteEndpoint)
{

	BaseObject::write("byte recv", 0);
	BaseObject::write(bytes_transferred);
	BaseObject::write(_remoteAddr, 0);
	BaseObject::write(ec.value(), 0);


	if (ec.value() != 0 || bytes_transferred <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseSocket::recvMessage(corteli::base::container::Buffer<char> buffer, corteli::network::Endpoint remoteEndpoint)
{
	BaseObject::write("recvSignal=", 0);
	BaseObject::write(buffer.getBuff());
	BaseObject::write(remoteEndpoint.getName(), 0);
	BaseObject::write(remoteEndpoint.getPort());
}

void BaseSocket::recvError(const boost::system::error_code & ec, corteli::network::Endpoint remoteEndpoint)
{
	if (ec.value() != 10061)
	{
		_setError(error::RECV_ERR);
		closeT(true);
	}
	BaseObject::write("recvErrorSigal");
	BaseObject::write(ec.message());
}

bool BaseSocket::sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId, corteli::network::Endpoint remoteEndpoint)
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

void BaseSocket::sentMessage(unsigned long long messageId, corteli::network::Endpoint remoteEndpoint)
{
	BaseObject::write("sentSigal=", 0);
	BaseObject::write(messageId);
}

void BaseSocket::sendError(const boost::system::error_code & ec, unsigned long long messageId, corteli::network::Endpoint remoteEndpoint)
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
		corteli::network::Endpoint remoteEndpoint((char*)_remoteAddr.address().to_string().c_str(), _remoteAddr.port());

		if (!recvContainsError(ec, bytes_transferred, remoteEndpoint))
		{
			recvMessage({ _recvBuff.getBuff(), bytes_transferred }, remoteEndpoint);//this block call


			_socket.async_receive_from(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()), _remoteAddr,

				boost::bind(&BaseSocket::_recvHandle, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)

				);
		}
		else
		{
			recvError(ec, remoteEndpoint);
		}
	}
	else
	{
		close(true);
	}
}

void BaseSocket::_sendHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId, corteli::network::Endpoint remoteEndpoint)
{
	BaseObject::write("byte send", 0);
	BaseObject::write(bytes_transferred, 0);
	BaseObject::write(remoteEndpoint.getName(), 0);
	BaseObject::write(remoteEndpoint.getPort(), 0);

	if (!sendContainsError(ec, bytes_transferred, messageId, remoteEndpoint))
	{
		if (_status == status::INITED && !_error)
		{
			_status = status::BOUND;
		}

		sentMessage(messageId, remoteEndpoint);
	}
	else
	{
		BaseObject::write("send err");
		sendError(ec, messageId, remoteEndpoint);
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