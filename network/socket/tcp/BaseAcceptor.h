#pragma once


#include "BasePool.h"


template<typename T>
class BaseAcceptor : protected BasePool<T>
{
public:
	enum status
	{
		NOT_INIT = 0, INITED = 1, BOUND = 2, ACCEPT_STARTED = 3, END_WORK = 4, CLOSED = 7
	};
	enum error
	{
		NO_ERR = 0, BIND_ERR = 1, LISTEN_ERR = 2, ACCEPT_ERR = 3, CLOSE = 100, CALL_DENIED = 200
	};

	BaseAcceptor(bool enableDebugMessage = true) : BasePool(enableDebugMessage), _server(_ioService.getIoSevice())
	{
		runExpansionT();
		_status = status::INITED;
	}

	~BaseAcceptor()
	{
		stop();
	}

	int bind(boost::asio::ip::tcp::endpoint localAddr)
	{
		if (_status == status::INITED && _error == error::NO_ERR)
		{
			boost::system::error_code err;

			_server.open(boost::asio::ip::tcp::v4(), err);
			_server.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
			if (err.value())
			{
				BaseObject::write(err.value());
				BaseObject::write(err.message());
				_setError(error::BIND_ERR);
				return err.value();
			}

			_server.bind(localAddr, err);

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

	int startAccept(int queueSize=100)
	{
		if (_status == status::BOUND && _error == error::NO_ERR)
		{
			boost::system::error_code err;
			_server.listen(queueSize, err);

			_status = status::ACCEPT_STARTED;

			if (err.value())
			{
				BaseObject::write(err.value());
				BaseObject::write(err.message());
				_setError(error::LISTEN_ERR);
				return err.value();
			}

			nextAccept();
			return error::NO_ERR;
		}
		else
		{
			return error::CALL_DENIED;
		}
	}

	int close()
	{
		if (_error != error::CLOSE)
		{
			_setError(error::CLOSE);
			_server.close();

			if (_status >= status::ACCEPT_STARTED)
			{
				while (_status < status::END_WORK && _ioService.getStatus() != IoService::status::STOPPED)
				{
					Sleep(1);
				}
			}


			BasePool::stop();
			cout << _clientList.size() << endl;
			return error::NO_ERR;
		}
		else
		{
			return error::CALL_DENIED;
		}
	}

	void reload()
	{
		close();
		_error = error::NO_ERR;
		_status = status::INITED;
	}

protected:
	virtual void accepted(T* socket)
	{
		socket->_acceptorThisSocketConnected();
		socket->startRecv(500);
		//socket->send("Hello", 6);
		//Sleep(10000);
		//socket->closeT();
	}

	virtual void acceptError(const boost::system::error_code& ec, T* socket)
	{
		BaseObject::write("acceptError");
	}

	void nextAccept()
	{
		T* socket = BasePool::newClient();

		_server.async_accept(socket->getSocket(),

			boost::bind(&BaseAcceptor::_acceptHandle, this,
				boost::asio::placeholders::error, socket
				)
			);
	}

private:
	void _setError(error err)
	{
		if (_error != error::CLOSE)
		{
			_error = err;
		}
	}

	void _acceptHandle(const boost::system::error_code & ec, T* socket)
	{
		if (_error == error::NO_ERR)
		{
			if (ec.value() == 0)
			{
				accepted(socket);
				nextAccept();
			}
			else
			{
				acceptError(ec, socket);
			}
		}
		else
		{
			_status = status::END_WORK;
			close();
		}
	}

	boost::asio::ip::tcp::acceptor _server;
	status _status = status::NOT_INIT;
	error _error = error::NO_ERR;
};
