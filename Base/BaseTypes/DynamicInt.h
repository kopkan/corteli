#pragma once

#include <stdlib.h>
#include <cstring>

#include <vector>


#define Int64 long long
#define UInt64 unsigned long long
#define BYTE unsigned char

namespace corteli
{
	namespace base
	{
		namespace type
		{

			class DynamicInt
			{
			public:
				void set(Int64 val);
				BYTE* setAndToByteArray(Int64 val, int *byteArrayLen);
				char* setAndToCharArray(Int64 val, int *byteArrayLen);
				size_t parse(BYTE *byteArray, int byteArrayLen);
				size_t parse(char *byteArray, int byteArrayLen);
				Int64 get(Int64 errorVal = -1);
				BYTE* toByteArray(int* len = NULL);
				char* toCharArray(int* len = NULL);
				size_t getSize();
				bool isValueReady();
				void clear();


			protected:
				UInt64 pow(UInt64 val, UInt64 order);

			private:
				std::vector<BYTE> _val;
				bool valueReady = true;

			};
		}
	}
}

#undef Int64
#undef UInt64
#undef BYTE
