
#include "BaseObject.h"

using namespace std;

class WW : public corteli::base::BaseObject
{
public:
	WW(bool i=0) : BaseObject(i)
	{
		write(100500);
		log("789");
		
	}
};


void main()
{
	WW q(1);
	//q.enabledDebugStream = 1;

	//corteli::base::BaseObject bo;

	/*
	bo.log("ferst");
	bo.log("2222222");
	bo.log("3333333");
	bo.log("4444444");


	char* buff = new char[10000000];
	buff[0] = '1';
	buff[1] = '2';
	buff[2] = '3';
	buff[3] = '\0';

	bo.log(buff);

	delete buff;


	string str = "string";
	bo.log(str);
	str = "qqq";


	bo.log("555555555");

	cout << str.c_str()<<endl;

	for (int i = 0; i < bo.getLogSize(); i++)
	{
		cout << bo.getLog(i).c_str() << endl;
	}
	*/

	system("pause");
}