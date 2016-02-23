#include "corteli.h"

namespace Corteli{

	namespace Network{

		bool Init(int vesion, char* buff){

			bool deleteInfoBuff = false;

			if (buff == NULL)
			{
				buff = new char[sizeof(WSADATA)];
				deleteInfoBuff = true;
			}

			if (WSAStartup(vesion, (WSADATA *)&buff[0]))
			{
				cout << "WSAStart error " << WSAGetLastError() << endl;
				return false;
			}

			if (deleteInfoBuff)
			{
				delete buff;
			}

			return true;
		}

		U_LONG getInetAddr(char* host){

			if (inet_addr(host) != INADDR_NONE){

				return inet_addr(host);
			}
			else{

				HOSTENT *hst;
				if (hst = gethostbyname(host)){

					return ((U_LONG **)hst->h_addr_list)[0][0];
				}
				else{

					cout << "Error: no addr" << endl;
					return 0;
				}
			}
		}

		namespace BSF{

			bool bind(SOCKET socket, int port, U_LONG localAddr, int af){

				cout << "bind " << port << localAddr << endl;

				sockaddr_in local_addr;
				local_addr.sin_family = af;

				local_addr.sin_port = htons(port);

				if (localAddr != 0){
					local_addr.sin_addr.s_addr = localAddr;
				}
				else{
					local_addr.sin_addr.s_addr = INADDR_ANY;
				}

				if (::bind(socket, (sockaddr *)&local_addr, sizeof(local_addr)))
				{
					cout << "Error bind " << WSAGetLastError() << endl;
					return false;
				}
				return true;
			}

			bool listen(SOCKET socket, int queueSize)
			{
				if (::listen(socket, queueSize)){

					cout << "Error listen " << WSAGetLastError() << endl;
					return false;
				}
				return true;
			}

			bool connect(SOCKET socket, U_LONG addr, int port){

				sockaddr_in dest_addr;
				dest_addr.sin_family = AF_INET;
				dest_addr.sin_port = htons(port);
				dest_addr.sin_addr.s_addr = addr;

				if (::connect(socket, (sockaddr *)&dest_addr, sizeof(dest_addr)))
				{
					cout << "Connect error" << WSAGetLastError() << endl;
					return false;
				}
				return true;

			}

			SOCKET accept(SOCKET socket){

				sockaddr_in client_addr;
				int client_addr_size = sizeof(client_addr);

				return ::accept(socket, (sockaddr *)&client_addr, &client_addr_size);
			}

			int recv(SOCKET socket, char* buff, int len, int flags){

				return ::recv(socket, buff, len, flags);
			}

			int send(SOCKET socket, char* buff, int len, int flags){

				return ::send(socket, buff, len, flags);
			}

			int recvFrom(SOCKET socket, char* buff, int len, sockaddr_in* client_addr, int flags){

				if (buff != NULL && client_addr != NULL){
					int addr_size = sizeof(sockaddr_in);
					return ::recvfrom(socket, buff, len, flags, (sockaddr *)client_addr, &addr_size);
				}
				return -2;
			}

			int sendTo(SOCKET socket, char* buff, int len, sockaddr_in* client_addr, int flags){

				if (buff != NULL && client_addr != NULL){
					int addr_size = sizeof(sockaddr_in);
					return ::sendto(socket, buff, len, flags, (sockaddr *)client_addr, addr_size);
				}
				return -2;

			}

			sockaddr_in* getName(SOCKET socket, sockaddr_in* sin){

				sockaddr_in* _sin = sin;
				if (sin == NULL){
					_sin = new sockaddr_in;
				}
				int len = sizeof(sockaddr_in);

				if (getsockname(socket, (sockaddr *)_sin, &len) == -1){

					if (sin == NULL){
						delete _sin;
					}
					cout << "getName err, sock:" << socket << endl;

					return NULL;
				}

				cout << "getName: " << htons(_sin->sin_port) << " " << inet_ntoa(_sin->sin_addr) << endl;

				return _sin;
			}

			sockaddr_in* getPeerName(SOCKET socket, sockaddr_in* sin){

				sockaddr_in* _sin = sin;
				if (sin == NULL){
					_sin = new sockaddr_in;
				}
				int len = sizeof(sockaddr_in);

				if (getpeername(socket, (sockaddr *)_sin, &len) == -1){

					if (sin == NULL){
						delete _sin;
					}
					cout << "getPeerName err, sock:" << socket << endl;

					return NULL;
				}

				cout << "getPeerName: " << htons(_sin->sin_port) << " " << inet_ntoa(_sin->sin_addr) << endl;

				return _sin;
			}

			WSAPROTOCOL_INFO* getInfo(SOCKET socket){

				WSAPROTOCOL_INFO* socket_info = new WSAPROTOCOL_INFO;
				int size, result;
				size = sizeof(*socket_info);
				ZeroMemory(socket_info, size);
				result = getsockopt(socket, SOL_SOCKET, SO_PROTOCOL_INFO, (char *)socket_info, &size);
				if (result == SOCKET_ERROR)
				{
					delete socket_info;

					cout << "getInfo err, sock: " << socket << endl;

					return NULL;
				}

				cout << "getInfo: " << socket_info->szProtocol << " " << socket_info->iSocketType << ' ' << socket_info->iAddressFamily << endl;

				return socket_info;
			}

		};




		sockaddr_in* BSI::getName(sockaddr_in* sin){

			return BSF::getName(_socket, sin);
		}

		sockaddr_in* BSI::getPeerName(sockaddr_in* sin){

			return BSF::getPeerName(_socket, sin);
		}

		WSAPROTOCOL_INFO* BSI::getInfo(){

			return BSF::getInfo(_socket);
		}

		SOCKET BSI::getSocket(){

			return _socket;
		}

		BSI::BSI(){

			_socket = NULL;
		}

		BSI::~BSI(){

			_destroy();
		}


		::SOCKET _socket;

		void BSI::_setSocket(SOCKET socket, bool destroyOld){

			if (destroyOld){
				_destroy();
			}

			_socket = socket;
		}

		bool BSI::_reload(int type, int protocol, int port, U_LONG localAddr, int af){

			_destroy();

			return _create(type, protocol, port, localAddr, af);
		}

		bool BSI::_create(int type, int protocol, int port, U_LONG localAddr, int af){

			_socket = socket(af, type, protocol);

			if (_socket < 0)
			{
				cout << "Socket() error " << WSAGetLastError() << endl;
				return false;
			}

			if (port>0 || localAddr != 0)
			{
				if (!BSF::bind(_socket, port, localAddr, af)){
					return false;
				}
			}

			return true;
		}

		void BSI::_destroy(){

			if (_socket != NULL){

				closesocket(_socket);
				_socket = NULL;
			}
		}




		Socket::Socket(int af, int type, int protocol, int port, U_LONG localAddr){

			_create(af, type, protocol, port, localAddr);
		}

		Socket::Socket(SOCKET socket){

			_socket = socket;
		}

		Socket::Socket(){

			_socket = NULL;
		}


		bool Socket::bind(int port, U_LONG localAddr, int af){

			return BSF::bind(_socket, port, localAddr, af);
		}

		bool Socket::listen(int queueSize){

			return BSF::listen(_socket, queueSize);
		}

		bool Socket::connect(U_LONG addr, int port){

			return BSF::connect(_socket, addr, port);
		}

		Socket* Socket::accept(){

			Socket* client = new Socket;
			client->_socket = BSF::accept(_socket);

			return client;
		}

		int Socket::recv(char* buff, int len, int flags){

			return BSF::recv(_socket, buff, len, flags);
		}

		int Socket::send(char* buff, int len, int flags){

			return BSF::send(_socket, buff, len, flags);
		}

		int Socket::recvFrom(char* buff, int len, sockaddr_in* client_addr, int flags){

			return BSF::recvFrom(_socket, buff, len, client_addr, flags);
		}

		int Socket::sendTo(char* buff, int len, sockaddr_in* client_addr, int flags){

			return BSF::sendTo(_socket, buff, len, client_addr, flags);
		}


		namespace TCP{

			Socket::Socket(int port, U_LONG localAddr){

				_create(port, localAddr);
			}

			bool Socket::bind(int port, U_LONG localAddr){

				return Network::BSF::bind(_socket, port, localAddr);
			}

			bool Socket::listen(int queueSize){

				return Network::BSF::listen(_socket, queueSize);
			}

			bool Socket::connect(U_LONG addr, int port){

				return Network::BSF::connect(_socket, addr, port);
			}

			Socket* Socket::accept(){

				Socket* client = new Socket;

				client->_socket = Network::BSF::accept(_socket);

				return client;
			}

			int Socket::recv(char* buff, int len, int flags){

				return Network::BSF::recv(_socket, buff, len, flags);
			}

			int Socket::send(char* buff, int len, int flags){

				return Network::BSF::send(_socket, buff, len, flags);
			}

			bool Socket::_create(int port, U_LONG localAddr){

				return BSI::_create(INFO::type::STREAM, INFO::protocol::TCP, port, localAddr);
			}





			Client::Client(int port, U_LONG localAddr){

				clientSocket.bind(port, localAddr);
			}

			Client::Client(Socket *s){

				clientSocket = *s;
			}

			bool Client::connectCycle(U_LONG ip, int port, int limNoCon, int timeLimit, int sleepBetweenTry){

				int startTime = clock();
				int nowTime = startTime;

				for (int i = limNoCon; i > 0 || i == -1; --i){

					if (nowTime - startTime > timeLimit || timeLimit == -1){

						if (clientSocket.connect(ip, port)){
							return true;
						}
					}
				}

				return false;
			}

			void Client::recvCycle(){

				while (1){

					char buff[1000];
					if (clientSocket.recv(buff, 1000) > 0){
						cout << buff << endl;
					}
					else{
						cout << "END CONN!" << endl;
						break;
					}
				}
			}

			std::thread* Client::recvCycleT(){

				std::thread* clRecvThread;
				clRecvThread = new std::thread(&TCP::Client::recvCycle, this);
				clRecvThread->detach();
				return clRecvThread;
			}

			int Client::send(char* buff, int len, int flags){

				return clientSocket.send(buff, len, flags);
			}

			Socket clientSocket;


			Server::Server(int port, U_LONG localAddr){

				serverSocket.bind(port, localAddr);
				serverSocket.listen();
			}

			void Server::acceptCycle(){

				while (1){

					Client *cl = new Client(serverSocket.accept());


					std::thread* clientFuncThread;
					clientFuncThread = new std::thread(&TCP::Server::clientFunc, this, cl);
					clientFuncThread->detach();
					//clientFunc(cl);
				}
			}

			std::thread* Server::acceptCycleT(){

				std::thread* acceptCycleThread;
				acceptCycleThread = new std::thread(&TCP::Server::acceptCycle, this);
				acceptCycleThread->detach();
				return acceptCycleThread;
			}


			void Server::clientFunc(Client* cl){

				cout << "new cl" << endl;

				clientList.add(cl);

				thread* clAcceptThread;
				cl->recvCycle();


				for (int i = clientList.getSize(); i >= 0; --i){

					if (clientList.get(i) == cl){
						clientList.remove(i);
						break;
					}
				}
			}

			int Server::send(int clId, char* buff, int len, int flags){

				clientList.get(clId)->send(buff, len, flags);
			}
		};




		namespace UDP{

			UDP::Socket::Socket(int port, U_LONG localAddr){

				_create(port, localAddr);
			}

			bool UDP::Socket::bind(int port, U_LONG localAddr){

				return Network::BSF::bind(_socket, port, localAddr);
			}

			int UDP::Socket::recvFrom(char* buff, int len, sockaddr_in* client_addr, int flags){

				return BSF::recvFrom(_socket, buff, len, client_addr, flags);
			}

			int UDP::Socket::sendTo(char* buff, int len, sockaddr_in* client_addr, int flags){

				return BSF::sendTo(_socket, buff, len, client_addr, flags);
			}

			bool UDP::Socket::_create(int port, U_LONG localAddr){

				return BSI::_create(INFO::type::DGRAM, INFO::protocol::UDP, port, localAddr);
			}
		};
	};
};
