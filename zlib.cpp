
#include <vector>
#include <assert.h>

#include <functional>

class ProgressListener
{
public:
	ProgressListener(std::function<void(int)> progressFunc)
	{
		_progressFunc = progressFunc;
	}

	void setProgress(int progress)
	{
		if (progress >= 0 && progress <= 100)
		{
			_progressFunc(progress);
		}
	}

private:
	std::function<void(int)> _progressFunc;
};


int compress_memory(void *in_data, size_t in_data_size, std::vector<uint8_t> &out_data, int compressLevel=Z_DEFAULT_COMPRESSION, ProgressListener* progressListener = NULL, int strategy= Z_DEFAULT_STRATEGY)
{
	const size_t BUFSIZE = 128 * 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = reinterpret_cast<uint8_t *>(in_data);
	strm.avail_in = in_data_size;
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	deflateInit2(&strm, compressLevel, Z_DEFLATED,
		15 | 16,
		9,
		strategy);

	if (progressListener != NULL) { progressListener->setProgress(0); }

	int res = Z_OK;
	while (res == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			out_data.insert(out_data.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;


			int progress = (float)(in_data_size - strm.avail_in) / in_data_size * 100;
			if (progressListener != NULL) { progressListener->setProgress(progress); }
		}
		res = deflate(&strm, Z_FINISH);
	}
	out_data.insert(out_data.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);

	if (progressListener != NULL) { progressListener->setProgress(100); }

	if (res != Z_STREAM_END) { return res; }
	else { return Z_OK; }
}


int decompress_memory(void *in_data, size_t in_data_size, std::vector<uint8_t> &out_data, ProgressListener* progressListener = NULL)
{
	const size_t BUFSIZE = 128 * 1024;
	uint8_t temp_buffer[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = reinterpret_cast<uint8_t *>(in_data);
	strm.avail_in = in_data_size;
	strm.next_out = temp_buffer;
	strm.avail_out = BUFSIZE;

	inflateInit2(&strm, 15 | 16);

	if (progressListener != NULL) { progressListener->setProgress(0); }

	int res = Z_OK;
	while (res == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			out_data.insert(out_data.end(), temp_buffer, temp_buffer + BUFSIZE);
			strm.next_out = temp_buffer;
			strm.avail_out = BUFSIZE;


			int progress = (float)(in_data_size - strm.avail_in) / in_data_size * 100;
			if (progressListener != NULL) { progressListener->setProgress(progress); }
		}
		res = inflate(&strm, Z_NO_FLUSH);
	}
	out_data.insert(out_data.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
	inflateEnd(&strm);

	if (progressListener != NULL) { progressListener->setProgress(100); }

	if (res != Z_STREAM_END){ return res; }
	else{ return Z_OK; }
}






class CompressStream
{	
public:
	CompressStream(int compressLevel = Z_DEFAULT_COMPRESSION)
	{
		_strm.zalloc = 0;
		_strm.zfree = 0;
		_strm.next_in = reinterpret_cast<uint8_t *>(_in.data());
		_strm.avail_in = _in.size();
		_strm.next_out = _temp_buffer;
		_strm.avail_out = _BUFSIZE;

		deflateInit(&_strm, compressLevel);
	}

	~CompressStream()
	{
		deflateEnd(&_strm);
	}


	int write(uint8_t* data, size_t len, ProgressListener* progressListener = NULL)
	{
		_in.insert(_in.end(), data, data + len);
		return _compress(progressListener);
	}

	void read(uint8_t* data, size_t* readLen, size_t maxLen)
	{
		if(_out.size() < maxLen){ maxLen = _out.size(); }

		memcpy(data, _out.data(), maxLen);
		_out.erase(_out.begin(), _out.begin()+ maxLen);
		*readLen = maxLen;
	}

private:
	std::vector<uint8_t> _in;
	std::vector<uint8_t> _out;

	static const size_t _BUFSIZE = 128 * 1024;
	uint8_t _temp_buffer[_BUFSIZE];
	z_stream _strm;



	int _compress(ProgressListener* progressListener = NULL)
	{
		if (progressListener != NULL) { progressListener->setProgress(0); }

		_strm.next_in = reinterpret_cast<uint8_t *>(_in.data());
		_strm.avail_in = _in.size();

		int res = Z_OK;
		while (res == Z_OK)
		{
			//cout << _strm.avail_in << ' ' << _strm.avail_out << endl;

			if (_strm.avail_out == 0)
			{
				_out.insert(_out.end(), _temp_buffer, _temp_buffer + _BUFSIZE);
				_strm.next_out = _temp_buffer;
				_strm.avail_out = _BUFSIZE;


				int progress = (float)(_in.size() - _strm.avail_in) / _in.size() * 100;
				if (progressListener != NULL) { progressListener->setProgress(progress); }
			}
			res = deflate(&_strm, Z_FINISH);
		}
		_out.insert(_out.end(), _temp_buffer, _temp_buffer + _BUFSIZE - _strm.avail_out);


		if (progressListener != NULL) { progressListener->setProgress(100); }

		_in.clear();

		if (res != Z_STREAM_END) { return res; }
		else { return Z_OK; }
	}
};



class DecompressStream
{
public:
	DecompressStream()
	{
		_strm.zalloc = 0;
		_strm.zfree = 0;
		_strm.next_in = _in.data();
		_strm.avail_in = _in.size();
		_strm.next_out = _temp_buffer;
		_strm.avail_out = _BUFSIZE;

		inflateInit(&_strm);
	}

	~DecompressStream()
	{
		inflateEnd(&_strm);
	}


	int write(uint8_t* data, size_t len, ProgressListener* progressListener = NULL)
	{
		_in.insert(_in.end(), data, data + len);
		return _decompress(progressListener);
	}

	void read(uint8_t* data, size_t* readLen, size_t maxLen)
	{
		if (_out.size() < maxLen) { maxLen = _out.size(); }

		memcpy(data, _out.data(), maxLen);
		_out.erase(_out.begin(), _out.begin() + maxLen);
		*readLen = maxLen;
	}

private:
	std::vector<uint8_t> _in;
	std::vector<uint8_t> _out;

	static const size_t _BUFSIZE = 128 * 1024;
	uint8_t _temp_buffer[_BUFSIZE];
	z_stream _strm;


	int _decompress(ProgressListener* progressListener = NULL)
	{
		if (progressListener != NULL) { progressListener->setProgress(0); }

		_strm.next_in = reinterpret_cast<uint8_t *>(_in.data());
		_strm.avail_in = _in.size();

		int res = Z_OK;
		while (res == Z_OK)
		{
			if (_strm.avail_out == 0)
			{
				_out.insert(_out.end(), _temp_buffer, _temp_buffer + _BUFSIZE);
				_strm.next_out = _in.data();
				_strm.avail_out = _in.size();


				int progress = (float)(_in.size() - _strm.avail_in) / _in.size() * 100;
				if (progressListener != NULL) { progressListener->setProgress(progress); }
			}
			res = inflate(&_strm, Z_NO_FLUSH);
		}
		_out.insert(_out.end(), _temp_buffer, _temp_buffer + _BUFSIZE - _strm.avail_out);

		if (progressListener != NULL) { progressListener->setProgress(100); }

		if (res != Z_STREAM_END) { return res; }
		else { return Z_OK; }
	}


};
