#include "Client.h"

using namespace std;

void y() {


	while (1) {
	
		cout << 123 << endl;
		Sleep(1000);
	
	}

}


void someFunc(boost::thread_group & thg, boost::thread * thisTh)
{
	// do sth

	thg.remove_thread(thisTh);
	delete thisTh; // we coud do this as thread of execution and boost::thread object are quite independent
}

void run()
{
	boost::thread_group my_threads;
	boost::thread *t = new boost::thread(y); // invalid handle, but we need some memory placeholder, so we could pass it to someFunc
	//t->detach();
	
	//*t = boost::thread(
	boost::bind(&someFunc, boost::ref(my_threads), t);
	//	);
	//my_threads.add_thread(t);
	
	// do not call join
}


int main(int argc, char* argv[])
{
	run();

	cout << 777 << endl;
	Sleep(100000);




	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода

	corteli::Client c;
	//c.bind(8801, "127.0.0.1");
	try
	{

		c.start("127.0.0.1", 8800, 100, 1000);

		c.send("777777");

		c.close();

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
