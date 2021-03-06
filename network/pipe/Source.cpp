#include <boost/asio.hpp>
#include <boost/thread.hpp>


#include "BaseAcceptor.h"

using namespace corteli::network::pipe;
using namespace std;

void main()
{
	//IoService io;
	//io.startT();

	//BaseClient b(&io, true);
	SetConsoleCP(1251);// ��������� ������� �������� win-cp 1251 � ����� �����
	SetConsoleOutputCP(1251);

	BasePool<BaseClient> pool;
	pool.runExpansionT();

	BaseClient* cl = pool.newClient();


	
	while (1)
	{
		cout << "connect" << cl->connectT("mypipe") << endl;
		cout << "waitConnected=" << cl->waitConnect(1000) << endl;
		cout << "startRecv=" << cl->startRecv(1000) << endl;

		char buff[1000];
		cin.getline(buff, 1000);
		cout << "send=" << cl->send({ buff, strlen(buff) + 1 }) << endl;

		Sleep(1000);
		cl->reload();
	}


	//cout << "closeEV=" << cl->close() << endl;

	system("pause");
}