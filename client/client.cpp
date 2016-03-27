#include "Client.h"

using namespace std;
using namespace boost::asio;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace corteli {

		Client::Client() {

			io_service = new boost::asio::io_service;
			clientSocket = new tcp::socket(*io_service);
			lastMesTime = clock();
		}

		Client::~Client() {

			clientSocket->close();
			delete clientSocket;
			delete io_service;
		}

		int Client::bind(char* ip, int port) {

			clientSocket->open(tcp::v4());
			tcp::endpoint myPoint(ip::address_v4::from_string(ip), port);
			clientSocket->bind(myPoint, err);

			return err.value();
		}

		int Client::connect(char* ip, int port) {

			tcp::endpoint servPoint(ip::address_v4::from_string(ip), port);
			clientSocket->connect(servPoint, err);

			return err.value();
		}

		int Client::send(char* buff, int size) {

			if (size == -1) {

				size = strlen(buff) + 1;
			}

			clientSocket->send(buffer(buff, size), 0, err);

			return err.value();

		}

		int Client::recv(char* buff, int size) {

			clientSocket->receive(buffer(buff, size), 0, err);

			lastMesTime = clock();

			return err.value();
		}




		bool Client::connectCycle(char* ip, int port, int limNoCon, int timeLimit, int sleepBetweenTry) {

			int startTime = clock();
			int nowTime;

			for (int i = limNoCon; i > 0 || limNoCon == -1; --i) {

				nowTime = clock();
				cout << "try con" << endl;
				if ((nowTime - startTime > timeLimit) || timeLimit == -1) {

					if (connect(ip, port) == 0) {
						return true;
					}

					Sleep(sleepBetweenTry);
				}
			}
			return false;
		}

		void Client::recvCycle(char* buff, int size) {

			while (1) {

				if (recv(buff, size) > 0) {
					newMessage(buff, size);
				}
				else {
					cout << "END CONN!" << endl;
					break;
				}
			}
		}



		boost::thread* Client::recvCycleT(char* buff, int size) {

			boost::thread* clRecvThread;
			clRecvThread = new boost::thread(&Client::recvCycle, this, buff, size);
			clRecvThread->detach();
			return clRecvThread;
		}

		bool Client::checkOnline()
		{

			if (clock() < lastMesTime+10000) {
				return true;
			}
			cout << lastMesTime << ' ' << clock() << endl;

			return false;
		}

		void Client::checkOnlineCycle() {

			while (checkOnline()) {
			
				Sleep(1000);
				cout << "time" << endl;
			}

			cout << "time err"<<endl;

		}

		boost::thread* Client::checkOnlineCycleT(){

			boost::thread* clRecvThread;
			clRecvThread = new boost::thread(&Client::checkOnlineCycle, this);
			clRecvThread->detach();
			return clRecvThread;

		}




		void Client::newMessage(char* buff, int size) {

			cout << buff << endl;

		}

		void Client::start(char* buff, int size, int time)
		{
			recvCycleT(buff, size);
			checkOnlineCycleT();

		}

};
