#pragma once

#include <boost\asio.hpp>
#include <memory>
#include <iostream>

class Acceptor
	: public std::enable_shared_from_this<Acceptor>
{
public:
	Acceptor(boost::asio::io_service& io);
	~Acceptor();
	void onAccept(std::function<void(boost::asio::ip::tcp::socket)> func);
	void onClose(std::function<void()> func);
	void listen(boost::asio::ip::tcp::endpoint endpoint);
	void close();

private:
	void accept_();

	boost::asio::ip::tcp::acceptor acceptor_;
	boost::asio::ip::tcp::socket client_;
	std::function<void(boost::asio::ip::tcp::socket)> acceptFunc_ = nullptr;
	std::function<void()> closeFunc_ = nullptr;
};
