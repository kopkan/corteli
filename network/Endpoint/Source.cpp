#include "Endpoint.h"


using namespace std;

void main()
{
	corteli::network::Endpoint e("Corteli-Note", 111);
	corteli::network::Endpoint e2("vk.com", 111);


	cout << e.getCharIp()<<endl;
	cout << e2.getCharIp() << endl;

	e = e2;

	cout << e.getCharIp() << endl;

	corteli::network::Endpoint e3({ "corteli.com", 111 });

	cout << e3.getCharIp() << endl;

	corteli::network::Endpoint e4(e3);

	cout << e4.getCharIp() << endl;

	system("pause");

}