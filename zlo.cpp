// Пример простого TCP клиента
#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <windows.h>


#define PORT 1500
#define SERVERADDR "127.0.0.1"

int main(int argc, char* argv[])
{
	char buff[1024];
	printf("TCP DEMO CLIENT\n");

	// Шаг 1 - инициализация библиотеки Winsock
	if (WSAStartup(0x202, (WSADATA *)&buff[0]))
	{
		printf("WSAStart error %d\n", WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET* my_sock=new SOCKET;
	*my_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (*my_sock < 0)
	{
		printf("Socket() error %d\n", WSAGetLastError());
		return -1;
	}

	//////////////////////////////
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1055);/////////////////
	// не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0;
	// сервер принимает подключения
	// на все IP-адреса

	// вызываем bind для связывания
	if (::bind(*my_sock, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		// Ошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(*my_sock);  // закрываем сокет!
		WSACleanup();
		//return -1;
	}

	// Шаг 3 - установка соединения

	// заполнение структуры sockaddr_in
	// указание адреса и порта сервера
	sockaddr_in dest_addr;
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(PORT);
	HOSTENT *hst;

	// преобразование IP адреса из символьного в
	// сетевой формат
	if (inet_addr(SERVERADDR) != INADDR_NONE)
		dest_addr.sin_addr.s_addr = inet_addr(SERVERADDR);
	else
		// попытка получить IP адрес по доменному
		// имени сервера
		if (hst = gethostbyname(SERVERADDR))
			// hst->h_addr_list содержит не массив адресов,
			// а массив указателей на адреса
			((unsigned long *)&dest_addr.sin_addr)[0] =
			((unsigned long **)hst->h_addr_list)[0][0];
		else
		{
			printf("Invalid address %s\n", SERVERADDR);
			closesocket(*my_sock);
			WSACleanup();
			//return -1;
		}

	// адрес сервера получен – пытаемся установить
	// соединение 
	if (connect(*my_sock, (sockaddr *)&dest_addr,
		sizeof(dest_addr)))
	{
		printf("Connect error %d\n", WSAGetLastError());
		//return -1;
	}

	printf("Соединение с %s успешно установлено\n\
    Type quit for quit\n\n", SERVERADDR);

	printf("Recv error %d\n", WSAGetLastError());

	Sleep(1000);
	closesocket(*my_sock);
	//WSACleanup();

	//shutdown(*my_sock, 2);
	//Sleep(1000);
	//closesocket(*my_sock);

	//CloseHandle((HANDLE)*my_sock);

	system("pause");
	return -1;
}











///

// Пример простого TCP – эхо сервера

#include <stdio.h>
#include <winsock2.h>  // Wincosk2.h должен быть 
// подключен раньше windows.h!
#include <windows.h>

#define MY_PORT    1500
// Порт, который слушает сервер

// макрос для печати количества активных
// пользователей 
#define PRINTNUSERS if (nclients)\
  printf("%d user on-line\n",nclients);\
  else printf("No User on line\n");

// прототип функции, обслуживающий
// подключившихся пользователей
DWORD WINAPI SexToClient(LPVOID client_socket);

// глобальная переменная – количество
// активных пользователей 
int nclients = 0;

int main(int argc, char* argv[])
{
	char buff[1024];    // Буфер для различных нужд

	printf("TCP SERVER DEMO\n");

	// Шаг 1 - Инициализация Библиотеки Сокетов
	// Т.к. возвращенная функцией информация
	// не используется ей передается указатель на
	// рабочий буфер, преобразуемый
	// к указателю  на структуру WSADATA.
	// Такой прием позволяет сэкономить одну
	// переменную, однако, буфер должен быть не менее
	// полкилобайта размером (структура WSADATA
	// занимает 400 байт)
	if (WSAStartup(0x0202, (WSADATA *)&buff[0]))
	{
		// Ошибка!
		printf("Error WSAStartup %d\n",
			WSAGetLastError());
		return -1;
	}

	// Шаг 2 - создание сокета
	SOCKET mysocket;
	// AF_INET     - сокет Интернета
	// SOCK_STREAM  - потоковый сокет (с
	//      установкой соединения)
	// 0      - по умолчанию выбирается TCP протокол
	if ((mysocket = socket(AF_INET, SOCK_STREAM, 0))<0)
	{
		// Ошибка!
		printf("Error socket %d\n", WSAGetLastError());
		WSACleanup();
		// Деиницилизация библиотеки Winsock
		return -1;
	}

	// Шаг 3 связывание сокета с локальным адресом
	sockaddr_in local_addr;
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(MY_PORT);
	// не забываем о сетевом порядке!!!
	local_addr.sin_addr.s_addr = 0;
	// сервер принимает подключения
	// на все IP-адреса

	// вызываем bind для связывания
	if (::bind(mysocket, (sockaddr *)&local_addr,
		sizeof(local_addr)))
	{
		// Ошибка
		printf("Error bind %d\n", WSAGetLastError());
		closesocket(mysocket);  // закрываем сокет!
		WSACleanup();
		return -1;
	}

	// Шаг 4 ожидание подключений
	// размер очереди – 0x100
	if (listen(mysocket, 0x100))
	{
		// Ошибка
		printf("Error listen %d\n", WSAGetLastError());
		closesocket(mysocket);
		WSACleanup();
		return -1;
	}

	printf("Ожидание подключений\n");

	// Шаг 5 извлекаем сообщение из очереди
	SOCKET client_socket;    // сокет для клиента
	sockaddr_in client_addr;    // адрес клиента
								// (заполняется системой)

								// функции accept необходимо передать размер
								// структуры
	int client_addr_size = sizeof(client_addr);

	// цикл извлечения запросов на подключение из
	// очереди
	while ((client_socket = accept(mysocket, (sockaddr *)
		&client_addr, &client_addr_size)))
	{
		nclients++;      // увеличиваем счетчик
						 // подключившихся клиентов

						 // пытаемся получить имя хоста
		HOSTENT *hst;
		hst = ::gethostbyaddr((char *)
			&client_addr.sin_addr.s_addr, 4, AF_INET);


		cout << "new" << ntohs(client_addr.sin_port) << endl;
		// вывод сведений о клиенте
		//PRINTNUSERS

			// Вызов нового потока для обслужвания клиента
			// Да, для этого рекомендуется использовать
			// _beginthreadex но, поскольку никаких вызов
			// функций стандартной Си библиотеки поток не
			// делает, можно обойтись и CreateThread
			DWORD thID;
		CreateThread(NULL, NULL, SexToClient,
			&client_socket, NULL, &thID);
	}
	return 0;
}

// Эта функция создается в отдельном потоке и
// обсуживает очередного подключившегося клиента
// независимо от остальных
DWORD WINAPI SexToClient(LPVOID client_socket)
{
	SOCKET my_sock;
	my_sock = ((SOCKET *)client_socket)[0];

	
	char buff[20 * 1024];
	
    #define sHELLO "Hello, Sailor\r\n"

	// отправляем клиенту приветствие 
	//send(my_sock, sHELLO, sizeof(sHELLO), 0);

	// цикл эхо-сервера: прием строки от клиента и
	// возвращение ее клиенту

	int R = recv(my_sock, &buff[0], sizeof(buff), 0);
	while (R > 0)
	{
		R = recv(my_sock, &buff[0], sizeof(buff), 0);
		send(my_sock, &buff[0], 10, 0);
	}
	// если мы здесь, то произошел выход из цикла по
	// причине возращения функцией recv ошибки –
	// соединение клиентом разорвано
	nclients--; // уменьшаем счетчик активных клиентов
	
	cout << "err=" << R << " "<< GetLastError() << endl;
	
	
		// закрываем сокет
		closesocket(my_sock);
	return 0;
}
