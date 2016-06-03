# corteli

		int ep = bigBuff.size() - tmpLen;

		cout << bigBuff.size() << ' ' << tmpLen <<' '<< size << endl;

		
		if (ep < size)
		{
			memcpy(&bigBuff[tmpLen], &buff[0], ep);



			int bigBuffLenLen = dynamicInt.parse(&buff[ep], 1000);
			//cout <<"epp== "<< ep << ' ' << bigBuffLenLen << endl;


			if (bigBuffLenLen != -1)
			{
				bigBuff.clear();
				bigBuff.shrink_to_fit();

				bigBuff.resize(dynamicInt.get());

				ep += bigBuffLenLen;

				//cout << "epp77== " << ep<<' '<< bigBuffLenLen<<' '<< dynamicInt.get() <<endl;

				memcpy(&bigBuff[0], &buff[ep], size - ep);
				tmpLen = size - ep;
			}
		}
		else 
		{
			memcpy(&bigBuff[tmpLen], &buff[0], size);
			tmpLen += size;
		}

























	void BUFFERING(char* buff, int size)
	{
		memcpy(&b2[ptr], buff, size);
		q++;
		ptr += size;

		
		if (q > 2)
		{
			unsigned long long mesId = BaseClient::sendAsunc(b2, ptr);
			cout << "------" << ptr << ' '<< mesId << endl;

			q = 0;
			ptr = 0;
		}

	}
