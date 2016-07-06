#include "DynamicInt.h"


#include <iostream>
using namespace std;


using namespace corteli::base::type;

void main() 
{
	DynamicInt num;

	int len;
	num.set(-1234567891011121314);
	num.set(789837534345);

	cout << num.get(-1) << endl;

	char* buff = (char*)num.toByteArray();
	num.clear();



	//cout << num.get() << endl;


	int i = 0;
	while (!num.parse(&buff[i], 1))
	{
		//cout << num.isValueReady() << endl;
		cout << "step" << endl;
		//cout << num.get() << endl;
		i++;
	}





	cout << num.get() << endl;




	system("pause");
}