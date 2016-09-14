#include "Acceptor.h"

using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

Acceptor::Acceptor(io_service& io)
	: acceptor_(io)
	, client_(io)
{
}

Acceptor::~Acceptor()
{
	if (closeFunc_ != nullptr)
	{
		closeFunc_();
	}
}

void Acceptor::onAccept(function<void(tcp::socket)> func)
{
	acceptFunc_ = func;
}

void Acceptor::onClose(function<void()> func)
{
	closeFunc_ = func;
}

void Acceptor::listen(tcp::endpoint endpoint)
{
	acceptor_.open(boost::asio::ip::tcp::v4());
	acceptor_.bind(endpoint);

	acceptor_.listen(1000);
	accept_();
}

void Acceptor::close()
{
	acceptor_.close();
}

void Acceptor::accept_()
{

	auto self(shared_from_this());

	acceptor_.async_accept(client_,
		[this, self](boost::system::error_code ec)
	{
		if (!ec)
		{
			if (acceptFunc_ != nullptr)
			{
				acceptFunc_(move(client_));
			}
			accept_();
		}
		else
		{
			cout << ec.value() << ' ' << ec.message() << endl;
			close();
		}
	});
}
