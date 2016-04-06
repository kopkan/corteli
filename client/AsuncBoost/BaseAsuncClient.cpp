#include "BaseAsuncClient.h"

using namespace std;

namespace corteli
{

	void BaseAsuncClient::setIO(boost::asio::io_service* io)
	{
		_io = io;
	}

	void BaseAsuncClient::setAttr(boost::asio::ip::tcp::endpoint remoteAddr, boost::asio::ip::tcp::endpoint localAddr, int buffSize)
	{
		_remoteAddr = remoteAddr;
		_localAddr = localAddr;
		_recvBuffSize = buffSize;
	}

	int BaseAsuncClient::start()
	{
		//мьютекс

		if (!_isWork) 
		{
			_isWork = true;
			_isStop = false;
			_error = error::NO_ERR;
			_status = status::NOT_INIT;

			_init(_io);
			_bind(_localAddr);


			//conCheckT
			_cl->async_connect(_remoteAddr,
				boost::bind(&BaseAsuncClient::_conA, this, boost::asio::placeholders::error)
				);

		}
		else
		{
			return 100;
		}
			
	}

	int BaseAsuncClient::send(char* buff, int size)
	{
		if (!_error) 
		{
			if (_status >= status::CONNECTED)
			{
				boost::system::error_code err;

				if (size == -1) 
				{
					size = strlen(buff) + 1;
				}

				_cl->send(boost::asio::buffer(buff, size), 0, err);

				return err.value();
			}
			else 
			{
				return 101;
			}
		}
		else
		{
			return 100;
		}
	}

	int BaseAsuncClient::stop()
	{
		//мьютекс

		if (_isWork) {
			_setError(error::CLOSE);
		}

		return _stop();
	}

	int BaseAsuncClient::getError()
	{
		return _error;
	}

	//protected
	void BaseAsuncClient::_sayHello()
	{
		cout << "say Hello" << endl;
		send("Hello server");
	}

	void BaseAsuncClient::_newMessage(char* buff, int size)
	{
		cout << "recv=" <<size<< buff << endl;
	}

	void BaseAsuncClient::_endWork()
	{
		cout << "_endWork" << endl;
	}

	void BaseAsuncClient::_connectExpansion()
	{
		cout << "_connectExpansion" << endl;
	}
	void BaseAsuncClient::_chatExpansion() 
	{
		cout << "_chatExpansion" << endl;
	}

	void BaseAsuncClient::_setError(error err)
	{
		_error = err;
		if (err != error::CLOSE)
		{
			_isStop = true;
			_stop();
		}
	}

	//private
	void BaseAsuncClient::_init(boost::asio::io_service* ioService)
	{
		_cl = new boost::asio::ip::tcp::socket(*ioService);
		_status = status::INIT_END;
	}

	int BaseAsuncClient::_bind(boost::asio::ip::tcp::endpoint localAddr)
	{
		boost::system::error_code err;
	
		_cl->open(boost::asio::ip::tcp::v4(), err);


		if (err.value() != 0)
		{
			cout << "open err";
			_setError(error::BIND_ERR);
			return err.value();
		}


		_cl->bind(localAddr, err);
		if (err.value() != 0)
		{
			cout << "bind err";
			_setError(error::BIND_ERR);
			return err.value();
		}

		_status = status::BOUND;
		return 0;
	}

	void BaseAsuncClient::_conA(const boost::system::error_code & ec)
	{
		cout << ec << endl;
		cout << ec.message() << endl;


		if (!_error)
		{
			if (ec != 0)
			{
				Sleep(1);

				_cl->async_connect(_remoteAddr,

					boost::bind(&BaseAsuncClient::_conA, this,
						boost::asio::placeholders::error
						)
					);
			}
			else
			{
				_connected();
			}
		}
		else
		{
			_isStop = true;
		}

	}

	void BaseAsuncClient::_connected()
	{
		_status = status::CONNECTED;

		_recvBuff = new char[_recvBuffSize];

		_cl->async_receive(boost::asio::buffer(_recvBuff, _recvBuffSize), 0,

			boost::bind(&BaseAsuncClient::_recvA, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred)
			);
		_status = status::RECV_STARTED;

		_sayHello();
		_status = status::HELLO_SAID;
		//expancionT
	}

	void BaseAsuncClient::_recvA(const boost::system::error_code & ec, std::size_t bytes_transferred)
	{
		cout << "r=" << bytes_transferred << ' ' << _recvBuff << endl;

		if (!_error)
		{
			if (ec.value() == 0)
			{

				_newMessage(_recvBuff, bytes_transferred);//this block oper

				_cl->async_receive(boost::asio::buffer(_recvBuff, 1000), 0,

					boost::bind(&BaseAsuncClient::_recvA, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred)

					);
			}
			else
			{
				_setError(error::RECV_ERR);
				cout << "stop" << endl;
			}
		}
		else
		{
			_isStop = true;
		}
	}

	int BaseAsuncClient::_stop()
	{
		if (_isWork) {
			cout << "stop st" << endl;

			_cl->close();
			cout << "stop end" << endl;

			while (!_isStop) {
				Sleep(1);
			}
			delete _cl;
			delete _recvBuff;

			_isWork = false;

			_endWork();
			return 0;
		}
		else
		{
			return 100;
		}
	}

}
