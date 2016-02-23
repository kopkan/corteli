#pragma once

#include <iostream>
using namespace std;

#include <winsock2.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib" )//socket
#pragma warning(disable : 4996)

#include <ctime>
#include <thread> 
#include <fstream>
#include <list>

#define LIST_C Corteli::List<char*>
#define LIST_C2 Corteli::List<LIST_C>
#define LIST_C3 Corteli::List<LIST_C2>


namespace Corteli{

	template<typename T>
	class List{

		list<T> valList;

	public:
		int add(T val);
		int remove(int num, bool del = 0);
		int removeAll(bool del = 0);
		int removeVal(T val, bool del = 0);
		int clear();
		T get(int num);
		T getNext();
		T getPre();
		list<T>* getValList();
		int getSize();
		T operator [](int num);

	};

#include "list.h"


	bool read_file(char *name, char *buff2);
	LIST_C* pars(char *buff, LIST_C*L, char delitel);
	int equal_str(char *str0, char *str);
	int random(int min, int max);//srand(clock()); NEED
	char *randomString(int size);//srand(clock()); NEED


#include "network.h"

}
