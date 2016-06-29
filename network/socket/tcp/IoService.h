#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <thread>

#include "D:/Develop/VisualStudio15Project/corteli/Base/BaseObject/BaseObject/BaseObject.h"
#pragma comment(lib, "D:/Develop/VisualStudio15Project/corteli/Base/BaseObject/Release/BaseObject.lib")


class IoService
{
public:
	enum status
	{
		STOPPED = 0, RUN = 1, STOP = 2
	};


	IoService(bool started = false);
	~IoService();
	void start();
	void startT();
	boost::asio::io_service& getIoSevice();
	void stop();
	status getStatus();

private:
	status _status = status::STOPPED;
	boost::asio::io_service _ioService;
};