#include "BaseClient.h"

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace std;


boost::unordered_map<char*, int> h;


void sender() {

	try {
		while (1) {

			cout << '8' << endl;
			//boost::this_thread::interruption_point();



			//boost::this_thread::interruptible_wait(1);
			boost::this_thread::sleep(boost::posix_time::seconds(1));

			//boost::this_thread::disable_interruption();

			//Sleep(1000);
		}

	}
	catch (boost::thread_interrupted &e) {
	

		cout << "catch" << endl;

	}

}

void mainThread()
{

	boost::thread* thrd;
	thrd=new boost::thread(sender);

	//thrd->detach();

	cout << thrd->joinable() << endl;
	//Sleep(1000);
	thrd->interrupt();
	thrd->join();
	//Sleep(300);
	cout << thrd->joinable() << endl;
	delete thrd;
	//thrd.detach();

	//cout << boost::thread::hardware_concurrency() << endl;

	//thrd = boost::thread(sender); //start thread
		//thrd.join();
	//Sleep(300);


		
		//boost::thread_interrupted();
		//thrd.interruption_requested();

	//thrd.detach();

	//Sleep(300);
		
	//thrd.interrupt();
	
	//TerminateThread(thrd.native_handle(), 0);
		
}


#pragma warning(disable :4996)

int main(int argc, char* argv[])
{
	/*
	cout << h.size() << endl;
	

	char* key = new char[3];
	strcpy(key, "12");


	h.insert({key, 28});

	//strcpy(key, "78");
	h.insert({ key, 29 });

	h.reserve(1000);

	cout << h.size() << endl;


	try {

		cout <<"size="<< h.count(key)<<endl;
		cout << "size1=" << h.bucket_count() << endl;
		cout << "v=" << h.at(key) << endl;

		//h.at(q);
	}
	catch (...) {
	
		cout << "err" << endl;
	
	}



	//cout << h.at(q) << endl;





	*/

	//mainThread();

	//cout << "----789----" << endl;


	/*
	boost::asio::ip::tcp::endpoint servPoint;

	cout << servPoint.address() << endl;
	cout << servPoint.port() << endl;
	*/

	corteli::BaseClient bs;


	bs.conInfo({ boost::asio::ip::address_v4::from_string("127.0.0.1"), 8800 }, 10000, {});
	while (1) {
		
		//cout << bs._status << endl;



		bs.startT();
		Sleep(100);

		cout << bs.send("123") << endl;

		bs.close();

		//Sleep(100);

		cout << bs.send("4789") << endl;

		//break;
	}

	cout << "---------end------" << endl;

	Sleep(100000);
	

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода




	corteli::Client2 c;

	try
	{
		cout << c.bind(8801, "127.0.0.1") << endl;

		c.workT("127.0.0.1", 8800, 1000, 3000);


		Sleep(300);

		c.send("777777");
	
		Sleep(300);
		c.close();

		cout << 10 << endl;

		Sleep(300);
		c.close();

		cout << 20 << endl;

		Sleep(300);
		c.close();
		Sleep(300);

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


/*
void udp_test() {

try
{
boost::asio::io_service io_service;

udp::socket s(io_service, udp::endpoint(udp::v4(), 0));

udp::resolver resolver(io_service);
udp::endpoint endpoint = *resolver.resolve({ udp::v4(), "127.0.0.1", "3002" });

while (1) {

s.send_to(buffer("123", 4), endpoint);

char reply[1000];

udp::endpoint sender_endpoint;
size_t reply_length = s.receive_from(boost::asio::buffer(reply, 500), sender_endpoint);

cout << reply << endl;

Sleep(1000);

}
}
catch (std::exception& e)
{
std::cerr << "Exception: " << e.what() << "\n";
}
}


void tcp_test() {

try
{
boost::asio::io_service io_service;

tcp::socket client(io_service);
tcp::endpoint serv_point(ip::address_v4::from_string("127.0.0.1"), 8800);

client.connect(serv_point);

std::cout << client.remote_endpoint() << std::endl;
std::cout << client.local_endpoint() << std::endl;

while (1) {

client.send(buffer("1234", 4));

char reply[1000];
size_t reply_length = client.receive(buffer(reply, 500));
cout << reply << endl;

Sleep(1000);

}
}
catch (std::exception& e)
{
std::cerr << "Exception: " << e.what() << "\n";
}

}



void tcp_client_recv(const boost::system::error_code & ec, std::size_t bytes_transferred) {

//cout << "123456";
//throw 123;
}

void tcp_client() {


try
{
boost::asio::io_service io_service;

tcp::socket client(io_service);
tcp::endpoint serv_point(ip::address_v4::from_string("127.0.0.1"), 8800);

client.connect(serv_point);

std::cout << client.remote_endpoint() << std::endl;
std::cout << client.local_endpoint() << std::endl;

client.send(buffer("Hello serv123"));

cout << "cout" << endl;


char reply[1000];

while (1) {

client.receive(buffer(reply, 1000));

cout << reply << endl;
Sleep(1000);
client.send(buffer("777"));
}


}
catch (std::exception& e)
{
std::cerr << "Exception: " << e.what() << "\n";
}
}
*/
