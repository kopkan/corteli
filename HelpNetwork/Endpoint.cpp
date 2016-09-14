#include "Endpoint.h"

using namespace boost::asio::ip;

Endpoint::Endpoint()
{
	clear();
}

Endpoint::Endpoint(std::string name, unsigned short port)
{
	setEndpoint(name, port);
}

Endpoint::Endpoint(const Endpoint& ep)
{
	setEndpoint(ep.getName(), ep.getPort());
}

void Endpoint::setEndpoint(std::string name, unsigned short port)
{
	name_ = name;
	port_ = port;
}

std::string Endpoint::getName() const
{
	return name_;
}


std::string Endpoint::getStringIp() const
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);

	boost::asio::ip::tcp::resolver::iterator i = resolver.resolve({ name_, "" });

	return (*i).endpoint().address().to_string();
}

unsigned short Endpoint::getPort() const
{
	return port_;
}

void Endpoint::clear()
{
	name_ = "0.0.0.0";
	port_ = 0;
}

Endpoint& Endpoint::operator=(Endpoint& ep2)
{
	setEndpoint(ep2.getName(), ep2.getPort());
	return *this;
}

tcp::endpoint Endpoint::toBoostEndpoint()
{
	return tcp::endpoint(address_v4::from_string(getStringIp()), getPort());
}
