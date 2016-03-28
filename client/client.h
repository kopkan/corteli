#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace corteli {

	class Client {

	public:
		Client();
		Client(int port, char* ip = 0);
		~Client();
		int bind(int port, char* ip = 0);
		int connect(char* ip, int port = 0);
		int send(char* buff, int size = -1);
		int recv(char* buff, int size);



		bool connectCycle(char* ip, int port, int limNoCon = -1, int timeLimit = -1, int sleepBetweenTry = 100);
		void recvCycle(int size);

		boost::thread* recvCycleT(int size);

		bool checkOnline();
		void checkOnlineCycle();

		boost::thread* checkOnlineCycleT();

		void start(char* ip, int port, int buffSize, int time);
		void close();

	private:
		boost::asio::io_service* io_service;
		boost::asio::ip::tcp::socket* clientSocket;

		boost::thread* checkOnlineT;
		boost::thread* recvT;

		boost::system::error_code err;
		int lastMesTime;
		char* buff;

		void _init();
		virtual void _newMessage(char* buff, int size);
		virtual void _endConnection();
	};
};
