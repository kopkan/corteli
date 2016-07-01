#pragma once


#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <thread>
#include <mutex>


#include "IoService.h"


namespace corteli {
	namespace network {
		namespace pipe {

			class BaseClient : public corteli::base::BaseObject
			{
			public:
				enum status
				{
					NOT_INIT = 0, INITED = 1, BOUND = 2, CONNECT = 3, CONNECTED = 4, RECV_STARTED = 5, END_WORK = 6, CLOSED = 7
				};
				enum error
				{
					NO_ERR = 0, BIND_ERR = 1, CONNECT_ERR = 2, RECV_ERR = 3, SEND_ERR = 4, CLOSE = 100, CALL_DENIED = 200
				};

				BaseClient(IoService* ioService, bool enabledDebugStream = true);
				//BaseClient(IoService* ioService, HANDLE hPipe, bool enabledDebugStream = false);
				virtual ~BaseClient();

				int getStatus();
				int getError();
				unsigned long long getLastRecvMessageTime();
				unsigned long long getLastSentMessageId();


				int connect(std::string pipename);
				int connectT(std::string pipename);
				bool waitConnected(time_t time=0);

				int startRecv(std::size_t size);

				unsigned long long send(char* buff, int size);
				int close(bool setEndWorkStatus=false);

				void runExpansion();

				void _acceptorSetPipeHandle(HANDLE pipe);

			protected:
				virtual void connected();
				virtual void connectError(DWORD ec, std::string pipename);
				virtual void recvMessage(char*buff, std::size_t size);
				virtual void recvError(const boost::system::error_code & ec);
				virtual void sentMessage(unsigned long long messageId);
				virtual void sendError(const boost::system::error_code& ec);
				virtual void endWork();

				virtual void connectExpansion();
				virtual void chatExpansion();

			private:
				//void _connectHandle(const boost::system::error_code & ec, boost::asio::ip::tcp::endpoint remoteAddr);
				void _recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, std::size_t buffSize);
				void _sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId);
				void _setError(error err);


				HANDLE _hPipe = NULL;
				boost::asio::windows::stream_handle _pipeStreamHandle;
				int _status = status::NOT_INIT;
				int _error = error::NO_ERR;
				char* _recvBuff = NULL;
				unsigned long long _lastSentMessageId = 0;
				unsigned long long _lastRecvMessageTime = 0;


				IoService* _ioService;
			};

		}
	}
}