#include "Header.h"

using namespace std;

/*
class ss
{
public:
	ss(boost::asio::io_service& _ioService) 
		: _socket(_ioService)
	{
	
	}

	boost::asio::ip::tcp::socket _socket;
};
*/


boost::asio::io_service _ioService;

void ff()
{
	cout << "-----ff" << endl;

	while(1)
	{ 
		_ioService.run();

		_ioService.reset();
		Sleep(10);
	}

}

void main()
{
	SetConsoleCP(1251);// ��������� ������� �������� win-cp 1251 � ����� �����
	SetConsoleOutputCP(1251); // ��������� ������� �������� win-cp 1251 � ����� ������

	/*
	ss* S = new ss(_ioService);


	boost::system::error_code err;
	S->_socket.open(boost::asio::ip::tcp::v4(), err);
	if (err.value())
	{
		cout<<err.value()<<endl;
		cout << err.message() << endl;
	}

	boost::asio::socket_base::reuse_address reuse_address_option(0);
	//S->_socket.set_option(reuse_address_option);


	S->_socket.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 12792), err);
	cout << err.value() << endl;

	if (err.value())
	{
		cout << err.value() << endl;
		cout << err.message() << endl;
	}

	S->_socket.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 1500), err);
	if (err.value())
	{
		cout << err.value() << endl;
		cout << err.message() << endl;
	}

	Sleep(2000);


	send(S->_socket.native_handle(), "228", 4, 0);

	closesocket(S->_socket.native_handle());
	//delete S;
	*/

	thread(ff).detach();
	//Sleep(100);
	
	while (1)
	{
		Socket s(_ioService, true);
		cout << "bindq=" << s.bind(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 13192)) << endl;
		cout << "con=" << s.connect(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 1500)) << endl;
		cout << "wait=" << s.waitConnect(1000) << endl;
		cout << "recvStart=" << s.startRecv(1000) << endl;

		cout << "send=" << s.send("hello", 6) << endl;
		Sleep(1000);

		
		cout << "close=" << s.close() << endl;

		cout << "-----------------"  << endl;
		//Sleep(100);
	}

	system("pause");
}

/*
// ������ �������� TCP �������
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>


#define PORT 1500
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
	char buff[1024];
	printf("TCP DEMO CLIENT\n");

	// ��� 1 - ������������� ���������� Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// ��� 2 - �������� ������
	SOCKET* my_sock=new SOCKET;
	*my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	//////////////////////////////
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1055);/////////////////
	// �� �������� � ������� �������!!!
	local_addr.sin_addr.s_addr = 0;
	// ������ ��������� �����������
	// �� ��� IP-������

	// �������� bind ��� ����������
	if (::bind(*my_sock, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		// ������
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(*my_sock);  // ��������� �����!
		WSACleanup();
		//return -1;
	}

	// ��� 3 - ��������� ����������

	// ���������� ��������� sockaddr_in
	// �������� ������ � ����� �������
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// �������������� IP ������ �� ����������� �
	// ������� ������
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// ������� �������� IP ����� �� ���������
		// ����� �������
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list �������� �� ������ �������,
			// � ������ ���������� �� ������
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(*my_sock);
			WSACleanup();
			//return -1;
		}

	// ����� ������� ������� � �������� ����������
	// ���������� 
	if (connect(*my_sock, (sockaddr *)&dest_addr,
		sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		//return -1;
	}

	printf("���������� � %s ������� �����������\n\
    Type quit for quit\n\n", SERVERADDR);

	printf("Recv error %d\n", WSAGetLastError());

	Sleep(1000);
	closesocket(*my_sock);
	//WSACleanup();

	//shutdown(*my_sock, 2);
	//Sleep(1000);
	//closesocket(*my_sock);

	//CloseHandle((HANDLE)*my_sock);

	system("pause");
	return -1;
}
*/