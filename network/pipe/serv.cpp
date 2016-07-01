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


class Client : public BaseClient
{
public:
	Client(IoService* ioService, bool enableDebugMessage = true) : BaseClient(ioService, enableDebugMessage)
	{};

	void recvError(const boost::system::error_code & ec)
	{
		cout<<"ers="<<send("errrr", 6, 1)<<endl;//fix bind problem
		BaseClient::recvError(ec);
	}

	virtual void recvMessage(char*buff, std::size_t size)
	{
		cout << "send=" << send(buff, size) << endl;
	}
};



















#include "D:/Develop/VisualStudio15Project/corteli/network/pipe/pipe/BaseAcceptor.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/pipe/Release/pipe.lib")



class Client2 : public corteli::network::pipe::BaseClient
{
public:
	Client2(corteli::network::pipe::IoService* ioService, bool enableDebugMessage = true) : BaseClient(ioService, enableDebugMessage)
	{};

	void recvError(const boost::system::error_code & ec)
	{
		cout << "ers=" << send("errrr", 6) << endl;//fix bind problem
		BaseClient::recvError(ec);
	}

	virtual void recvMessage(char*buff, std::size_t size)
	{
		cout << "send=" << send(buff, size) << endl;
	}
};




int main(int argc, char* argv[])
{

	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
							  //udp_test();

	/*
	BaseAcceptor<Client> a(true);
	a.bind({ boost::asio::ip::tcp::v4(), 1500 });

	a.startAccept();
	//a.stop();
	*/



	corteli::network::pipe::BaseAcceptor<Client2> a(true);

	//a.bind({ boost::asio::ip::tcp::v4(), 1500 });

	a.startAcceptT("\\\\.\\pipe\\mypipe");


	system("pause");
	return 0;
}



/*
//
// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

class session
{
public:
	session(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));
	}

private:
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(data_, bytes_transferred),
				boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error));
		}
		else
		{
			cout << 8888;
			delete this;
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
				boost::bind(&session::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			delete this;
		}
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}

		start_accept();
	}

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
};

int main(int argc, char* argv[])
{
	try
	{
		boost::asio::io_service io_service;

		using namespace std; // For atoi.
		server s(io_service, 1500);

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}*/





/*
// Пример простого TCP – эхо сервера

#include <stdio.h>
#include <winsock2.h>  // Wincosk2.h должен быть 
// подключен раньше windows.h!
#include <windows.h>

#define MY_PORT    1500
// Порт, который слушает сервер

// макрос для печати количества активных
// пользователей 
#define PRINTNUSERS if (nclients)\
  printf("%d user on-line\n",nclients);\
  else printf("No User on line\n");

// прототип функции, обслуживающий
// подключившихся пользователей
DWORD WINAPI SexToClient(LPVOID client_socket);

// глобальная переменная – количество
// активных пользователей 
int nclients = 0;

int main(int argc, char* argv[])
{
	char buff[1024];    // Буфер для различных нужд

	printf("TCP SERVER DEMO\n");

	// Шаг 1 - Инициализация Библиотеки Сокетов
	// Т.к. возвращенная функцией информация
	// не используется ей передается указатель на
	// рабочий буфер, преобразуемый
	// к указателю  на структуру WSADATA.
	// Такой прием позволяет сэкономить одну
	// переменную, однако, буфер должен быть не менее
	// полкилобайта размером (структура WSADATA
	// занимает 400 байт)
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		// Ошибка!
		printf("Error WSAStartup %d\n",
			WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET mysocket;
	// AF_INET     - сокет Интернета
	// SOCK_STREAM  - потоковый сокет (с
	//      установкой соединения)
	// 0      - по умолчанию выбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		// Деиницилизация библиотеки Winsock
		return -1;
	}

	// Шаг 3 связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	// не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0;
	// сервер принимает подключения
	// на все IP-адреса

	// вызываем bind для связывания
	if (::bind(mysocket, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		// Ошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);  // закрываем сокет!
		WSACleanup();
		return -1;
	}

	// Шаг 4 ожидание подключений
	// размер очереди – 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Ожидание подключений\n");

	// Шаг 5 извлекаем сообщение из очереди
	SOCKET client_socket;    // сокет для клиента
	sockaddr_in client_addr;    // адрес клиента
								// (заполняется системой)

								// функции accept необходимо передать размер
								// структуры
	int client_addr_size = sizeof(client_addr);

	// цикл извлечения запросов на подключение из
	// очереди
	while ((client_socket = accept(mysocket, (sockaddr *)
		&client_addr, &client_addr_size)))
	{
		nclients++;      // увеличиваем счетчик
						 // подключившихся клиентов

						 // пытаемся получить имя хоста
		HOSTENT *hst;
		hst = ::gethostbyaddr((char *)
			&client_addr.sin_addr.s_addr, 4, AF_INET);


		cout << "new" << ntohs(client_addr.sin_port) << endl;
		// вывод сведений о клиенте
		//PRINTNUSERS

			// Вызов нового потока для обслужвания клиента
			// Да, для этого рекомендуется использовать
			// _beginthreadex но, поскольку никаких вызов
			// функций стандартной Си библиотеки поток не
			// делает, можно обойтись и CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, SexToClient,
			&client_socket, NULL, &thID);
	}
	return 0;
}

// Эта функция создается в отдельном потоке и
// обсуживает очередного подключившегося клиента
// независимо от остальных
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];

	
	char buff[20 * 1024];
	
    #define sHELLO "Hello, Sailor\r\n"

	// отправляем клиенту приветствие 
	//send(my_sock, sHELLO, sizeof(sHELLO), 0);

	// цикл эхо-сервера: прием строки от клиента и
	// возвращение ее клиенту

	int R = recv(my_sock, &buff[0], sizeof(buff), 0);
	while (R > 0)
	{
		R = recv(my_sock, &buff[0], sizeof(buff), 0);
		send(my_sock, &buff[0], 10, 0);
	}
	// если мы здесь, то произошел выход из цикла по
	// причине возращения функцией recv ошибки –
	// соединение клиентом разорвано
	nclients--; // уменьшаем счетчик активных клиентов
	
	cout << "err=" << R << " "<< GetLastError() << endl;
	
	
		// закрываем сокет
		closesocket(my_sock);
	return 0;
}
*/
