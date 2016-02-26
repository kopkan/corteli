#include "list.h"

#include <iostream>
using namespace std;

void main(){

	List<char> C;
	C.add('2');
	C.add('1');
	C.add('9');

	cout <<"size="<< C.getSize() << endl;
	cout << "15(i)="<<(int)C.get(15)<<endl;
	cout << "1="<<C.get(1) << endl;

	//C.removeAll();
	cout << "remV 1="<<C.removeVal('1')<<endl;
	cout << "size=" << C.getSize() << endl;

	List<int> CC=*C.getValId('1', 1);
	cout<<CC.getSize()<<endl;

	C.removeAll();
	cout << C[0] << endl;


	List<char> *C2 = new List<char>;
	C2->add('2');
	C2->add('1');
	C2->add('9');

	//cout << C2[0] << endl;



	system("pause");
}
