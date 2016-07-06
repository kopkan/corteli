#include "BaseClient.h"

using namespace corteli::network::socket::tcp;
using namespace corteli::network;

BaseClient::BaseClient(IoService* ioService, bool enableDebugMessage) : BaseObject(enableDebugMessage), _socket(ioService->getIoSevice())
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

int BaseClient::bind(corteli::network::Endpoint localEndpoint)
{
	if (_status >= status::INITED && _status < status::CONNECTED && !_error)
	{
		boost::system::error_code err;

		_socket.open(boost::asio::ip::tcp::v4(), err);
		//_socket.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		if (err.value())
		{
			BaseObject::write(err.value());
			BaseObject::write(err.message());
			_setError(error::BIND_ERR);
			return err.value();
		}

		boost::asio::ip::tcp::endpoint localAddr(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(localEndpoint.getCharIp()), localEndpoint.getPort()));
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


int BaseClient::connect(corteli::network::Endpoint remoteEndpoint)//////////////////////////////
{
	if (_status >= status::INITED && _status < status::CONNECTED && !_error)
	{
		_setStatus(status::CONNECT);

		boost::asio::ip::tcp::endpoint remoteAddr(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(remoteEndpoint.getCharIp()), remoteEndpoint.getPort()));


		_socket.async_connect(remoteAddr,
			
			boost::bind(&BaseClient::_connectHandle, this, boost::asio::placeholders::error, remoteEndpoint)
			);

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

boost::asio::ip::tcp::socket& BaseClient::getSocket()
{
	return _socket;
}

void BaseClient::_acceptorThisSocketConnected()
{
	_setStatus(status::CONNECTED);
}

unsigned long long BaseClient::getLastRecvMessageTime()
{
	return _lastRecvMessageTime;
}

unsigned long long BaseClient::getLastSentMessageId()
{
	return _lastSentMessageId;
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

int BaseClient::startRecv(std::size_t size)
{
	if (_status >= status::CONNECTED && _status < status::RECV_STARTED && !_error)
	{

		_recvBuff.resize(size);

		_setStatus(status::RECV_STARTED);

		_socket.async_receive(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()), ///////

			boost::bind(&BaseClient::_recvHandle, this,
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


unsigned long long BaseClient::send(corteli::base::container::Buffer<char> buffer, int flag)
{
	if (_status >= status::CONNECTED && _status < status::END_WORK && !_error)
	{
		_countSendWaiting++;

		_socket.async_send(boost::asio::buffer(buffer.getBuff(), buffer.getSize()), flag,

			boost::bind(&BaseClient::_sendHandle, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred,
				++_lastSentMessageId
				)
			);

		return _lastSentMessageId;
	}

	return 0;
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

		boost::system::error_code err;

		_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, err);
		_socket.close(err);

		if (_status >= status::CONNECT)
		{
			while ((_countSendWaiting != 0 || _status < status::END_WORK) && _status != status::CONNECTED && _ioService->getStatus()!=IoService::status::STOPPED)
			{
				BaseObject::write(_status);
				BaseObject::write(_countSendWaiting);
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

bool BaseClient::connectContainsError(const boost::system::error_code & ec, corteli::network::Endpoint remoteEndpoint)
{
	if (ec != 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void BaseClient::connected(corteli::network::Endpoint remoteEndpoint)
{
	BaseObject::write("connectedSignal");
}

void BaseClient::connectError(const boost::system::error_code & ec, corteli::network::Endpoint remoteEndpoint)
{
	if (ec.value() == 10061)
	{
		connect(remoteEndpoint);
	}
	else
	{
		_setError(error::CONNECT_ERR);
		close(true);
		BaseObject::write("connectErrorSignal");
	}
}

bool BaseClient::recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred)
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

void BaseClient::recvMessage(corteli::base::container::Buffer<char> buffer)
{
	BaseObject::write("recvSignal=", 0);
	BaseObject::write(buffer.getBuff());
}

void BaseClient::recvError(const boost::system::error_code & ec)
{
	//send("errrr", 6, 1);//fix bind problem
	_setError(error::RECV_ERR);
	BaseObject::write("recvErrorSigal");
	closeT(true);
	BaseObject::write(ec.message());
}

bool BaseClient::sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId)
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

void BaseClient::sentMessage(unsigned long long messageId)
{

	BaseObject::write("sentSigal=", 0);
	BaseObject::write(messageId);
}

void BaseClient::sendError(const boost::system::error_code & ec, unsigned long long messageId)
{
	BaseObject::write("sendErrorSigal");
	_setError(error::SEND_ERR);
}

void BaseClient::preClosing(bool closingFlag)
{
	BaseObject::write("preClosing");
}

void BaseClient::afterClosing(bool closingFlag)
{
	BaseObject::write("afterClosing");
}


void BaseClient::_connectHandle(const boost::system::error_code & ec, corteli::network::Endpoint remoteEndpoint)
{
	if (!_error)
	{
		if (connectContainsError(ec, remoteEndpoint))
		{
			BaseObject::write(ec.value());
			BaseObject::write((char*)ec.message().c_str());
	
			connectError(ec, remoteEndpoint);
		}
		else
		{
			_setStatus(status::CONNECTED);
			connected(remoteEndpoint);
		}
	}
	else
	{
		close(true);
	}
}

void BaseClient::_recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred)
{
	if (!_error)
	{
		if (!recvContainsError(ec, bytes_transferred))
		{
			recvMessage({ _recvBuff.getBuff(), bytes_transferred });//this block call

			_socket.async_receive(boost::asio::buffer(_recvBuff.getBuff(), _recvBuff.getSize()),

				boost::bind(&BaseClient::_recvHandle, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred)
				 
				);

			_lastRecvMessageTime = clock();
		}
		else
		{
			recvError(ec);
		}
	}
	else
	{
		close(true);
	}
}

void BaseClient::_sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId)
{
	/*
	BaseObject::write("byte send", 0);
	BaseObject::write(bytes_transferred);
	BaseObject::write(ec);
	*/

	if (!sendContainsError(ec, bytes_transferred, messageId))
	{
		sentMessage(messageId);
	}
	else
	{
		sendError(ec, messageId);
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