#include "Header.h"

char* strToChar(std::string str) {

	char* buff = new char[100];

	for (int i = str.size(); i >= 0; --i) {

		buff[i] = str[i];
	}

	return buff;
}
