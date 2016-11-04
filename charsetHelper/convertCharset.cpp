#define USING_STATIC_LIBICONV
#include <stdio.h>
#include <iconv/iconv.h>
#include <errno.h>

#pragma comment(lib, "iconv//libiconvStatic")

class CharsetConverter
{
public:
	CharsetConverter(string fromCharsetName, string toCharsetName)
	{
		fromCharsetName_ = fromCharsetName;
		toCharsetName_ = toCharsetName;
	}

	void write(string buff)
	{
		inBuffer.insert(inBuffer.end(), buff.begin(), buff.end());
		size_t nonConvertSize = convertCharset_(inBuffer, (char*)fromCharsetName_.c_str(), (char*)toCharsetName_.c_str());
		inBuffer.erase(inBuffer.begin(), inBuffer.end() - nonConvertSize);
	}

	size_t read(string& buffer, size_t maxSize)
	{
		if (outBuffer.size() < maxSize)
		{
			maxSize = outBuffer.size();
		}
		buffer.append(outBuffer.data(), maxSize);
		outBuffer.erase(outBuffer.begin(), outBuffer.begin()+ maxSize);

		return maxSize;
	}

private:
	int convertCharset_(vector<char>& instr, char* from, char* to)
	{
		libiconv_t cd;

		cd = libiconv_open(to, from);
		if (cd == (libiconv_t)(-1))
		{
			throw std::runtime_error("convertCharset: Could not open handle iconv");
		}

		size_t inSize = instr.size();
		const char* in = instr.data();


		vector<char> outBuf(1000);
		char* out;
		size_t outSize;

		bool badWord = false;
		while (inSize > 0 && badWord == false)
		{
			errno = 0;
			
			out = outBuf.data();
			outSize = outBuf.size();

			size_t k = libiconv(cd, &in, &inSize, &out, &outSize);
			if (k == (size_t)-1)
			{
				switch (errno)
				{
				case EINVAL:
					badWord = true;//cannot convert all data, not the full character
					break;
				case E2BIG:
					//out buff small
					break;
				case NULL:
					break;
				default:
					throw std::runtime_error(string("convertCharset: error:") + to_string(errno) + " in byte №" + to_string(instr.size()- inSize));
				}
			}

			int parsedBytes = outBuf.size() - outSize;
			outBuffer.insert(outBuffer.end(), outBuf.begin(), outBuf.begin() + parsedBytes);
		}

		if (iconv_close(cd) != 0)
			std::runtime_error("convertCharset: Could not close handle iconv");

		return inSize;
	}

	string fromCharsetName_;
	string toCharsetName_;

	vector<char> inBuffer;
	vector<char> outBuffer;
};












int convertCharset(string inStr, string& outStr, char* fromCharset, char* toCharset, bool clearOutStr = true)
{
	libiconv_t cd;

	cd = libiconv_open(toCharset, fromCharset);
	if (cd == (libiconv_t)(-1))
	{
		throw std::runtime_error("convertCharset: Could not open handle iconv");
	}

	size_t inSize = inStr.size();
	const char* in = inStr.data();


	vector<char> outBuf(1000);
	char* out;
	size_t outSize;

	bool badWord = false;
	while (inSize > 0 && badWord == false)
	{
		errno = 0;

		out = outBuf.data();
		outSize = outBuf.size();

		size_t k = libiconv(cd, &in, &inSize, &out, &outSize);
		if (k == (size_t)-1)
		{
			switch (errno)
			{
			case EINVAL:
				badWord = true;//cannot convert all data, not the full character
				break;
			case E2BIG:
				//out buff small
				break;
			case NULL:
				break;
			default:
				throw std::runtime_error(string("convertCharset: error:") + to_string(errno) + " in byte №" + to_string(inStr.size() - inSize));
			}
		}

		if (clearOutStr) { outStr.clear(); }

		int parsedBytes = outBuf.size() - outSize;
		outStr.append(outBuf.begin(), outBuf.begin() + parsedBytes);
	}

	if (iconv_close(cd) != 0)
		std::runtime_error("convertCharset: Could not close handle iconv");

	return inSize;
}
