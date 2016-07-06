#pragma once


#include "BaseClient.h"

namespace corteli {
	namespace network {
		namespace pipe {

			template <typename T>
			class BasePool : public corteli::base::BaseObject
			{
			public:
				enum status
				{
					STOPPED = 0, RUN = 1, STOP = 2
				};


				BasePool(bool enableDebugMessage = true) : BaseObject(enableDebugMessage), _ioService(true)
				{

				}

				~BasePool()
				{
					stop();
				}

				T* newClient()
				{
					T* client = new T(&_ioService);
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
						_status = status::RUN;

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

				void stop()
				{
					if (_status == status::RUN)
					{
						_status = status::STOP;
						while (_status != status::STOPPED)
						{
							Sleep(1);
						}

						while (_clientList.size())
						{
							std::list<T*>::iterator it = _clientList.begin();
							remove((*it));
						}
					}
				}

			protected:
				corteli::network::IoService _ioService;
				std::list<T*> _clientList;


			private:
				status _status = status::STOPPED;
			};


		}
	}
}