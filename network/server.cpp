#include <boost/asio.hpp>
#include <boost/thread.hpp>
using boost::asio::ip::udp;
using boost::asio::ip::tcp;

using namespace boost::asio;
using namespace std;

#pragma warning(disable:4996)

void udp_test() {

	try
	{
		io_service io_service;
		udp::socket server(io_service, udp::endpoint(ip::address::from_string("127.0.0.1"), 3002));
		udp::endpoint sender_endpoint;


		while (1) {
			char reply[1000];
			size_t reply_length = server.receive_from(buffer(reply, 500), sender_endpoint);
			server.send_to(buffer(reply, 500), sender_endpoint);

			cout << reply << endl;

			Sleep(1000);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

void client(tcp::socket *sock) {


	cout << "rl" << endl;
	boost::system::error_code err;

	try
	{
		while (1) 
		{
			char reply[1000];

			size_t reply_length = sock->receive(buffer(reply, 1000), 0, err);

			if (err.value())
			{
				cout << err.value() << err.message() << endl;

	//			Sleep(100);
				sock->send(buffer("7777", 5), 0, err);
				cout << err.value() << err.message() << endl;
				
				sock->shutdown(sock->shutdown_both, err);
				sock->close(err);
				cout << err.value() << err.message() << endl;

				break;
			}
			else
			{
				cout << "rec=" << reply << endl;
				sock->send(buffer(reply, strlen(reply) + 1));
				cout << "send=" << reply << endl;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception228: "  << e.what() << "\n";

		//sock->send(buffer("7788", 5), 0, err);
		if (err.value())
		{
			cout << err.value() << endl;
			cout << err.message() << endl;
		}


		//sock->close();
		//delete sock;

		cout << "8910";
	}
}

void tcp_test() {

		io_service io_service;

		tcp::acceptor server(io_service);
		
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), 1500);
		server.open(endpoint.protocol());
		server.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		server.bind(endpoint);
		server.listen();

		while (1)
		{
			cout << "newwww79" << endl;

			tcp::socket *sock = new tcp::socket(io_service);
			server.accept(*sock);
			cout << sock->remote_endpoint() << endl;
			cout << sock->local_endpoint() << endl;
			//Sleep(10000);

			boost::thread Thread(client, sock);
			Thread.detach();
		}


}




#include "D:/Develop/VisualStudio15Project/corteli/network/socket/tcp/tcp/BaseAcceptor.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/socket/tcp/Release/tcp.lib")


class Client : public corteli::network::socket::tcp::BaseClient
{
public:
	Client(corteli::network::IoService* ioService, bool enableDebugMessage = true) : BaseClient(ioService, enableDebugMessage)
	{};

	void recvError(const boost::system::error_code & ec)
	{
		cout << "ers=" << send({ "errrr", 6 }, 1) << endl;//fix bind problem
		BaseClient::recvError(ec);
	}

	virtual void recvMessage(corteli::base::container::Buffer<char> buffer)
	{
		cout << "send=" << send(buffer) << endl;
	}
};

/*
int main(int argc, char* argv[])
{

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
							  //udp_test();

	corteli::network::socket::tcp::BaseAcceptor<Client> a(true);
	a.bind({ boost::asio::ip::tcp::v4(), 1500 });

	a.startAccept();
	//a.stop();


	system("pause");
	return 0;
}

*/















#include "D:/Develop/VisualStudio15Project/corteli/network/pipe/pipe/BaseAcceptor.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/pipe/Release/pipe.lib")



class Client2 : public corteli::network::pipe::BaseClient
{
public:
	Client2(corteli::network::IoService* ioService, bool enableDebugMessage = true) : BaseClient(ioService, enableDebugMessage)
	{};

	void recvError(const boost::system::error_code & ec)
	{
		cout << "ers=" << send({ "errrr", 6 }) << endl;//fix bind problem
		BaseClient::recvError(ec);
	}

	virtual void recvMessage(corteli::base::container::Buffer<char> buffer)
	{
		cout << "send=" << send(buffer) << endl;
	}
};




int main(int argc, char* argv[])
{

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток выв



	corteli::network::pipe::BaseAcceptor<Client2> a(true);

	a.startAcceptT("\\\\.\\pipe\\mypipe");


	system("pause");
	return 0;
}




#include "D:/Develop/VisualStudio15Project/corteli/network/socket/udp/udp/BaseSocket.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/socket/udp/Release/udp.lib")


/*
int main(int argc, char* argv[])
{

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
							  //udp_test();


	corteli::network::IoService* io = new corteli::network::IoService(true);


	corteli::network::socket::udp::BaseSocket s(io, true);



	int i = 0;
	while (i < 10)
	{
		cout << "bind=" << s.bind({ "127.0.0.1", (unsigned short)(1234 + i) }) << endl;
		cout << "startRecv=" << s.startRecv(5000000) << endl;


		std::ostringstream oss;
		oss << "MY MESSAGE=" << i;

		std::string s2 = oss.str();



		cout << "send=" << s.sendTo({ (char*)s2.c_str(), strlen((char*)s2.c_str()) + 1 }, { "127.0.0.1", 12345 }) << endl;
		Sleep(1000);

		s.reload();
		i++;
	}

	system("pause");

	cout << "ENDDDDD!!!" << endl;
	return 0;
}
*/
