#pragma once
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>

#include "D:/Develop/VisualStudio15Project/corteli/Base/BaseObject/BaseObject/BaseObject.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/Base/BaseObject/Release/BaseObject.lib")





class BaseClient : corteli::base::BaseObject
{
public:
	enum status
	{
		NOT_INIT = 0, INITED = 1, BOUND = 2, CONNECT = 3, CONNECTED = 4, RECV_STARTED = 5, END_WORK = 6, CLOSED=7
	};
	enum error
	{
		NO_ERR = 0, BIND_ERR = 1, CONNECT_ERR = 2, RECV_ERR = 3, SEND_ERR = 4, CLOSE = 100, CALL_DENIED = 200
	};

	BaseClient(boost::asio::io_service& ioService, bool enableDebugMessage = true);
	BaseClient(boost::asio::ip::tcp::socket socket, bool enableDebugMessage = true);
	~BaseClient();
	int getStatus();
	int getError();
	int bind(boost::asio::ip::tcp::endpoint localAddr);///
	int connect(boost::asio::ip::tcp::endpoint remoteAddr);
	int startRecv(std::size_t size);
	unsigned long long send(char* buff, int size, int flag=0);
	int close(bool noWaitEndWorkSignal = false);
	bool waitConnect(time_t time = 0);
	boost::asio::ip::tcp::socket& getSocket();
	void _acceptorThisSocketConnected();

	unsigned long long getLastRecvMessageTime();
	unsigned long long getLastSentMessageId();
	void runExpansion();

protected:
	virtual void connected();
	virtual void connectError(const boost::system::error_code & ec, boost::asio::ip::tcp::endpoint remoteAddr);
	virtual void recvMessage(char*buff, std::size_t size);
	virtual void recvError(const boost::system::error_code & ec);
	virtual void sendMessage(unsigned long long messageId);
	virtual void sendError(const boost::system::error_code& ec);
	virtual void endWork();

	virtual void connectExpansion();//call in pool
	virtual void chatExpansion();//call in pool

private:
	void _connectHandle(const boost::system::error_code & ec, boost::asio::ip::tcp::endpoint remoteAddr);
	void _recvHandle(const boost::system::error_code & ec, std::size_t bytes_transferred, std::size_t buffSize);
	void _sendHandle(const boost::system::error_code& ec, std::size_t bytes_transferred, unsigned long long messageId);
	void _setError(error err);

	int _status = status::NOT_INIT;
	int _error = error::NO_ERR;
	char* _recvBuff = NULL;
	unsigned long long _lastSentMessageId = 0;
	unsigned long long _lastRecvMessageTime = 0;

	

	boost::asio::ip::tcp::socket _socket;
};