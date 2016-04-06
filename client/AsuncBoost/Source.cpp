#include "Client.h"
#include "Client2.h"

#include "BaseClient.h"

#include "BaseAsuncClient.h"
#include <corteli\list\list.h>

#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

using namespace std;


boost::unordered_map<char*, int> h;



#pragma warning(disable :4996)
/*
boost::asio::ip::tcp::socket* cl;
char reply[1000];
int stop = 0;
void recvA(const boost::system::error_code & ec, std::size_t bytes_transferred) {
cout << "r=" << bytes_transferred << ' ' << reply << endl;
if (!stop) {
//cl->send(boost::asio::buffer("777"));
cl->async_receive(boost::asio::buffer(reply, 1000), 0, recvA);
}
else {
cout << "stop" << endl;
}
}
void s(boost::asio::io_service* io) {
io->run();
}
void ff() {
cl->async_receive(boost::asio::buffer(reply, 1000), 0, recvA);
}
void f() {
boost::asio::io_service io;
boost::asio::ip::tcp::endpoint serv_point(boost::asio::ip::address_v4::from_string("127.0.0.1"), 8800);
cl=new boost::asio::ip::tcp::socket(io);
cl->connect(serv_point);
cl->send(boost::asio::buffer("Hello serv123"));
cl->send(boost::asio::buffer("777"));

ff();
boost::thread q( s, &io );
Sleep(1000);
stop = 1;
//Sleep(100);
cl->close();
//io.stop();
system("pause");
}
*/



class BasePool
{

public:
	BasePool(boost::asio::io_service* io2)
	{
		io = io2;
	}

	int add(corteli::BaseAsuncClient* cl)
	{
		l.add(cl);

		cl->start();

		return 0;
	}

	corteli::BaseAsuncClient* get(int n)
	{
		return l.get(n);
	}

	void ioRun() 
	{
		while (1)
		{
			io->run();
			//cout << "run end" << endl;

			Sleep(10);
		}
	}

	void workRun()
	{
		while (1)
		{
			for (int i = 0; i < l.getSize(); i++)
			{
				l.get(i)->send("frash");
			}
			Sleep(1000);
		}
	}


	void start()
	{
		boost::thread(&BasePool::ioRun, this);
		boost::thread(&BasePool::workRun, this);
	}

	boost::asio::io_service* io;
	List<corteli::BaseAsuncClient*> l;

};



void f() {

	boost::asio::io_service io;
	corteli::BaseAsuncClient C;


	C.setIO(&io);
	C.setAttr({ boost::asio::ip::address_v4::from_string("127.0.0.1"), 8800 }, {}, 10000);



	BasePool pool(&io);
	cout << pool.add(&C);

	pool.start();

	/*
	while (1) {

		pool.get(0)->start();

		//cout << pool.get(0)->getError() << endl;
		Sleep(10000);
		cout << "er1=" << pool.get(0)->getError() << endl;

		cout <<"stop="<< pool.get(0)->stop() << endl;
		cout <<"er2="<< pool.get(0)->getError() << endl;
		break;
	}
	*/


	system("pause");
}

int main(int argc, char* argv[])
{


	boost::asio::ip::address_v4::from_string("93.127.78.100");

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода


	f();
	system("pause");





	corteli::BaseClient bs;


	bs.conInfo({ boost::asio::ip::address_v4::from_string("127.0.0.1"), 8800 }, 10000, {});

	int i = 0;

	cout << "size=" << sizeof(corteli::BaseClient) << endl;

	while (i<5) {

		bs.startT();
		bs.waitingConnect();


		cout << "s1=" << bs.send("123") << endl;
		bs.close();

		//Sleep(100);

		cout << "s2=" << bs.send("4789") << endl;


		cout << "-----------------" << endl;

		i++;
		//break;

		Sleep(1000);
	}

	cout << "---------end------" << endl;
	Sleep(10000);

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
