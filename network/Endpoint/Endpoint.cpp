#include "Endpoint.h"

#include <boost/asio.hpp>


using namespace corteli::network;


Endpoint::Endpoint()
{
	clear();
}

Endpoint::Endpoint(char* name, unsigned short port)
{
	setEndpoint(name, port);
}

Endpoint::Endpoint(const Endpoint& ep)
{
	setEndpoint(ep.getName(), ep.getPort());
}

Endpoint::~Endpoint()
{
	_clear();
}

void Endpoint::setEndpoint(char* name, unsigned short port)
{
	if (name != NULL) 
	{
		_clear();
		int namesize = strlen(name) + 1;
		_name = new char[namesize];
		memcpy(_name, name, namesize);
	}
	else 
	{
		clear();
	}
	_port = port;
}

char* Endpoint::getName() const
{
	return _name;
}


char* Endpoint::getCharIp() 
{
	hostent *h;

	h = gethostbyname(_name);
	if (h)
	{
		for (int i = 0; h->h_addr_list[i]; ++i)
		{
			sockaddr_in addr;
			memcpy(&addr.sin_addr, h->h_addr_list[i], h->h_length);
			return inet_ntoa(addr.sin_addr);
		}
	}
	else
	{
		return _name;
	}

	return _name;
}

unsigned short Endpoint::getPort() const
{
	return _port;
}

void Endpoint::clear()
{
	_clear();
	_name = new char[8];
	strcpy_s(_name, 8, "0.0.0.0");
}

Endpoint& Endpoint::operator=(Endpoint& ep2)
{
	setEndpoint(ep2.getName(), ep2.getPort());

	return *this;
}

void Endpoint::_clear()
{
	delete _name;
	_port = 0;
}
