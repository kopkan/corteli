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




#include "D:/Develop/VisualStudio15Project/corteli/network/socket/tcp/tcp/BasePool.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/socket/tcp/Release/tcp.lib")


template<typename T>
class serv : protected BasePool<T>
{
public:
	serv(boost::asio::ip::tcp::endpoint ep) : _server(_ioService.getIoSevice(), ep)
	{
		runExpansionT();
	}

	void bind(boost::asio::ip::tcp::endpoint ep)
	{

	}

	T* nextAccept()
	{
		T* socket = newClient();

		_server.accept(socket->getSocket());
		socket->_acceptorThisSocketConnected();
		socket->startRecv(500);

		return socket;

	}

private:
	boost::asio::ip::tcp::acceptor _server;
};


int main(int argc, char* argv[])
{

	SetConsoleCP(1251);// ��������� ������� �������� win-cp 1251 � ����� �����
	SetConsoleOutputCP(1251); // ��������� ������� �������� win-cp 1251 � ����� ������
							  //udp_test();
	//tcp_test();

	IoService io;
	io.startT();
	serv<BaseClient> a({ boost::asio::ip::tcp::v4(), 1500 });

	while (1)
	{
		BaseClient* s = a.nextAccept();
		s->send("Hello", 6);
		Sleep(100);
		s->close();
	}

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
// ������ �������� TCP � ��� �������

#include <stdio.h>
#include <winsock2.h>  // Wincosk2.h ������ ���� 
// ��������� ������ windows.h!
#include <windows.h>

#define MY_PORT    1500
// ����, ������� ������� ������

// ������ ��� ������ ���������� ��������
// ������������� 
#define PRINTNUSERS if (nclients)\
  printf("%d user on-line\n",nclients);\
  else printf("No User on line\n");

// �������� �������, �������������
// �������������� �������������
DWORD WINAPI SexToClient(LPVOID client_socket);

// ���������� ���������� � ����������
// �������� ������������� 
int nclients = 0;

int main(int argc, char* argv[])
{
	char buff[1024];    // ����� ��� ��������� ����

	printf("TCP SERVER DEMO\n");

	// ��� 1 - ������������� ���������� �������
	// �.�. ������������ �������� ����������
	// �� ������������ �� ���������� ��������� ��
	// ������� �����, �������������
	// � ���������  �� ��������� WSADATA.
	// ����� ����� ��������� ���������� ����
	// ����������, ������, ����� ������ ���� �� �����
	// ������������ �������� (��������� WSADATA
	// �������� 400 ����)
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		// ������!
		printf("Error WSAStartup %d\n",
			WSAGetLastError());
		return -1;
	}

	// ��� 2 - �������� ������
	SOCKET mysocket;
	// AF_INET     - ����� ���������
	// SOCK_STREAM  - ��������� ����� (�
	//      ���������� ����������)
	// 0      - �� ��������� ���������� TCP ��������
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		// ������!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		// �������������� ���������� Winsock
		return -1;
	}

	// ��� 3 ���������� ������ � ��������� �������
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	// �� �������� � ������� �������!!!
	local_addr.sin_addr.s_addr = 0;
	// ������ ��������� �����������
	// �� ��� IP-������

	// �������� bind ��� ����������
	if (::bind(mysocket, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		// ������
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);  // ��������� �����!
		WSACleanup();
		return -1;
	}

	// ��� 4 �������� �����������
	// ������ ������� � 0x100
	if (listen(mysocket, 0x100))
	{
		// ������
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("�������� �����������\n");

	// ��� 5 ��������� ��������� �� �������
	SOCKET client_socket;    // ����� ��� �������
	sockaddr_in client_addr;    // ����� �������
								// (����������� ��������)

								// ������� accept ���������� �������� ������
								// ���������
	int client_addr_size = sizeof(client_addr);

	// ���� ���������� �������� �� ����������� ��
	// �������
	while ((client_socket = accept(mysocket, (sockaddr *)
		&client_addr, &client_addr_size)))
	{
		nclients++;      // ����������� �������
						 // �������������� ��������

						 // �������� �������� ��� �����
		HOSTENT *hst;
		hst = ::gethostbyaddr((char *)
			&client_addr.sin_addr.s_addr, 4, AF_INET);


		cout << "new" << ntohs(client_addr.sin_port) << endl;
		// ����� �������� � �������
		//PRINTNUSERS

			// ����� ������ ������ ��� ����������� �������
			// ��, ��� ����� ������������� ������������
			// _beginthreadex ��, ��������� ������� �����
			// ������� ����������� �� ���������� ����� ��
			// ������, ����� �������� � CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, SexToClient,
			&client_socket, NULL, &thID);
	}
	return 0;
}

// ��� ������� ��������� � ��������� ������ �
// ���������� ���������� ��������������� �������
// ���������� �� ���������
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];

	
	char buff[20 * 1024];
	
    #define sHELLO "Hello, Sailor\r\n"

	// ���������� ������� ����������� 
	//send(my_sock, sHELLO, sizeof(sHELLO), 0);

	// ���� ���-�������: ����� ������ �� ������� �
	// ����������� �� �������

	int R = recv(my_sock, &buff[0], sizeof(buff), 0);
	while (R > 0)
	{
		R = recv(my_sock, &buff[0], sizeof(buff), 0);
		send(my_sock, &buff[0], 10, 0);
	}
	// ���� �� �����, �� ��������� ����� �� ����� ��
	// ������� ���������� �������� recv ������ �
	// ���������� �������� ���������
	nclients--; // ��������� ������� �������� ��������
	
	cout << "err=" << R << " "<< GetLastError() << endl;
	
	
		// ��������� �����
		closesocket(my_sock);
	return 0;
}
*/