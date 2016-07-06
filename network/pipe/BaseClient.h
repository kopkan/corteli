#pragma once


#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <thread>
#include <mutex>


#include "D:\Develop\VisualStudio15Project\corteli\network\IoService\IoService\IoService.h"
#include "D:\Develop\VisualStudio15Project\corteli\Base\BaseContainers\BaseContainers\Buffer.h"
#include "D:\Develop\VisualStudio15Project\corteli\network\Endpoint\Endpoint\Endpoint.h"


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

				BaseClient(corteli::network::IoService* ioService, bool enabledDebugStream = true);
				virtual ~BaseClient();

				int getStatus();
				int getError();
				unsigned long long getLastRecvMessageTime();
				unsigned long long getLastSentMessageId();


				int connect(std::string pipename);
				int connectT(std::string pipename);
				bool waitConnect(time_t time=0);
				int startRecv(std::size_t size);
				unsigned long long send(corteli::base::container::Buffer<char> buffer);
				int close(bool setEndWorkStatus=false, bool closingFlag = false);
				int closeT(bool setEndWorkStatus = false, bool closingFlag = false);
				bool waitClose(time_t time = 0);

				void runExpansion();
				void reload();

				void _acceptorSetPipeHandle(HANDLE pipe);

			protected:
				virtual void connected();
				virtual void connectError(DWORD ec, std::string pipename);
				virtual bool recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred);
				virtual void recvMessage(corteli::base::container::Buffer<char> buffer);
				virtual void recvError(const boost::system::error_code & ec);
				virtual bool sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId);
				virtual void sentMessage(unsigned long long messageId);
				virtual void sendError(const boost::system::error_code& ec, unsigned long long messageId);
				virtual void preClosing(bool closingFlag);
				virtual void afterClosing(bool closingFlag);

				virtual void connectExpansion();
				virtual void chatExpansion();

			private:
				void _recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred);
				void _sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId);
				void _setError(error err);
				void _setStatus(status status);

				HANDLE _hPipe = NULL;
				boost::asio::windows::stream_handle _pipeStreamHandle;
				int _status = status::NOT_INIT;
				int _error = error::NO_ERR;
				corteli::base::container::Buffer<char> _recvBuff;
				unsigned long long _lastSentMessageId = 0;
				unsigned long long _lastRecvMessageTime = 0;
				unsigned short _countSendWaiting = 0;
				unsigned short _countCloseWaiting = 0;

				corteli::network::IoService* _ioService;
			};

		}
	}
}