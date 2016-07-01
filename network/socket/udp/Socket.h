#pragma once

#include "IoService.h"


namespace corteli {
	namespace network {
		namespace socket {
			namespace udp {

				class Socket : public corteli::base::BaseObject
				{
				public:
					enum status
					{
						NOT_INIT = 0, INITED = 1, BOUND = 2, RECV_STARTED = 3, END_WORK = 4, CLOSED = 7
					};
					enum error
					{
						NO_ERR = 0, BIND_ERR = 1, RECV_ERR = 2, SEND_ERR = 3, CLOSE = 100, CALL_DENIED = 200
					};

					Socket(IoService* ioService, bool enableDebugMessage = true);
					~Socket();
					int getStatus();
					int getError();
					int bind(boost::asio::ip::udp::endpoint localAddr);
					int startRecv(std::size_t size);
					unsigned long long sendTo(char* buff, int size, boost::asio::ip::udp::endpoint remoteAddr, int flag = 0);
					int close();
					int closeT();

					boost::asio::ip::udp::socket& getSocket();
					void reload();

				protected:
					virtual void recvMessage(char*buff, std::size_t size, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void recvError(const boost::system::error_code & ec, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void sendMessage(unsigned long long messageId);
					virtual void sendError(const boost::system::error_code& ec);
					virtual void endWork();

				private:
					void _recvHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, std::size_t buffSize, boost::asio::ip::udp::endpoint remoteAddr);
					void _sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId);
					void _setError(error err);

					int _status = status::NOT_INIT;
					int _error = error::NO_ERR;
					char* _recvBuff = NULL;
					unsigned long long _lastSentMessageId = 0;

					IoService* _ioService;
					boost::asio::ip::udp::socket _socket;
				};

			}
		}
	}
}