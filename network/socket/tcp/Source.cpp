#include "BaseAcceptor.h"

using namespace std;

using namespace corteli::network::socket::tcp;
using namespace corteli::network;


void main()
{
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода

	BasePool<BaseClient> bp;
	bp.runExpansionT();

	while (1)
	{
	
		BaseClient* s = bp.newClient();
		cout << "bindq=" << s->bind({ "127.0.0.1", 13195 }) << endl;
		cout << "con=" << s->connect({ "127.0.0.1", 1500 }) << endl;
		cout << "wait=" << s->waitConnect() << endl;
		cout << "recvStart=" << s->startRecv(1000) << endl;

		cout << "send=" << s->send({ "hello", 6 }) << endl;

		cout << "-----------------"  << endl;

		while (1)
		{
			char buff[1000];
			cin.getline(buff, 1000);
			cout << "send=" << s->send({ buff, strlen(buff) + 1 }) << endl;
		}

		break;
	}


	system("pause");
	
}