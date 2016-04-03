#include "BaseClient.h"


using namespace std;

namespace corteli {

	BaseClient::BaseClient(boost::asio::io_service* ioService)
	{
		_init(ioService);
	}

	BaseClient::~BaseClient()
	{
		startT();
		close();
	}

	void BaseClient::_init(boost::asio::io_service* ioService)
	{
		if (ioService == NULL)
		{
			_ioService = new boost::asio::io_service;
			_isNewIoService = true;
		}
		else
		{
			_ioService = ioService;
		}

		_clientSocket = new boost::asio::ip::tcp::socket(*_ioService);
	}



	int BaseClient::_bind(boost::asio::ip::tcp::endpoint localAddr)
	{
		boost::system::error_code err;

		_clientSocket->open(boost::asio::ip::tcp::v4(), err);
		if (err.value() != 0)
		{
			cout << "open err";
			return err.value();
		}


		_clientSocket->bind(localAddr, err);
		if (err.value() != 0)
		{
			cout << "bind err";
		}

		return err.value();
	}

	int BaseClient::_connect(boost::asio::ip::tcp::endpoint remoteAddr)
	{
		boost::system::error_code err;
		_clientSocket->connect(remoteAddr, err);

		return err.value();
	}

	bool BaseClient::_connectCycle(boost::asio::ip::tcp::endpoint remoteAddr, int limNoCon, int timeLimit, int sleepBetweenTry)
	{

		int startTime = clock();
		int nowTime;

		for (int i = limNoCon; i > 0 || limNoCon == -1; --i) {

			if (_error != 0) {//bad
			
				return true;
			}

			nowTime = clock();
			cout << "try con" << endl;
			if ((nowTime - startTime > timeLimit) || timeLimit == -1) {

				int con = _connect(remoteAddr);

				if (con == 0 || con == 10056) {//ok or already connected

					return true;
				}
				Sleep(sleepBetweenTry);
			}
		}

		return false;
	}

	int BaseClient::send(char* buff, int size)
	{
		if (_status > 2 && _status < 5)
		{
			boost::system::error_code err;

			if (size == -1) {
				size = strlen(buff) + 1;
			}

			_clientSocket->send(boost::asio::buffer(buff, size), 0, err);

			return err.value();
		}
		else
		{
			return 100;
		}
	}


	void BaseClient::_recvCycleT(int buffSize)
	{

		boost::thread* _recvT = new boost::thread(&BaseClient::_recvCycle, this, buffSize);
		_recvT->detach();

	}

	int BaseClient::start()
	{
		//мьютекс

		if (_isWork) {
			return 100;
		}

		_isWork = true;
		_error = 0;
		_status = 0;

		while (_isWork && _error == 0) {

			switch (_status)
			{
			case 0:
			{
				_init(_ioService);
				_status = 1;
			}
			case 1:
			{
				if (_bind(_localAddr) == 0)
				{
					_status = 2;
				}
				else
				{
					_error = 1;
				}
				break;
			}
			case 2:
			{
				if (_connectCycle(_remoteAddr) == true)
				{
					_status = 3;
				}
				else
				{
					_error = 2;
				}
				break;
			}
			case 3:
			{
				_recvCycleT(_recvBuffSize);
				_status = 4;
				break;
			}
			case 4:
			{
				cout << "expansion" << endl;
				if (_expansion() != 0)
				{
					_error = 4;
				}
				break;
			}
			default:
				break;
			}
		}


		if (_error > 0) {
			cout << "_destroy" << endl;
			if (_destroy() != 0)
			{
				_error = 5;
			}

			_isWork = false;
			_status = 0;
			//_endWork();
		}


		cout << "err=" <<_error << endl;

		return _error;
	}

	int BaseClient::startT()
	{
		//мьютекс

		if (_isWork) 
		{
			return 100;
		}

		_workT= new boost::thread(&BaseClient::start, this);
		_workT->detach();


		
		while (!_isWork) 
		{
			Sleep(1);
		}

		return 0;
	}

	int BaseClient::conInfo(boost::asio::ip::tcp::endpoint remoteAddr, int recvBuffSize, boost::asio::ip::tcp::endpoint localAddr)
	{
		_remoteAddr = remoteAddr;
		_recvBuffSize = recvBuffSize;
		_localAddr = localAddr;

		return 0;
	}

	void BaseClient::close()
	{
		//мьютекс
		cout << "CLOSE" << endl;
		if (_isWork)
		{
			_error = 100;
			while (_isWork != 0) {
			
				Sleep(1);
			}
		}
		else
		{
			cout << "no work" << endl;
		}
	}

	int BaseClient::_expansion()
	{

		Sleep(100);

		return 0;
	}


	int BaseClient::_destroy()
	{
		boost::system::error_code err;

		try
		{
			_clientSocket->close(err);
			delete _clientSocket;
			_clientSocket = NULL;

			if (_isNewIoService) 
			{
				delete _ioService;
				_ioService = NULL;
			}
		}
		catch (...) 
		{
			cout << "destroy err" << endl;
		}

		return err.value();
	}

	int BaseClient::_recv(char* buff, int size)
	{
		boost::system::error_code err;
		_clientSocket->receive(boost::asio::buffer(buff, size), 0, err);
		return err.value();
	}

	void BaseClient::_recvCycle(int size)
	{
		char* buff = new char[size];

		while (1) {

			if (_recv(buff, size) == 0) {
				_newMessage(buff, size);
			}
			else {
				cout << "END CONN!" << endl;
				break;
			}
		}

		delete buff;
	}


	void BaseClient::_newMessage(char* buff, int size)
	{
		cout << "recv=" << buff << endl;
	}

	void BaseClient::_endWork()
	{
		cout << "_endWork" << endl;
	}


};
