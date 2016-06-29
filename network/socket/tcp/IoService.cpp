#include "IoService.h"

IoService::IoService(bool started)
{
	if (started)
	{
		startT();
	}
}

IoService::~IoService()
{
	stop();
}

void IoService::start()
{
	if (_status == status::STOPPED)
	{
		_status = status::RUN;

		while (_status != status::STOP)
		{
			_ioService.run();
			_ioService.reset();
			Sleep(10);
		}
		_status = status::STOPPED;
	}
}

void IoService::startT()
{
	if (_status == status::STOPPED)
	{
		std::thread(&IoService::start, this).detach();
	}
}

boost::asio::io_service& IoService::getIoSevice()
{
	return _ioService;
}

void IoService::stop()
{
	if (_status == status::RUN)
	{
		_status = status::STOP;
		_ioService.stop();
		while (_status != status::STOPPED)
		{
			Sleep(1);
		}
	}
}

IoService::status IoService::getStatus()
{
	return _status;
}
