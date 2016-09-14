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
	shared_ptr<Acceptor> acceptor = make_shared<Acceptor>(*io);
	
	acceptor->onAccept([](tcp::socket socket) 
	{
		cout << "new connection" << endl;
		shared_ptr<Client> client = make_shared<Client>(move(socket));

		client->onRecive([](Buffer<char> message)
		{
			cout << message.getBuff() << endl;
		});


		client->startRecive();
	});


	acceptor->listen(tcp::endpoint(tcp::v4(), 80));
	io->run();

	PAUSE
	system("pause");
}
