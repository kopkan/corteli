#pragma once
#include <boost/asio.hpp>
#include <boost/thread.hpp>

namespace corteli {

	class Client {

	public:
		Client();
		~Client();
		int bind(char* ip, int port = 0);
		int connect(char* ip, int port = 0);
		int send(char* buff, int size = -1);
		int recv(char* buff, int size);



		bool connectCycle(char* ip, int port, int limNoCon = -1, int timeLimit = -1, int sleepBetweenTry = 100);
		void recvCycle(char* buff, int size);

		boost::thread* recvCycleT(char* buff, int size);

		bool checkOnline();
		void checkOnlineCycle();

		boost::thread* checkOnlineCycleT();


		virtual void newMessage(char* buff, int size);
		void start(char* buff, int size, int time);

	private:
		boost::asio::io_service* io_service;
		boost::asio::ip::tcp::socket* clientSocket;
		boost::system::error_code err;
		int lastMesTime;
	};
};
