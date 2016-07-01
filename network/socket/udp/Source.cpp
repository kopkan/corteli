#include "Socket.h"



using namespace corteli::network::socket::udp;
using namespace std;


void main()
{
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода



	IoService io(true);


	Socket s(&io, true);

	cout << "bind=" << s.bind(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 0)) << endl;

	cout << "ep=" << s.getSocket().local_endpoint() << endl;


	cout << "startRecv=" << s.startRecv(500) << endl;
	cout << "send=" << s.sendTo("12345", 6, boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 1234)) << endl;
	cout << "send=" << s.sendTo("12345", 6, boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 1234)) << endl;

	// при delete может быть ошибка из-за sendHandle не отработав

	system("pause");

}