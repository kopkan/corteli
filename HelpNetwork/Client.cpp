#pragma once

#include "Client.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

Client::Client(io_service& io)
	: socket_(io)
{
}

Client::Client(tcp::socket socket)
	: socket_(move(socket))
{
	if (connectFunc_ != nullptr)
	{
		connectFunc_();
	}
}

Client::~Client()
{
	if (closeFunc_ != nullptr)
	{
		closeFunc_();
	}
}

time_t Client::getlastMessageTime()
{
	return lastRecvMessageTime_;
}

void Client::connect(Endpoint endpoint)
{
	auto self(shared_from_this());



	socket_.async_connect(endpoint.toBoostEndpoint(),
		[this, self](boost::system::error_code ec)
	{
		if (!ec)
		{
			if (connectFunc_ != nullptr)
			{
				connectFunc_();
			}
		}
		else
		{
			cout << "connect error=" << ec.value() << ' ' << ec.message() << endl;
			close();
		}
	});
}

void Client::startRecive()
{
	recvBuff_.resize(1000);
	recive_();
}

int Client::close()
{
	boost::system::error_code err;
	socket_.close(err);
	return err.value();
}

void Client::onRecive(function<void(corteli::base::container::Buffer<char>)> recvFunc)
{
	recvFunc_ = recvFunc;
}

void Client::onClose(function<void()> closeFunc)
{
	closeFunc_ = closeFunc;
}

void Client::onConnect(function<void()> connectFunc)
{
	connectFunc_ = connectFunc;
}

void Client::send(corteli::base::container::Buffer<char> message)
{
	auto self(shared_from_this());

	socket_.async_send(buffer(message.getBuff(), message.getSize()),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (ec)
		{
			cout << "send error=" << ec.value() << ' ' << ec.message() << endl;
			close();
		}
	});
}

void Client::recive_()
{
	lastRecvMessageTime_ = clock();

	auto self(shared_from_this());
	socket_.async_receive(boost::asio::buffer(recvBuff_.getBuff(), recvBuff_.getSize()),
		[this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			if (recvFunc_ != nullptr)
			{
				recvFunc_(recvBuff_);
			}
			recive_();
		}
		else
		{
			cout << "recive error=" << ec.value() << ' ' << ec.message() << endl;
			close();
		}
	});
}
