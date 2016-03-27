#include "Client.h"


int main(int argc, char* argv[])
{
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода

	corteli::Client c;
	try
	{
		//c.bind("127.0.0.1", 80900);
		c.connectCycle("127.0.0.1", 8800);
		

		char buff[100];
		//c.recvCycleT(buff, 100);
		c.start(buff, 100, 1000);

		while (1) {
			c.send("12345");
			Sleep(1000);

		}


	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	system("pause");
	return 0;
}
