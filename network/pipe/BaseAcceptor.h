#pragma once


#include "BasePool.h"

namespace corteli {
	namespace network {
		namespace pipe {

			template<typename T>
			class BaseAcceptor : protected BasePool<T>
			{
			public:
				enum status
				{
					NOT_INIT = 0, INITED = 1, ACCEPT_STARTED = 2, END_WORK = 3, CLOSED = 7
				};
				enum error
				{
					NO_ERR = 0, BIND_ERR = 1, LISTEN_ERR = 2, ACCEPT_ERR = 3, CLOSE = 100, CALL_DENIED = 200
				};

				BaseAcceptor(bool enableDebugMessage = true) : BasePool(enableDebugMessage), _server(_ioService.getIoSevice())
				{
					runExpansionT();
					_status = status::INITED;
				}

				~BaseAcceptor()
				{
					stop();
				}

				int startAccept(std::string pipename)
				{
					if (_status == status::INITED && !_error)
					{
						_status = status::ACCEPT_STARTED;


						BaseObject::write("BaseAcceptor STARTED");

						HANDLE hPipe = NULL;

						while (!_error)
						{
							hPipe = CreateNamedPipeA(
								pipename.c_str(),             // pipe name 
								PIPE_ACCESS_DUPLEX
								| FILE_FLAG_OVERLAPPED,       // read/write access 
								PIPE_TYPE_MESSAGE |       // message type pipe 
								PIPE_READMODE_MESSAGE |   // message-read mode 
								PIPE_WAIT,                // blocking mode      
														  //PIPE_NOWAIT,
								PIPE_UNLIMITED_INSTANCES, // max. instances  
								0,                  // output buffer size 
								0,                  // input buffer size 
								0,                        // client time-out 
								NULL);                    // default security attribute 

							if (hPipe == INVALID_HANDLE_VALUE)
							{
								DWORD err = GetLastError();
								bindError(err);
								continue;
							}
							else
							{
								OVERLAPPED ov = { 0 };
								ov.hEvent = ::CreateEventW(nullptr, TRUE, FALSE, nullptr);

								while (!_error)
								{
									BOOL retVal = ::ConnectNamedPipe(hPipe, &ov);

									if (retVal == 0)
									{
										DWORD err = ::GetLastError();

										if (err == ERROR_IO_PENDING)
										{
											::WaitForSingleObject(ov.hEvent, 100);
											continue;
										}
										else
										{
											if (err == ERROR_PIPE_CONNECTED || !err)
											{
												T* client = BasePool::newClient();
												client->_acceptorSetPipeHandle(hPipe);
												accepted(client);
												hPipe = NULL;
												break;
											}
											else
											{
												CloseHandle(hPipe);
												acceptError(err);
												break;
											}
										}
									}
								}

							}
						}

						if (hPipe != NULL)
						{
							CloseHandle(hPipe);
						}
					}
					else
					{
						return error::CALL_DENIED;
					}
				}


				int startAcceptT(std::string pipename)
				{
					if (_status == status::INITED && !_error)
					{
						std::thread(&BaseAcceptor::startAccept, this, pipename).detach();
						return error::NO_ERR;
					}
					else
					{
						return error::CALL_DENIED;
					}
				}

				int close()
				{
					if (_error != error::CLOSE)
					{
						_setError(error::CLOSE);
						_server.close();

						if (_status >= status::ACCEPT_STARTED)
						{
							while (_status < status::END_WORK && _ioService.getStatus() != IoService::status::STOPPED)
							{
								Sleep(1);
							}
						}


						BasePool::stop();
						cout << _clientList.size() << endl;
						return error::NO_ERR;
					}
					else
					{
						return error::CALL_DENIED;
					}
				}

				void reload()
				{
					close();
					_error = error::NO_ERR;
					_status = status::INITED;
				}

			protected:
				virtual void accepted(T* pipe)
				{
					pipe->startRecv(500);

					BaseObject::write("BaseAcceptor accepted");
				}

				virtual void bindError(DWORD ec)
				{
					BaseObject::write("bindError");

					switch (ec)
					{
					case ERROR_PIPE_BUSY://All pipe instances are busy.
						BaseObject::write("All pipe instances are busy");
						Sleep(100);
						break;
					case ERROR_INVALID_PARAMETER:
						BaseObject::write("bad attrb");
						return;
					default:
						BaseObject::write("err create pipe=", 0);
						BaseObject::write(ec);
						return;
					}
				}

				virtual void acceptError(DWORD ec)
				{
					BaseObject::write("acceptError=", 0);
					BaseObject::write(ec);
				}

			private:
				void _setError(error err)
				{
					if (_error != error::CLOSE)
					{
						_error = err;
					}
				}

				boost::asio::ip::tcp::acceptor _server;
				status _status = status::NOT_INIT;
				error _error = error::NO_ERR;
			};

		}
	}
}