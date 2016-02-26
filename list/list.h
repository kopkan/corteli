#pragma once

#include <list>

#define LIST_C List<char*>
#define LIST_C2 List<LIST_C>
#define LIST_C3 List<LIST_C2>


template<typename T>
class List{

public:
	int add(T val);
	int remove(int num);
	int remove(int num, bool del);
	void removeAll();
	void removeAll(bool del);
	List<int>* getValId(T val, int listSize=0);
	int removeVal(T val);
	int removeVal(T val, bool del);
	T get(int num);
	int getSize();
	T operator [](int num);

private:
	std::list<T> valList;
};

#include "listRel.h"

