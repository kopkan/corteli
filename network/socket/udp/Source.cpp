#include "BaseSocket.h"
#include <map>

using namespace corteli::network::socket::udp;
using namespace corteli::network;
using namespace std;

void ff()
{
	while (clock()<2000)
	{
		cout << "tt=" << clock() <<"TE-"<< endl;
		Sleep(100);
	}

}


class Client
{
public:
	Client(boost::asio::ip::udp::endpoint remoteAddr)
	{
		cout << "Client!!!" << endl;
		_remoteAddr = remoteAddr;
	}

	~Client()
	{
		cout << "~~~Client!!!" << endl;
	}

	void sent()
	{
		cout << "SENTT!!!" << _remoteAddr << endl;
	}

private:
	boost::asio::ip::udp::endpoint _remoteAddr;

};


class Amphibian : public BaseSocket
{
public:
	Amphibian(IoService * ioService, bool enableDebugMessage) :BaseSocket(ioService, enableDebugMessage) {}

	unsigned long long sendTo(corteli::base::container::Buffer<char> buffer, corteli::network::Endpoint remoteEndpoint, int flag = 0)
	{

		boost::asio::ip::udp::endpoint remoteAddr(boost::asio::ip::udp::endpoint(boost::asio::ip::address_v4::from_string(remoteEndpoint.getCharIp()), remoteEndpoint.getPort()));

		std::map<boost::asio::ip::udp::endpoint, Client*>::iterator it= clMap.find(remoteAddr);

		if (it == clMap.end())
		{
			Client* cl = new Client(remoteAddr);
			clMap.insert(std::pair<boost::asio::ip::udp::endpoint, Client*>(remoteAddr, cl));
		}
		return BaseSocket::sendTo(buffer, remoteEndpoint, flag);
	}


protected:
	virtual void recvMessage(char*buff, std::size_t size, boost::asio::ip::udp::endpoint remoteAddr)
	{



	}
	virtual void recvError(const boost::system::error_code & ec, boost::asio::ip::udp::endpoint remoteAddr)
	{
		cout << "recvError"<< ec.value() << endl;
		//10061 ���������� �����������
		Client* cl = clMap.find(remoteAddr)->second;
		if (cl != clMap.end()->second)
		{
			delete cl;
			clMap.erase(remoteAddr);
		}

	}
	virtual void sentMessage(unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
	{
		std::map<boost::asio::ip::udp::endpoint, Client*>::iterator it = clMap.find(remoteAddr);
		if (it != clMap.end())
		{
			Client* cl = it->second;
			cl->sent();
		}
	}
	virtual void sendError(const boost::system::error_code& ec, unsigned long long messageId, boost::asio::ip::udp::endpoint remoteAddr)
	{
	}


private:
	std::map<boost::asio::ip::udp::endpoint, Client*> clMap;


};



void main()
{
	SetConsoleCP(1251);// ��������� ������� �������� win-cp 1251 � ����� �����
	SetConsoleOutputCP(1251); // ��������� ������� �������� win-cp 1251 � ����� ������


	IoService* io = new IoService(true);
	BaseSocket* s2 = new BaseSocket(io, true);
	s2->bind({ "127.0.0.1", 12345 });
	s2->startRecv(500);


	
	Amphibian* s = new Amphibian(io, true);

	cout << "bind=" << s->bind({ "127.0.0.1", 55555 }) << endl;
	cout << "startRecv=" << s->startRecv(500) << endl;
	
	
	s->sendTo({ "789", 4 }, { "127.0.0.1", 12345 });
	s->sendTo({ "789", 4 }, { "127.0.0.1", 12345 });

	s->sendTo({"789", 4}, { "127.0.0.1", 5588 });


	// ��� delete ����� ���� ������ ��-�� sendHandle �� ���������


	//delete s;
	//delete io;

	system("pause");

}