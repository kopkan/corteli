#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include "Header.h"



namespace corteli
{
	class BaseAsuncClient
	{
	public:
		enum status
		{
			NOT_INIT = 0, INIT_END = 1, BOUND = 2, CONNECTED = 3, RECV_STARTED = 4, HELLO_SAID = 5
		};

		enum error
		{
			NO_ERR = 0, BIND_ERR = 1, CONNECT_ERR = 2, RECV_ERR = 3, HELLO_ERR = 4, EXSPANCION_ERR = 5, DESTROY_ERR = 6, CLOSE = 100
		};

	public:
		void setIO(boost::asio::io_service* io);
		void setAttr(boost::asio::ip::tcp::endpoint remoteAddr, boost::asio::ip::tcp::endpoint localAddr, int buffSize);
		int start();
		int send(char* buff, int size = -1);
		int stop();
		int getError();


	protected:
		virtual void _sayHello();
		virtual void _newMessage(char* buff, int size);
		virtual void _endWork();

		virtual void _connectExpansion();//call in pool
		virtual void _chatExpansion();//call in pool
		void _setError(error err);

	private:
		boost::asio::io_service* _io;
		boost::asio::ip::tcp::socket* _cl;
		boost::asio::ip::tcp::endpoint _remoteAddr;
		boost::asio::ip::tcp::endpoint _localAddr;
		bool _isWork = false;
		bool _isStop = true;
		int _error = error::NO_ERR;
		int _status = status::NOT_INIT;
		char* _recvBuff;
		int _recvBuffSize;

		void _init(boost::asio::io_service* ioService);
		int _bind(boost::asio::ip::tcp::endpoint localAddr);
		void _conA(const boost::system::error_code & ec);
		void _connected();
		void _recvA(const boost::system::error_code & ec, std::size_t bytes_transferred);
		int _stop();
	};

}
