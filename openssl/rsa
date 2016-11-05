#pragma warning(disable:4996)

#include <stdio.h>
#include <string.h>
#include <openssl/openssl.h>

#include <Windows.h>


#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#include <time.h>



streampos getIFstreamSize(ifstream& ifstream)
{
	streampos fpoz = ifstream.tellg();

	ifstream.seekg(0, ifstream.end);
	streampos fsize = ifstream.tellg();
	ifstream.seekg(fpoz, ifstream.beg);

	return fsize;
}

bool readFile(wstring fpath, string& data)
{
	ifstream f(fpath, ios::binary);
	if (!f) { return false; }

	streampos fsize = getIFstreamSize(f);

	data.resize(fsize);


	f.read((char*)data.data(), fsize);
	if (!f) { return false; }
	else { return true; }
}

bool writeFile(wstring fpath, string data)
{
	ofstream f(fpath, ios::binary);
	if (!f) { return false; }

	f.write((char*)data.data(), data.size());
	if (!f) { return false; }
	else { return true; }
}


bool GenKeys(size_t keySize, string& publicKey, string& privateKey) 
{
	RSA* rsa = RSA_generate_key(keySize, RSA_F4, NULL, NULL);
	if (rsa == NULL) { return false; }

	{
		BIO *bio = BIO_new(BIO_s_mem());
		PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, NULL, NULL, NULL);

		int keylen = BIO_pending(bio);
		char* pem_key = (char*)calloc(keylen + 1, 1);
		BIO_read(bio, pem_key, keylen);
		privateKey = pem_key;
		BIO_free_all(bio);
		free(pem_key);
	}

	{
		BIO *bio = BIO_new(BIO_s_mem());
		PEM_write_bio_RSAPublicKey(bio, rsa);
		int keylen = BIO_pending(bio);
		char* pem_key = (char*)calloc(keylen + 1, 1);
		BIO_read(bio, pem_key, keylen);
		publicKey = pem_key;
		BIO_free_all(bio);
		free(pem_key);
	}
	RSA_free(rsa);
	return true;
}



RSA* getRSA(string key, bool isPublicKey)
{
	RSA *rsa = NULL;
	BIO *keybio;
	keybio = BIO_new_mem_buf((unsigned char*)key.data(), key.size());

	if (keybio == NULL)
	{
		printf("Failed to create key BIO");
		return NULL;
	}
	if (isPublicKey)
	{
		rsa = PEM_read_bio_RSAPublicKey(keybio, &rsa, NULL, NULL);
	}
	else
	{
		rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
	}
	return rsa;
}

bool encrypt(string keyData, bool isPublicKey, string inData, string& outData)
{
	RSA* Key = getRSA(keyData, isPublicKey);
	int key_size = RSA_size(Key);

	unsigned char ctext[5000];
	int maxBlock = key_size - 11;
	for (int i = 0; i < inData.size();)
	{
		int write = maxBlock;
		if (inData.size() - i < maxBlock)
		{
			write = inData.size() - i;
		}


		int outlen = 0;
		if (isPublicKey){
			outlen = RSA_public_encrypt(write, (byte*)(&inData.data()[i]), ctext, Key, RSA_PKCS1_PADDING);
		}
		else{
			outlen = RSA_private_encrypt(write, (byte*)(&inData.data()[i]), ctext, Key, RSA_PKCS1_PADDING);
		}

		outData.append((char*)ctext, outlen);

		i += write;
	}

	return true;
}

bool decrypt(string keyData, bool isPublicKey, string inData, string& outData)
{
	RSA* Key = getRSA(keyData, isPublicKey);
	int key_size = RSA_size(Key);

	unsigned char ptext[1000];
	for (int i=0; i<inData.size(); )
	{
		int outlen = 0;
		if (isPublicKey) {
			outlen = RSA_public_decrypt(key_size, (byte*)(&inData.data()[i]), ptext, Key, RSA_PKCS1_PADDING);
		}
		else {
			outlen = RSA_private_decrypt(key_size, (byte*)(&inData.data()[i]), ptext, Key, RSA_PKCS1_PADDING);
		}

		outData.append((char*)ptext, outlen);
		i += key_size;
	}

	return true;
}


bool publicEncrypt(string keyData, string inData, string& outData)
{
	return encrypt(keyData, true, inData, outData);
}

bool privateEncrypt(string keyData, string inData, string& outData)
{
	return encrypt(keyData, false, inData, outData);
}



bool publicDecrypt(string keyData, string inData, string& outData)
{
	return decrypt(keyData, true, inData, outData);
}

bool privateDecrypt(string keyData, string inData, string& outData)
{
	return decrypt(keyData, false, inData, outData);
}


void main()
{
	setlocale(LC_ALL, "Russian");


	string pubKeyData;
	string privKeyData;


	int t1 = clock();
	{
		cout << GenKeys(2048, pubKeyData, privKeyData) << endl;

		cout << pubKeyData << endl;
		cout << privKeyData << endl;
		writeFile(L"e://pub.key", pubKeyData);
		writeFile(L"e://priv.key", privKeyData);
	}


	int t2 = clock();

	string codeData;
	{
		string Data;
		if (!readFile(L"e://123.rar", Data)) { return; }
		cout << privateEncrypt(privKeyData, Data, codeData) << endl;
		writeFile(L"e://123.rar.rsa", codeData);
	}

	int t3 = clock();


	{
		string myData;
		cout << publicDecrypt(pubKeyData, codeData, myData) << endl;
		writeFile(L"e://123.rar.rar", myData);
	}

	int t4 = clock();

	cout << t2 - t1 << ' ' << t3 - t2 << ' ' << t4 - t3 << endl;

	system("pause");
}
