#pragma once


#include "BaseClient.h"
#include "IoService.h"


template <typename T>
class BasePool
{
public:
	enum status
	{
		STOPPED = 0, RUN = 1, STOP = 2
	};


	BasePool() : _ioService(true)
	{

	}

	~BasePool()
	{
		_status = status::STOP;
		while (_status != status::STOPPED)
		{
			Sleep(1);
		}


		for (std::list<T*>::iterator it = _clientList.begin(); it != _clientList.end(); ++it)
		{
			remove((*it));
		}
	}

	T* newClient()
	{
		T* client = new T(_ioService.getIoSevice());
		_clientList.push_back(client);
		return client;
	}

	void runExpansionT()
	{
		if (_status != status::RUN)
		{
			std::thread(&BasePool::runExpansion, this).detach();
		}
	}

	void runExpansion()
	{

		if (_status != status::RUN)
		{
			while (_status != status::STOP)
			{

				for (std::list<T*>::iterator it = _clientList.begin(); it != _clientList.end(); ++it)
				{
					(*it)->runExpansion();
				}
				Sleep(10);
			}
			_status = status::STOPPED;
		}

	}

	void remove(T* client)
	{
		_clientList.remove(client);
		delete client;
	}

protected:
	IoService _ioService;
	std::list<T*> _clientList;


private:
	status _status = status::STOPPED;
};