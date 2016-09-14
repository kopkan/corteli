#pragma once
#include <iostream>
#include <boost/asio.hpp>

class Endpoint
{
public:
	Endpoint();
	Endpoint(std::string name, unsigned short port);
	Endpoint(const Endpoint& ep);
	void setEndpoint(std::string name, unsigned short port);
	std::string getName() const;
	std::string getStringIp() const;
	unsigned short getPort() const;
	void clear();
	Endpoint& operator=(Endpoint& v);
	boost::asio::ip::tcp::endpoint toBoostEndpoint();

private:
	std::string name_;
	unsigned short port_;

};
