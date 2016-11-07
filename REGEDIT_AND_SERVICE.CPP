class MutexSecurity
{
public:
	MutexSecurity(char* name)
	{
		_hMutex = CreateMutex(
			NULL,
			FALSE,
			name);

		if (_hMutex == NULL || GetLastError() != NULL)
		{
			CloseHandle(_hMutex);
			_hMutex = NULL;
		}
	}

	bool isTake()
	{
		return _hMutex != NULL;
	}

	~MutexSecurity()
	{
		if (_hMutex)
		{
			CloseHandle(_hMutex);
		}
	}

private:
	HANDLE _hMutex=NULL;
};

class RegEdit 
{
public:
	RegEdit(HKEY hInit)
	{
		hInit_ = hInit;
	}

	bool write(string path, string name, string val)
	{
		HKEY h = hInit_;
		int result = RegCreateKeyEx(h, path.data(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &h, NULL);
		if (result == ERROR_SUCCESS)
		{
			result = RegSetValueExA(h, name.data(), 0, REG_SZ, (byte*)val.data(), val.length());
			RegCloseKey(h);
			return true;
		}
		return false;
	}

	bool read(string path, string name, string& value)
	{
		HKEY h = hInit_;

		if (RegOpenKeyExA(h, path.data(), 0, KEY_QUERY_VALUE, &h) == ERROR_SUCCESS)
		{
			DWORD type;
			DWORD len;
			if (RegQueryValueExA(h, name.data(), NULL, &type, 0, &len) == ERROR_SUCCESS)
			{
				if (type == REG_SZ) 
				{
					value.resize(len);
					if (RegQueryValueExA(h, name.data(), NULL, &type, (byte*)value.c_str(), &len) == ERROR_SUCCESS)
					{
						RegCloseKey(h);
						return true;
					}
				}
			}
		}

		RegCloseKey(h);
		return false;
	}

	bool remove(string path, string name)
	{
		HKEY h = hInit_;

		if (path.length() > 1)
		{
			if (RegOpenKeyEx(h, path.data(), 0, KEY_ALL_ACCESS, &h) != ERROR_SUCCESS) { return false; }
		}

		if (RegDeleteValue(h, name.data()) == ERROR_SUCCESS)
		{
			if (path.length() > 1) { RegCloseKey(h); }
			return true;
		}
		else
		{
			if (path.length() > 1) { RegCloseKey(h); }
			return false;
		}
	}


	bool removeTree(char *path) 
	{
		HKEY h = hInit_;
		if (RegDeleteTree(h, path) == ERROR_SUCCESS)
		{
			return true;
		}
		else { return false; }
	}



private:
	HKEY hInit_;
};


namespace serviceHelper
{
	int installService(char* name, char* path)
	{
		SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		if (!hSCManager) {
			DWORD error = GetLastError();
			printf("Error: Can't open Service Control Manager");
			return error;
		}

		SC_HANDLE hService = CreateServiceA(
			hSCManager,
			name,
			name,
			SERVICE_ALL_ACCESS,
			SERVICE_WIN32_OWN_PROCESS,
			//SERVICE_KERNEL_DRIVER,
			SERVICE_AUTO_START,
			//SERVICE_DEMAND_START,
			SERVICE_ERROR_NORMAL,
			path,
			NULL, NULL, NULL, NULL, NULL
		);

		if (!hService) {
			int error = GetLastError();
			switch (error) {
			case ERROR_ACCESS_DENIED:
				printf("Error: ERROR_ACCESS_DENIED");
				break;
			case ERROR_CIRCULAR_DEPENDENCY:
				printf("Error: ERROR_CIRCULAR_DEPENDENCY");
				break;
			case ERROR_DUPLICATE_SERVICE_NAME:
				printf("Error: ERROR_DUPLICATE_SERVICE_NAME");
				break;
			case ERROR_INVALID_HANDLE:
				printf("Error: ERROR_INVALID_HANDLE");
				break;
			case ERROR_INVALID_NAME:
				printf("Error: ERROR_INVALID_NAME");
				break;
			case ERROR_INVALID_PARAMETER:
				printf("Error: ERROR_INVALID_PARAMETER");
				break;
			case ERROR_INVALID_SERVICE_ACCOUNT:
				printf("Error: ERROR_INVALID_SERVICE_ACCOUNT");
				break;
			case ERROR_SERVICE_EXISTS:
				printf("Error: ERROR_SERVICE_EXISTS");
				break;
			default:
				printf("Error: Undefined");
			}
			CloseServiceHandle(hSCManager);
			return error;
		}
		CloseServiceHandle(hService);

		CloseServiceHandle(hSCManager);
		printf("Success install service!");
		return 0;
	}

	int removeService(char* name)
	{
		SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hSCManager) {
			DWORD error = GetLastError();
			printf("Error: Can't open Service Control Manager");
			return error;
		}
		SC_HANDLE hService = OpenService(hSCManager, name, SERVICE_STOP | DELETE);
		if (!hService) {
			DWORD error = GetLastError();
			printf("Error: Can't remove service");
			CloseServiceHandle(hSCManager);
			return error;
		}

		DeleteService(hService);
		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		printf("Success remove service!");
		return 0;
	}

	int startService(char* name)
	{
		SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
		SC_HANDLE hService = OpenService(hSCManager, name, SERVICE_START);

		//cout << hService << endl;

		if (!StartService(hService, 0, NULL)) {

			DWORD error = GetLastError();


			CloseServiceHandle(hSCManager);
			printf("Error: Can't start service");
			return error;
		}

		//cout << "run";

		CloseServiceHandle(hService);
		CloseServiceHandle(hSCManager);
		return 0;
	}
};
