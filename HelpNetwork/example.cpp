#include "Acceptor.h"
#include "Client.h"


using namespace boost::asio;
using namespace boost::asio::ip;
using namespace std;

using namespace corteli::base::container;

#define PAUSE cout<<"PRESS KEY"<<endl; getchar();


void main()
{
	shared_ptr<io_service> io = make_shared<io_service>();



	//acceptor
	shared_ptr<Acceptor> acceptor = make_shared<Acceptor>(*io);
	
	acceptor->onAccept([](tcp::socket socket) 
	{
		cout << "new connection" << endl;
		shared_ptr<Client> client = make_shared<Client>(move(socket));
		weak_ptr <Client> weak_client = client;

		client->onRecive([weak_client](Buffer<char> message)
		{
			cout << message.getBuff() << endl;
			weak_client.lock()->send({"HTTP/1.0 200 OK\nContent-Type: text/html\nContent-Length: 3\n\n123", 63});
			weak_client.lock()->close();
		});


		client->startRecive();
	});


	acceptor->listen({"0.0.0.0", 80});
	//end acceptor



	//client
	shared_ptr<Client> client = make_shared<Client>(*io);
	weak_ptr <Client> weak_client = client;


	client->onConnect([weak_client]()
	{
		cout << "connected" << endl;
		weak_client.lock()->send({ "GET / HTTP/1.1\nHost: vk.com\n\n", 30 });
		weak_client.lock()->startRecive();
	});

	client->onRecive([weak_client](Buffer<char> message)
	{
		cout << message.getBuff() << endl;
		weak_client.lock()->close();
	});

	client->connect({"vk.com", 80});
	//end client


	io->run();

	PAUSE
	system("pause");
}
