#pragma once

#include "corteli.h"


template <typename T>
int Corteli::List<T>::add(T val){

	valList.push_back(val);

	return 0;
}


template <typename T>
int Corteli::List<T>::remove(int num, bool del){

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
int Corteli::List<T>::removeAll(bool del){


	if (del){

		for (list<T>::iterator prt = valList.begin(); prt != valList.end();){

			T valDelete = *prt;
			valList.erase(prt++);
			delete valDelete;
		}
	}
	else{
		valList.clear();
	}

	return 1;

}

template <typename T>
int Corteli::List<T>::removeVal(T val, bool del = 0){


	for (list<T>::iterator prt = valList.begin(); prt != valList.end(); prt++){

		if (val == *prt){

			if (del){
				delete *prt;
			}
			valList.erase(prt);

			return 1;
		}


	}

	return 0;

}

template <typename T>
T Corteli::List<T>::get(int num){

	int i = 0;
	for (list<T>::iterator prt = valList.begin(); prt != valList.end(); prt++){

		if (i == num){
			return *prt;
		}

		i++;

	}

}


template <typename T>
list<T>* Corteli::List<T>::getValList(){

	return &valList;
}


template <typename T>
int Corteli::List<T>::getSize(){

	return valList.size();
}

template <typename T>
T Corteli::List<T>::operator [](int num){

	return get(num);

}
