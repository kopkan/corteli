#pragma once

#include "D:\Develop\VisualStudio15Project\corteli\network\IoService\IoService\IoService.h"

#include "D:\Develop\VisualStudio15Project\corteli\Base\BaseContainers\BaseContainers\Buffer.h"

namespace corteli {
	namespace network {
		namespace socket {
			namespace udp {

				class Socket : public corteli::base::BaseObject
				{
				public:
					enum status
					{
						NOT_INIT = 0, INITED = 1, BOUND = 2, RECV_STARTED = 3, END_WORK = 4, CLOSED = 5
					};
					enum error
					{
						NO_ERR = 0, BIND_ERR = 1, RECV_ERR = 2, SEND_ERR = 3, CLOSE = 100, CALL_DENIED = 200
					};

					Socket(corteli::network::IoService* ioService, bool enableDebugMessage = true);
					~Socket();
					int getStatus();
					int getError();
					int bind(char* ip, unsigned short port);
					int startRecv(std::size_t size);
					unsigned long long sendTo(char* buff, int size, char* ip, unsigned short port, int flag = 0);
					int close(bool setEndWorkStatus=false, bool closingFlag=false);
					int closeT(bool setEndWorkStatus=false, bool closingFlag=false);
					bool waitClose(time_t time = 0);

					boost::asio::ip::udp::socket& getSocket();
					void reload();

				protected:
					virtual bool recvContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void recvMessage(char*buff, std::size_t size, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void recvError(const boost::system::error_code & ec, boost::asio::ip::udp::endpoint remoteAddr);
					virtual bool sendContainsError(const boost::system::error_code & ec, std::size_t bytes_transferred, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void sentMessage(unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void sendError(const boost::system::error_code& ec, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr);
					virtual void preClosing(bool closingFlag);
					virtual void afterClosing(bool closingFlag);

				private:
					void _recvHandle(const boost::system::error_code& ec, std::size_t bytes_transferred);
					void _sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr);
					void _setError(error err);
					void _setStatus(status st);

					int _status = status::NOT_INIT;
					int _error = error::NO_ERR;

					corteli::base::container::Buffer<char> _recvBuff;
					boost::asio::ip::udp::endpoint _remoteAddr;
					unsigned long long _lastSentMessageId = 0;
					unsigned short _countSendWaiting = 0;
					unsigned short _countCloseWaiting = 0;

					corteli::network::IoService* _ioService;
					boost::asio::ip::udp::socket _socket;
				};

			}
		}
	}
}