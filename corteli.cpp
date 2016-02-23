#include "corteli.h"

bool Corteli::read_file(char *name, char *buff2){

	fstream fin;
	char buff[500000];

	fin.open(name);

	if (fin){
		int i2 = 0;
		while (!fin.eof()){
			fin.getline(buff, 500000);
			for (int i = 0; buff[i] != 0; i++){
				buff2[i2] = buff[i];
				i2++;
			}
			buff2[i2] = '\n';
			i2++;
		}
		buff2[i2] = '\0';

	}
	else{
		buff2[0] = '\0';
		return 0;
	}

	fin.close();

	return 1;
}



LIST_C* Corteli::pars(char *buff, LIST_C*L, char delitel = ' '){//данный пример делит по пробелам

	int word_len;
	int word_split = 0;
	char *lexeme;

	if (!L){ L = new LIST_C; }



	for (int i = 0; true; i++){

		word_split = 0;

		for (word_len = 0; true; word_len++){

			if (buff[i + word_len] == delitel){
				word_split = 1;
				break;
			}

			if (buff[i + word_len] == '\0'){
				word_split = 0;
				break;
			}

		}

		if (word_len == 0){

			if (word_split == 1){
				continue;
			}
			if (word_split == 0){
				break;
			}

		}

		lexeme = new char[word_len + 1];


		for (int u = 0; u < word_len; u++){

			lexeme[u] = buff[i + u];

		}
		lexeme[word_len] = '\0';
		i += word_len;

		L->add(lexeme);


		if (!word_split){
			break;
		}

	}


	return L;

}


int Corteli::equal_str(char *str0, char *str){

	for (int i = 0; str0[i] == str[i]; i++){
		if (str0[i] == 0){
			return 1;
		}
	}

	return 0;
}



int Corteli::random(int min, int max){

	return rand() % ((max)-(min)+1) + (min);
}

char* Corteli::randomString(int size){

	char *str = new char[size + 1];

	char helpSet[] = "qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM123456789";
	int helpSetSize = strlen(helpSet) - 1;


	for (int i = 0; i < size; i++){

		str[i] = helpSet[random(0, helpSetSize)];
	}
	str[size] = '\0';

	return str;
}
