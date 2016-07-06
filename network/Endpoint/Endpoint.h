#pragma once
#include <iostream>


namespace corteli
{
	namespace network
	{

		class Endpoint
		{
		public:
			Endpoint();

			Endpoint(char* name, unsigned short port);

			Endpoint(const Endpoint& ep);

			~Endpoint();

			void setEndpoint(char* name, unsigned short port);

			char* getName() const;

			char* getCharIp();

			unsigned short getPort() const;

			void clear();

			Endpoint& operator=(Endpoint& v);

		private:
			void _clear();

			char* _name = NULL;
			unsigned short _port = 0;

		};

	}
}

#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/network/Endpoint/Release/Endpoint.lib")