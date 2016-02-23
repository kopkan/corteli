#include "corteli.h"

using namespace Corteli::Network;

int main() {

	Init();

	TCP::Server S(666);
	S.acceptCycleT();

	
	TCP::Client C;
	C.connectCycle(getInetAddr("127.0.0.1"), 666);
	C.recvCycleT();

	
	TCP::Client C2;
	C2.connectCycle(getInetAddr("127.0.0.1"), 666);
	C2.recvCycleT();
	

	system("pause");
	return 0;
}
