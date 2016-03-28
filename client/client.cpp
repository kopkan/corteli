#include "Client.h"

using namespace std;
using namespace boost::asio;
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

namespace corteli {

	Client::Client()
	{
		_init();
	}

	Client::Client(int port, char* ip)
	{
		_init();
		bind(port, ip);
	}

	Client::~Client()
	{
		clientSocket->close();
		delete clientSocket;
		delete io_service;
	}

	int Client::bind(int port, char* ip)
	{

		clientSocket->open(tcp::v4());
		tcp::endpoint myPoint(ip::address_v4::from_string(ip), port);
		clientSocket->bind(myPoint, err);

		return err.value();
	}

	int Client::connect(char* ip, int port)
	{

		tcp::endpoint servPoint(ip::address_v4::from_string(ip), port);
		clientSocket->connect(servPoint, err);

		return err.value();
	}

	int Client::send(char* buff, int size)
	{

		if (size == -1) {

			size = strlen(buff) + 1;
		}

		clientSocket->send(buffer(buff, size), 0, err);

		return err.value();

	}

	int Client::recv(char* buff, int size)
	{

		clientSocket->receive(buffer(buff, size), 0, err);

		lastMesTime = clock();

		return err.value();
	}




	bool Client::connectCycle(char* ip, int port, int limNoCon, int timeLimit, int sleepBetweenTry)
	{
		cout << ip << ' ' << port<<endl;

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

	void Client::recvCycle(int size) {

		while (1) {

			if (recv(buff, size) == 0) {
				_newMessage(buff, size);
			}
			else {
				cout << "END CONN!" << endl;
				break;
			}
		}
	}



	boost::thread* Client::recvCycleT(int size)
	{
		recvT = new boost::thread(&Client::recvCycle, this, size);
		recvT->detach();
		
		return recvT;
	}

	bool Client::checkOnline()
	{

		if (clock() < lastMesTime + 10000) {
			return true;
		}
		cout << lastMesTime << ' ' << clock() << endl;

		return false;
	}

	void Client::checkOnlineCycle()
	{

		while (checkOnline()) {

			Sleep(1000);
			cout << "time" << endl;
		}

		cout << "time err" << endl;

	}

	boost::thread* Client::checkOnlineCycleT()
	{

		checkOnlineT = new boost::thread(&Client::checkOnlineCycle, this);
		checkOnlineT->detach();
		return checkOnlineT;
	}


	void Client::_newMessage(char* buff, int size) {

		cout << buff << endl;

	}

	void Client::start(char* ip, int port, int buffSize, int time)
	{
		buff = new char[buffSize];
		connectCycle(ip, port);
		recvCycleT(buffSize);
		checkOnlineCycleT();

	}

	void Client::close()
	{
		_endConnection();
	}

	void Client::_init()
	{
		io_service = new boost::asio::io_service;
		clientSocket = new tcp::socket(*io_service);
		lastMesTime = clock();
	}


	char* strToChar(string str) {
	
		char* buff = new char[100];

		for (int i = str.size(); i >= 0; --i) {
		
			buff[i] = str[i];
		}

		return buff;
	}

	void Client::_endConnection() 
	{


		char* ip = strToChar(clientSocket->remote_endpoint().address().to_string());
		int port = clientSocket->remote_endpoint().port();


		cout << (int)ip[0] << (int)'1' << endl;

		clientSocket->close();

		
		//checkOnlineT->sleep();

		//boost::thread_group my_threads;


		delete checkOnlineT;
		delete recvT;
		//kill thread

		clientSocket = new tcp::socket(*io_service);
		lastMesTime = clock();

		cout << ip << endl;

		start(ip, port, 100, 1000);
		delete ip;
	}

};
