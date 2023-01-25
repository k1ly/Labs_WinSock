#include <iostream>
#include <Windows.h>
#include <string>

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case ERROR_BROKEN_PIPE: msgText = "The pipe has been ended."; break;
	case ERROR_PIPE_LOCAL: msgText = "The pipe is local."; break;
	case ERROR_BAD_PIPE: msgText = "The pipe state is invalid."; break;
	case ERROR_PIPE_BUSY: msgText = "All pipe instances are busy."; break;
	case ERROR_NO_DATA: msgText = "The pipe is being closed."; break;
	case ERROR_PIPE_NOT_CONNECTED: msgText = "No process is on the other end of the pipe."; break;
	case ERROR_PIPE_CONNECTED: msgText = "There is a process on other end of the pipe."; break;
	case ERROR_PIPE_LISTENING: msgText = "Waiting for a process to open the other end of the pipe."; break;
	case ERROR_CANNOT_IMPERSONATE: msgText = "Unable to impersonate using a named pipe until data has been read from that pipe."; break;
	default: msgText = string("***ERROR***") + "(" + to_string(code) + ")"; break;
	};
	return msgText;
};

string SetPipeError(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

int main()
{
	setlocale(LC_ALL, "ru");
	HANDLE hM;
	try
	{
		wchar_t* msName = (wchar_t*)L"\\\\.\\mailslot\\Box";
		hM = CreateMailslot(msName,
			300,
			3 * 60 * 1000,
			NULL);
		if (hM == INVALID_HANDLE_VALUE)
			throw SetPipeError("create: ", GetLastError());

		//char buf[50];
		//DWORD lbuf = 0;
		//if (!ReadFile(hM, buf, sizeof(buf), &lbuf, NULL))
		//	throw SetPipeError("readFile: ", GetLastError());
		//cout << buf << endl;

		clock_t time = clock();
		while (true)
		{
			char buf[50];
			DWORD lbuf = 0;
			if (!ReadFile(hM, buf, sizeof(buf), &lbuf, NULL))
				throw SetPipeError("readFile: ", GetLastError());
			if (lbuf == 0)
				break;
			cout << buf << endl;
		}
		cout << "Прошло секунд: " << (float)(clock() - time) / CLK_TCK << endl;
	}
	catch (string ErrorPipeText)
	{
		cout << ErrorPipeText << endl;
	}
}