#pragma once

#include "list.h"


template <typename T>
int List<T>::add(T val){

	valList.push_back(val);

	return 0;
}


template <typename T>
int List<T>::remove(int num, bool del){

	int i = 0;
	for (list<T>::iterator prt = valList.begin(); prt != valList.end(); prt++){

		if (i == num){

			if (del){
				delete *prt;
			}
			valList.erase(prt);

			return 1;
		}
		i++;
	}
	return 0;
}

template <typename T>
int List<T>::remove(int num){

	int i = 0;
	for (list<T>::iterator prt = valList.begin(); prt != valList.end(); prt++){

		if (i == num){

			valList.erase(prt);
			return 1;
		}
		i++;
	}
	return 0;
}


template <typename T>
void List<T>::removeAll(bool del){

	for (list<T>::iterator prt = valList.begin(); prt != valList.end();){

		void* valDelete = *prt;
		valList.erase(prt++);
		delete valDelete;
	}
}


template <typename T>
void List<T>::removeAll(){

	valList.clear();
}

template <typename T>
List<int>* List<T>::getValId(T val, int listSize){

	List<int>* valIdList = new List<int>;
	int i = 0;

	for (list<T>::iterator prt = valList.begin(); prt != valList.end(); ++prt){

		if (val == *prt){
			valIdList->add(i);
		}

		if (valIdList->getSize() == listSize && listSize!=0){
			break;
		}
		i++;
	}

	return valIdList;
}


template <typename T>
int List<T>::removeVal(T val, bool del){

	List<int>* remIdList = getValId(val);
	int delNum = remIdList->getSize();


	while (int remId = remIdList->get(0)){

		remove(remId);
		remIdList->remove(0, del);
	}

	delete remIdList;

	return delNum;
}


template <typename T>
int List<T>::removeVal(T val){

	List<int>* remIdList = getValId(val);
	int delNum = remIdList->getSize();
	

	while (int remId = remIdList->get(0)){

		remove(remId);
		remIdList->remove(0);
	}

	delete remIdList;

	return delNum;
}

template <typename T>
T List<T>::get(int num){

	if (num >= getSize() || num<0){

		return NULL;
	}

	int i = 0;
	for (std::list<T>::iterator prt = valList.begin(); prt != valList.end(); prt++){

		if (i == num){
			return *prt;
		}

		i++;
	}
}

template <typename T>
int List<T>::getSize(){

	return valList.size();
}

template <typename T>
T List<T>::operator [](int num){

	return get(num);
}
