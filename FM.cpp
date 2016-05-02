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




class MyPars
{
public:

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

			memcpy((void*)&buff[ptr], (void*)name, nameSize);
			
		}

		ptr += 1;


		memcpy((void*)&buff[ptr], (void*)data, sizeof(I)*dataSize);
		ptr += sizeof(I)*dataSize;
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



};



int main(int argc, char* argv[])
{
	SetConsoleCP(1251);// установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода



	MyPars M;

	char b[70000];
	M.addCharArr("имя", 3, "инфо тута", 9);
	M.addCharArr("имя", 3, "инфо тута", 9);


	M.addDouble("даб", 3, 7.5);
	cout<<M.see()<<endl;

	M._push<char, char>("имя", "инфо тута", 9);



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