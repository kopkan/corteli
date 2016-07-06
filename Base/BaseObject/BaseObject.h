#pragma once

#include <iostream>
#include <vector>

namespace corteli
{
	namespace base
	{

		class BaseObject
		{
		public:
			BaseObject(bool enabledDebugStream = 0);

			std::string getLog(int num);
			std::string getLastLog();
			int getLogSize();
			void clearLog();
			bool enabledDebugStream = 0;

		protected:
			void log(std::string mesasage);

			template <typename T>
			void write(T buff, bool newStr = true)
			{
				if (enabledDebugStream)
				{
					std::cout << buff;
					if (newStr)
					{
						std::cout << std::endl;
					}
				}
			}

		private:
			std::vector<std::string> logVector;
		};
	}
}