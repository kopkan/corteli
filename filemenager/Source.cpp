#include "Header.h"






class session
{
public:
	session(boost::asio::io_service& io_service)
		: socket_(io_service)
	{
	}

	tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{

		boost::asio::async_write(socket_,
			boost::asio::buffer("жопа", 5),
			boost::bind(&session::handle_write2, this,
				boost::asio::placeholders::error));


		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			boost::bind(&session::handle_read, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred));

	}

private:
	void handle_read(const boost::system::error_code& error,
		size_t bytes_transferred)
	{
		if (!error)
		{
			cout << bytes_transferred<<'-' <<endl;

			/*
			for (int i = 0; i < bytes_transferred; i++)
			{
				cout << i << ' ' << (int)data_[i] <<' '<< data_[i]<<endl;

			}
			cout << data_ << endl;
			*/
			cout <<data_<< endl;





			if (data_[0] == 'f')
			{

				cout <<(int) data_[0]<<endl;
				cout << (int)data_[1]<<endl;

				char* mes = seeCat("d:\\");

				boost::asio::async_write(socket_,
					boost::asio::buffer(mes, strlen(mes) + 1),
					boost::bind(&session::handle_write, this,
						boost::asio::placeholders::error));

				return;
			}


			boost::asio::async_write(socket_,
				boost::asio::buffer(data_, bytes_transferred),
				boost::bind(&session::handle_write, this,
					boost::asio::placeholders::error));
		}
		else
		{
			delete this;
		}
	}

	void handle_write2(const boost::system::error_code& error)
	{

	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
				boost::bind(&session::handle_read, this,
					boost::asio::placeholders::error,
					boost::asio::placeholders::bytes_transferred));
		}
		else
		{
			delete this;
		}
	}

	tcp::socket socket_;
	enum { max_length = 1024 };
	char data_[max_length];
};

class server
{
public:
	server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service),
		acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
	{
		start_accept();
	}

private:
	void start_accept()
	{
		session* new_session = new session(io_service_);
		acceptor_.async_accept(new_session->socket(),
			boost::bind(&server::handle_accept, this, new_session,
				boost::asio::placeholders::error));
	}

	void handle_accept(session* new_session,
		const boost::system::error_code& error)
	{
		if (!error)
		{
			new_session->start();
		}
		else
		{
			delete new_session;
		}

		cout << "new" << new_session->socket().remote_endpoint()<<endl;

		start_accept();
	}

	boost::asio::io_service& io_service_;
	tcp::acceptor acceptor_;
};


/*
#include <deque>

class DataContainer
{
public:
	enum types
	{
		boolType = 1, charType, wcharType, sintType, intType, floatType, longType, doubleType
	};


	void setName(char* name, char nameLen)
	{
		if (name != NULL)
		{
			if (nameLen == -1)
			{
				nameLen = strlen(name) + 1;
			}

			if (nameLen > 0)
			{
				this->name = new char[nameLen];

				memcpy((void*)this->name, name, nameLen);
				this->name[nameLen] = '\0';
			}
		}
		else
		{
			this->name = NULL;
			this->nameLen = 0;
		}
	}

	char* getName()
	{
		return name;
	}

	void setData(char* type, void* data, int dataLen)
	{

		this->type = 0;
		if (strcmp(type, "bool") == 0) { this->type = boolType; }
		if (strcmp(type, "char") == 0) { this->type = charType; }
		if (strcmp(type, "wchar_t") == 0) { this->type = wcharType; }
		if (strcmp(type, "short") == 0) { this->type = sintType; }
		if (strcmp(type, "int") == 0) { this->type = intType; }
		if (strcmp(type, "float") == 0) { this->type = floatType; }
		if (strcmp(type, "long") == 0) { this->type = longType; }
		if (strcmp(type, "double") == 0) { this->type = doubleType; }

		cout << type << endl;

		if (this->type != 0)
		{
			this->data = data;
			this->dataLen = dataLen;
		}
	}

	void* getData()
	{
		return data;
	}

	int getDataSize()
	{
		return dataLen;
	}

	char getType()
	{
		return type;
	}



private:
	char* name; char nameLen;
	//char* attr; char attrLen;
	char type;
	void* data; int dataLen;



};

class MyPars
{
public:
	template<typename T>
	void add(T* data, int dataLen, char* name=NULL, int nameLen=-1)
	{
		DataContainer* dataContainer = new DataContainer;
		dataContainer->setName(name, nameLen);
		dataContainer->setData((char*)typeid(T).name(), data, dataLen);
		this->data.push_back(dataContainer);

	}

	void see()
	{
		for (int i = 0; i < data.size(); i++)
		{
			cout << "name=" << (int)data.at(i)->getName() << endl;
			cout << "type=" << (int)data.at(i)->getType() << endl;


			cout << "dataSize=" << (int)data.at(i)->getDataSize() << endl;

			switch (data.at(i)->getType())
			{

			case 1:
			{
				for (int ii = 0; i < (int)data.at(i)->getDataSize(); ii++)
				{
					cout << ((bool*)data.at(i)->getData())[ii] << endl;
					//cout << ((bool*)data.at(i)->getData()) << endl;
				}
			}
			case 2:
			{
				for (int ii = 0; ii < (int)data.at(i)->getDataSize(); ii++)
				{
					cout << ((char*)data.at(i)->getData())[ii] << endl;
					//cout << ((char*)data.at(i)->getData()) << endl;
				}
			}

			default:
				break;
			}

			//cout << ((char*)data.at(i)->getData()) << endl;

			cout << endl;
		}

	
	}

	void addCharArr(char* name, char nameLen, char* data, int dataLen)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'C';
		ptr++;


		memcpy((void*)&buff[ptr], (void*)&dataLen, 4);
		ptr +=4;

		for (int i = 0; i < dataLen; i++)
		{
			buff[ptr] = data[i];
			ptr++;
		}
	}

	void addChar(char* name, char nameLen, char data)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'c';
		ptr++;


		buff[ptr] = data;
		ptr++;
	}

	void addIntArr(char* name, char nameLen, int* data, int dataLen)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'I';
		ptr++;


		memcpy((void*)&buff[ptr], (void*)&dataLen, 4);
		ptr += 4;

		for (int i = 0; i < dataLen; i++)
		{
			memcpy((void*)&buff[ptr], (void*)&data[i], 4);
			ptr += 4;
		}
	}

	void addInt(char* name, char nameLen, int data)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'i';
		ptr++;


		memcpy((void*)&buff[ptr], (void*)&data, 4);
		ptr += 4;
	}

	void addDoubleArr(char* name, char nameLen, double* data, int dataLen)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'D';
		ptr++;


		memcpy((void*)&buff[ptr], (void*)&dataLen, 4);
		ptr += 4;

		for (int i = 0; i < dataLen; i++)
		{
			memcpy((void*)&buff[ptr], (void*)&data[i], 8);
			ptr += 8;
		}
	}

	void addDouble(char* name, char nameLen, double data)
	{
		_nameVal(name, nameLen);

		buff[ptr] = 'd';
		ptr++;

		memcpy((void*)&buff[ptr], (void*)&data, 8);
		ptr += 8;
	}





	bool see()
	{
		char* name;
		char type;

		for (int i = 0; i < ptr; i++)
		{
			name= _takeDataArray<char, char>(buff, i, ptr);
			type = _takeData<char>(buff, i, ptr);


			switch (type)
			{
			case 'C':
			{
				char* str = _takeDataArray<char, int>(buff, i, ptr);

				cout << name << ' ' << str << endl;
				break;
			}
			case 'd':
			{
				double str= _takeData<double>(buff, i, ptr);

				cout << name << ' ' << str << endl;
				break;
			}


			default:
				return 0;
				break;
			}

			i--;
			continue;
		}

		return 1;

	}

	template <typename T, typename I>
	void _push(char* name, T* data, I dataSize, char nameSize = -1)
	{
		if (nameSize == -1)
		{
			nameSize = strlen(name);
		}

		memcpy((void*)&buff[ptr], (void*)&nameSize, sizeof(char));
		ptr += sizeof(char);

		memcpy((void*)&buff[ptr], (void*)name, nameSize);
		ptr += nameSize;


		char type;
		if (strcmp(typeid(T).name(), "char"))
		{
			type = 'C';
		}
		if (strcmp(typeid(T).name(), "int"))
		{
			type = 'i';
		}

		memcpy((void*)&buff[ptr], (void*)&dataSize, sizeof(I));
		ptr += 1;


		memcpy((void*)&buff[ptr], (void*)data, sizeof(T)*dataSize);
		ptr += sizeof(T)*dataSize;
	}


private:
	template <typename T>
	T _takeData(char* buff, int& startPoz, int maxPoz)
	{
		T* tmp = _takeDataArray<T>(buff, 1, startPoz, maxPoz);
		T result = tmp[0];
		delete tmp;

		return result;
	}

	template <typename T>
	T* _takeDataArray(char* buff, int size, int& startPoz, int maxPoz)
	{
		if (startPoz + size > maxPoz) { throw "PARS ERR"; return NULL; }

		T* result = new T[size];
		memcpy((void*)result, (void*)&buff[startPoz], sizeof(T)*size);
		startPoz += sizeof(T)*size;

		return result;
	}

	template <typename T, typename I>
	T* _takeDataArray(char* buff, int& startPoz, int maxPoz)
	{
		I size = _takeData<I>(buff, startPoz, maxPoz);
		T* result = _takeDataArray<T>(buff, size, startPoz, maxPoz);

		return result;
	}

	void _nameVal(char* name, char nameLen)
	{
		buff[ptr] = nameLen;
		ptr++;

		for (int i = 0; i < nameLen; i++)
		{
			buff[ptr] = name[i];
			ptr++;
		}
	}



	char buff[70000];
	int ptr = 0;


private:
	std::deque<DataContainer*> data;


};
*/


unsigned char* setNum(unsigned long long val, int &len)
{
	unsigned char tmpBuff[100];


	int i;
	for (i = 0; true; i++)
	{
		tmpBuff[i] = val % 128;
		if (val = val / 128)
		{
			tmpBuff[i] += 128;
		}
		else
		{
			break;
		}
	}
	i++;

	unsigned char* strVal = new unsigned char[i];
	memcpy((void*)strVal, (void*)tmpBuff, i);


	len = i;

	return strVal;
}

unsigned long long pow2(unsigned long long val, unsigned long long order)
{
	unsigned long long ret = 1;

	while (order--)
	{
		ret *= val;
	}

	return ret;
}

unsigned long long getNum(unsigned char* strVal, int &len)
{
	unsigned long long val = 0;
	int i;
	for (i = 0; true; i++)
	{

		val += (strVal[i] % 128)*pow2(128, i);
		if ((strVal[i] / 128) == 0)
		{
			break;
		}

	}
	len = i+1;

	return val;
}








class DataContainer
{
public:

	void setData(void* data, int dataLen)
	{
		this->data = data;
		this->dataLen = dataLen;
	}

	void* getData()
	{
		return data;
	}

	int getDataSize()
	{
		return dataLen;
	}

	char* toString(int& size)
	{
		int sizeStrDataLen;
		char* strDataLen=(char*)setNum(dataLen, sizeStrDataLen);


		int sizeStr= sizeStrDataLen + dataLen;

		char* str = new char[sizeStr];
		memcpy((void*)str, (void*)strDataLen, sizeStrDataLen);
		memcpy((void*)&str[sizeStrDataLen], (void*)data, dataLen);

		size = sizeStr;
		delete strDataLen;

		return str;
	}

	int fromString(char* str)
	{
		int dataLenSize;
		dataLen = getNum((unsigned char*)str, dataLenSize);

		data = new char[dataLen];

		memcpy((void*)data, (void*)&str[dataLenSize], dataLen);

		return dataLenSize + dataLen;
	}

private:
	void* data; int dataLen;

};


class DT
{
public:
	void setData(char* name, void* data, unsigned long long dataLen, unsigned long long nameLen=-1)
	{
		if (name != NULL)
		{
			if (nameLen == -1)
			{
				nameLen = strlen(name);
			}
		}

		this->name.setData(name, nameLen);
		this->data.setData(data, dataLen);
	}

	char* toString(int& size)
	{
		int nameStrSize;
		char* nameStr=this->name.toString(nameStrSize);

		int dataStrSize;
		char* dataStr = this->data.toString(dataStrSize);

		int sizeStr = nameStrSize + dataStrSize;

		char* str = new char[sizeStr];
		memcpy((void*)str, (void*)nameStr, nameStrSize);
		memcpy((void*)&str[nameStrSize], (void*)dataStr, dataStrSize);

		delete nameStr;
		delete dataStr;


		size = sizeStr;
		return str;
	}


	int fromString(char* str)
	{
		int len=this->name.fromString(str);
		len = this->data.fromString(&str[len]);
		return len;
	}

	void see()
	{
		cout << (char*)this->name.getData()<<endl;
		cout << this->name.getDataSize() << endl;
		cout << (char*)this->data.getData() << endl;
		cout << this->data.getDataSize() << endl;
	}

	DataContainer* getName()
	{
		return &name;
	}

	DataContainer* getData()
	{
		return &data;
	}

private:
	DataContainer name;
	DataContainer data;

};

#include <deque>

class MyPars
{
public:
	void add(char* name, void* data, unsigned long long dataLen, unsigned long long nameLen = -1)
	{
		DT* dt=new DT;
		dt->setData(name, data, dataLen);

		_data.push_back(dt);
		
	}

	template<typename T>
	T* get(int num)
	{
		return (T*)_data[num]->getData()->getData();
	}


	char* toString(int &len)
	{
		std::deque<char*> _tmpStr;
		std::deque<int> _tmpStrSize;

		int size = 0;
		for (int i = 0; i < _data.size(); i++)
		{
			int s;
			_tmpStr.push_back(_data[i]->toString(s));
			_tmpStrSize.push_back(s);
			size += s;
		}

		char* str = new char[size];

		int sz = 0;
		for (int i = 0; i < _tmpStr.size(); i++)
		{
			memcpy((void*)&str[sz], (void*)_tmpStr[i], _tmpStrSize[i]);

			delete _tmpStr[i];

			sz += _tmpStrSize[i];
		}
	
		len = size;
	}

	std::deque<DT*> _data;
};



char* ff2(string str, int& size)
{
	char* dataT = new char[str.size()];
	
	int fl = 0; 
	int q = 0;
	for (int i = 0; i<str.size(); i++)
	{
		if (str.at(i) == '\1') { fl = 1; continue; }
		if (fl == 1)
		{
			if (str.at(i) == '\2')
			{
				dataT[q] = '\0';
				q++;
				fl = 0;
				continue;
			}
			if (str.at(i) == '\3')
			{
				dataT[q] = '\1';
				q++;
				fl = 0;
				continue;
			}
		}

		dataT[q] = str.at(i);
		q++;
	}
	char* data = new char[q];

	memcpy((void*)data, (void*)dataT, q);

	delete dataT;

	size = q;

	return data;
}


string ff(char* data, int size) //экранирование
{
	string str;
	for (int i = 0; i < size; i++)
	{
		if (data[i] == '\1')
		{
			str.push_back('\1');
			str.push_back('\3');
			continue;
		}
		if (data[i] == '\0')
		{
			str.push_back('\1');
			str.push_back('\2');
			continue;
		}
		str.push_back(data[i]);
	}

	return str;
}


int main(int argc, char* argv[])
{
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода



	unsigned char mask[8];

	for (int i = 0; i < 8; i++)
	{
		mask[i] = pow(2, i);
	}

	string S =  ff("123\0\1\3", 6);

	int ss;
	char* D = ff2(S, ss);

	cout << S << ' ' << S.size() << endl;
	cout << D << ' ' << ss << endl;


	/*
	DT Q;
	Q.setData("NAME", "DATA", 4);
	Q.see();

	int s;
	char* data = Q.toString(s);
	cout << s<<' '<<data<<endl;


	DT Q2;
	Q2.fromString(data);
	Q2.see();
	

	//cout << a<<' '<<a.size();
	*/

	/*
	MyPars a;
	a.add(0, "DATA", 4);
	a.add(0, "DATA123", 7);
	cout<<a.get<char>(0)<<endl;
	cout << a.get<char>(1) << endl;
	*/







	/*
	DataContainer D;
	D.setData("мясо", 4);
	cout << (char*)D.getData() << endl;


	
	int size;
	char*data =  D.toString(size);
	cout << size << endl;
	

	cout << data<< endl;
	
	DataContainer D2;
	size=D2.fromString(data);

	cout << size <<endl;
	cout << (char*)D2.getData() << endl;
	cout <<  D2.getDataSize() << endl;
	*/




	boost::property_tree::ptree heroTree;


	char qq[256];
	for (int i = 0; i < 256; i++) 
	{

		if (i == 0) 
		{
			i = 1;
		}
		qq[i] = i;
	}

	heroTree.put("cat", qq);
	//heroTree.put("Exp", 150);
	//heroTree.put("Weapon", "<Weapon>Blue Sword</Weapon>");
	//heroTree.put("Money", 3000);
	//int exp = heroTree.get<int>("Exp");
	//std::string weapon = heroTree.get<std::string>("Weapon");
	//cout << exp<<' '<< weapon << endl;
	cout<<heroTree.get<std::string>("cat");

	stringstream output_stream;
	boost::property_tree::write_xml(output_stream, heroTree);
	string str = output_stream.str();

	//cout << str.size();
	//cout << str;



	//std::string filename="d:\\1.xml";
	//boost::property_tree::write_xml(filename, heroTree);

	//cout << filename;




	/*
	unsigned char val = 255;

	char type = (char)(val & (mask[0] | mask[1] | mask[2]));
	char len = (char)((val & (mask[3] | mask[4]))/mask[3]);

	bool flag[3];
	for (int i = 0; i < 3; i++)
	{
		flag[i] = (bool)(val & (mask[i+5]));
	}

	cout << (int)val << endl;

	cout << (int)type << endl;
	cout << (int)len << endl;
	cout << flag[0] << endl;//use the name?
	cout << flag[1] << endl;//use the attr?
	cout << flag[2] << endl;//this combo data(only table)?
	*/


	/*
	unsigned char val = 255;

	int ver = (val & (mask[0] | mask[1] | mask[2] | mask[3]));
	int type = ((val & (mask[4] | mask[5])) / mask[4]);
	int len = ((val & (mask[6] | mask[7])) / mask[6]);


	cout << (int)ver << endl;
	cout << (int)type << endl;
	cout << (int)len << endl;



	void** a=new void*[100];
	a[0] = new char[100];

	a[1] = new double[1];

	a[2] = new void**[100];

	strcpy((char*)a[0], "123");
	((double*)a[1])[0] = 15;


	cout << (char*)a[0];
	cout << ((double*)a[1])[0]<<endl<<endl;
	*/


	/*
	MyPars M;


	M.add<char>("789", 4);

	char q[1] = { '1' };
	M.add<char>(q, 1);


	bool q2[1]={ 1 };
	M.add<bool>(q2, 1);
	M.see();
	*/

	/*
	char b[70000];
	M.addCharArr("имя", 3, "инфо тута", 9);
	M.addCharArr("имя", 3, "инфо тута", 9);


	M.addDouble("даб", 3, 7.5);
	cout<<M.see()<<endl;

	M._push<char, char>("имя", "инфо тута", 9);
	*/


	/*

	cout<<seeCat("d:\\");
	//system("pause");


	try
	{

		boost::asio::io_service io_service;

		using namespace std; // For atoi.
		server s(io_service, atoi("8001"));

		io_service.run();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	*/

	system("pause");

	return 0;
}

/*
void main() 
{

	
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода
	locale::global(std::locale("rus_rus.1251"));

	cout << seeCat("d:\\")<<endl;
	

	handle_connect
		ions();



	system("pause");
}

*/