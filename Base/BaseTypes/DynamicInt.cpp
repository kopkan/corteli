#include "DynamicInt.h"

#define Int64 long long
#define UInt64 unsigned long long
#define BYTE unsigned char

using namespace corteli::base::type;

void DynamicInt::set(Int64 val)
{
	clear();

	int i;
	for (i = 0; true; i++)
	{
		_val.push_back(val % 128);
		if (val = val / 128)
		{
			_val.at(i) += 128;//in the high byte will be 1 - means pars is not over
		}
		else
		{
			break;
		}
	}
	i++;
}

BYTE* DynamicInt::setAndToByteArray(Int64 val, int *byteArrayLen)
{
	set(val);
	return toByteArray(byteArrayLen);
}

char* DynamicInt::setAndToCharArray(Int64 val, int *byteArrayLen)
{
	set(val);
	return toCharArray(byteArrayLen);
}


size_t DynamicInt::parse(BYTE* byteArray, int byteArrayLen)
{
	if (valueReady)
	{
		_val.clear();
		valueReady = false;
	}

	for (int i = 0; i<byteArrayLen; i++)
	{
		_val.push_back(byteArray[i]);


		if ((byteArray[i] / 128) == 0)
		{
			valueReady = true;
			return _val.size();
		}
	}

	return -1;
}

size_t DynamicInt::parse(char *byteArray, int byteArrayLen)
{
	return parse((BYTE*)byteArray, byteArrayLen);
}


Int64 DynamicInt::get(Int64 errorVal)
{
	if (valueReady)
	{
		unsigned long long val = 0;
		for (int i = 0; i < _val.size(); i++)
		{
			val += (_val[i] % 128)*pow(128, i);
			if ((_val[i] / 128) == 0)
			{
				break;
			}
		}

		return val;
	}
	else
	{
		return errorVal;
	}
}

BYTE* DynamicInt::toByteArray(int* len)
{
	if (valueReady)
	{
		int size = _val.size();

		BYTE* strVal = new BYTE[size];

		for (int i = 0; i < _val.size(); i++)
		{
			strVal[i] = _val[i];
		}

		if (len != NULL)
		{
			*len = size;
		}

		return strVal;
	}
	else 
	{
		return NULL;
	}
}

char* DynamicInt::toCharArray(int* len)
{
	return (char*)toByteArray(len);
}


size_t DynamicInt::getSize()
{
	if (valueReady)
	{
		return _val.size();
	}
	else
	{
		return -1;
	}
}

bool DynamicInt::isValueReady()
{
	return valueReady;
}


void DynamicInt::clear()
{
	_val.clear();
	valueReady = true;
}

UInt64 DynamicInt::pow(UInt64 val, UInt64 order)
{
	UInt64 ret = 1;

	while (order--)
	{
		ret *= val;
	}

	return ret;
}