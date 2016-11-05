#include <openssl/openssl.h>

streampos getIFstreamSize(ifstream& ifstream)
{
	streampos fpoz = ifstream.tellg();

	ifstream.seekg(0, ifstream.end);
	streampos fsize = ifstream.tellg();
	ifstream.seekg(fpoz, ifstream.beg);

	return fsize;
}


bool SHA256_File(wstring filename, unsigned char* hashBuff)
{
	ifstream f(filename, ios::binary);
	if (!f) { return false; }
	size_t rawSize = getIFstreamSize(f);

	const int maxBlock = 1000;
	unsigned char buff[maxBlock];

	SHA256_CTX context;
	if (!SHA256_Init(&context))
	{
		return false;
	}

	while (rawSize != 0)
	{
		int readSize;
		if (rawSize > maxBlock) { readSize = maxBlock; }
		else { readSize = rawSize; }

		f.read((char*)buff, readSize);
		if (!f) { return false; }

		if (!SHA256_Update(&context, buff, readSize))
		{
			return false;
		}

		rawSize -= readSize;
	}

	if (!SHA256_Final(hashBuff, &context))
	{
		return false;
	}
	return true;
}
