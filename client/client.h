#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>

#include "Header.h"

namespace corteli {

	class BaseClient {

	public:
		BaseClient(boost::asio::io_service* ioService = NULL);
		~BaseClient();

		int send(char* buff, int size = -1);

		int start();
		int startT();

		int conInfo(boost::asio::ip::tcp::endpoint remoteAddr, int recvBuffSize, boost::asio::ip::tcp::endpoint localAddr);

		void close();

	protected:
		boost::asio::io_service* _ioService;
		boost::asio::ip::tcp::socket* _clientSocket;
		boost::asio::ip::tcp::endpoint _localAddr;
		boost::asio::ip::tcp::endpoint _remoteAddr;

		bool _isNewIoService=false;
		bool _isWork=false;
		int _error = 0;


		int _status=0;
		int _recvBuffSize;

		boost::thread* _workT;

		void _init(boost::asio::io_service* ioService);
		int _bind(boost::asio::ip::tcp::endpoint localAddr);
		int _connect(boost::asio::ip::tcp::endpoint remoteAddr);
		bool _connectCycle(boost::asio::ip::tcp::endpoint remoteAddr, int limNoCon = -1, int timeLimit = -1, int sleepBetweenTry = 100);
		int _recv(char* buff, int buffSize);
		void _recvCycle(int buffSize);
		void _recvCycleT(int buffSize);

		virtual int _expansion();
		int _destroy();

		virtual void _newMessage(char* buff, int size);
		virtual void _endWork();



	};
};
