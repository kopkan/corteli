/*
#include <iostream>
using namespace std;

#include <winsock2.h>
#include <windows.h>
#include <ctime>


#pragma comment(lib, "ws2_32.lib" )//socket
#pragma warning(disable : 4996)

#include<thread> 
*/

#pragma once

#include "corteli.h"

#define U_LONG unsigned long

namespace Network{

	bool Init(int vesion = 0x202, char *buff = NULL);
	U_LONG getInetAddr(char* host);


	namespace INFO{

		enum af { UNSPEC = 0, INET = 2, IPX = 6, APPLETALK = 16, NETBIOS = 17, INET6 = 23, IRDA = 26, BTH = 32 };
		enum type{ STREAM = 1, DGRAM = 2, RAW = 3, RDM = 4, SEQPACKET = 5 };
		enum protocol{ ICMP = 1, IGMP = 2, RFCOMM = 3, TCP = 6, UDP = 17, ICMPV6 = 58, RM = 113 };
		enum rules{ NO_DES_DEL = 1, NO_REL_DEL = 2, ALL = 255 };
	}

	namespace BSF{

		bool bind(SOCKET socket, int port, U_LONG localAddr = 0, int af = AF_INET);
		bool listen(SOCKET socket, int queueSize = 10);
		bool connect(SOCKET socket, U_LONG addr, int port);
		SOCKET accept(SOCKET socket);
		int recv(SOCKET socket, char* buff, int len, int flags = 0);
		int send(SOCKET socket, char* buff, int len, int flags = 0);
		int recvFrom(SOCKET socket, char* buff, int len, sockaddr_in* client_addr, int flags = 0);
		sockaddr_in* getName(SOCKET socket, sockaddr_in* sin = NULL);
		sockaddr_in* getPeerName(SOCKET socket, sockaddr_in* sin = NULL);
		WSAPROTOCOL_INFO* getInfo(SOCKET socket);
	};


	class BSI{

	public:

		sockaddr_in* getName(sockaddr_in* sin = NULL);
		sockaddr_in* getPeerName(sockaddr_in* sin = NULL);
		WSAPROTOCOL_INFO* getInfo();
		SOCKET getSocket();
		BSI();
		~BSI();

	protected:
		::SOCKET _socket;

		void _setSocket(SOCKET socket, bool destroyOld = false);
		bool _reload(int type, int protocol, int port = 0, U_LONG localAddr = 0, int af = AF_INET);
		bool _create(int type, int protocol, int port = 0, U_LONG localAddr = 0, int af = AF_INET);
		virtual void _destroy();

	};




	class Socket : public BSI{

	public:
		Socket(int af, int type, int protocol, int port = 0, U_LONG localAddr = 0);
		Socket(SOCKET socket);
		Socket();

		bool bind(int port, U_LONG localAddr = 0, int af = AF_INET);
		bool listen(int queueSize = 10);
		bool connect(U_LONG addr, int port);
		Socket* accept();
		int recv(char* buff, int len, int flags = 0);
		int send(char* buff, int len, int flags = 0);
		int recvFrom(char* buff, int len, sockaddr_in* client_addr, int flags = 0);
		int sendTo(char* buff, int len, sockaddr_in* client_addr, int flags = 0);
	};

	namespace TCP{

		class Socket : public Network::BSI{

		public:
			Socket(int port = 0, U_LONG localAddr = 0);

			bool bind(int port, U_LONG localAddr = 0);
			bool listen(int queueSize = 10);
			bool connect(U_LONG addr, int port);
			Socket* accept();
			int recv(char* buff, int len, int flags = 0);
			int send(char* buff, int len, int flags = 0);

		private:
			bool _create(int port = 0, U_LONG localAddr = 0);

		};


		class Client{

		public:
			Client(int port = 0, U_LONG localAddr = 0);
			Client(Socket* s);

			bool connectCycle(U_LONG ip, int port, int limNoCon = -1, int timeLimit = -1, int sleepBetweenTry = 100);
			void recvCycle();
			std::thread* recvCycleT();
			int send(char* buff, int len, int flags = 0);

		private:
			Socket clientSocket;
		};

		class Server{

		public:
			Server(int port, U_LONG localAddr = 0);
			void acceptCycle();
			std::thread* acceptCycleT();
			int send(int clId, char* buff, int len, int flags = 0);

			List<Client*> clientList;

		private:
			Socket serverSocket;
			
			void clientFunc(Client* cl);
		};
	};


	namespace UDP{

		class Socket : public Network::BSI{

		public:
			Socket(int port = 0, U_LONG localAddr = 0);
			bool bind(int port, U_LONG localAddr = 0);
			int recvFrom(char* buff, int len, sockaddr_in* client_addr, int flags = 0);
			int sendTo(char* buff, int len, sockaddr_in* client_addr, int flags = 0);

		protected:
			bool _create(int port = 0, U_LONG localAddr = 0);
		};
	};
};
