#include "corteli.h"

using namespace Corteli::Network;

int main() {

	Init();

	TCP::Server S(666);
	S.acceptCycleT();

	
	TCP::Client *C = new TCP::Client;
	C->connectCycle(getInetAddr("127.0.0.1"), 666);
	C->recvCycleT();

	//Sleep(1000);
	//delete C;

	
	TCP::Client C2;
	C2.connectCycle(getInetAddr("127.0.0.1"), 666);
	C2.recvCycleT();
	

	Sleep(100);
	cout<<"size="<<S.clientList.getSize()<<endl;

	system("pause");
	return 0;
}
