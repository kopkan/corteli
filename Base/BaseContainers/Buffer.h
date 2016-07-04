#pragma once


namespace corteli
{
	namespace base
	{
		namespace container
		{

			template <typename T>
			class Buffer
			{
			public:
				Buffer(size_t size = 0)
				{
					if (size != 0)
					{
						_size = size;
						_buffer = new T[_size];
					}
				}

				Buffer(Buffer& buffer)
				{
					if (buffer.getSize() > 0)
					{
						_size = buffer.getSize();
						_buffer = new T[_size];
						memcpy(_buffer, buffer.getBuff(), _size);
					}
				}

				~Buffer()
				{
					remove();
				}


				void remove()
				{
					if (_buffer)
					{
						delete _buffer;
						_buffer = NULL;
						_size = 0;
					}
				}

				void resize(size_t size)
				{
					remove();
					_size = size;
					_buffer = new T[_size];
				}

				T* getBuff()
				{
					return _buffer;
				}

				size_t getSize()
				{
					return _size;
				}

			private:
				T* _buffer = NULL;
				size_t _size = 0;
			};

		}
	}
}
