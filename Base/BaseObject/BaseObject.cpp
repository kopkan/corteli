#include "BaseObject.h"


namespace corteli
{
	namespace base
	{

		BaseObject::BaseObject(bool enabledDebugStream)
		{
			this->enabledDebugStream = enabledDebugStream;
		}

		std::string BaseObject::getLog(int num)
		{
			if (num >= logVector.size())
			{
				return "error log num";
			}
			else
			{
				return logVector.at(num);
			}
		}

		std::string BaseObject::getLastLog()
		{
			return getLog(logVector.size() - 1);
		}

		int BaseObject::getLogSize()
		{
			return logVector.size();
		}

		void BaseObject::clearLog()
		{
			logVector.clear();
		}

		void BaseObject::log(std::string mesasage)
		{
			logVector.push_back(mesasage);
			write("new log=", 0);
			write(mesasage.c_str(), 1);
		}

	}
}