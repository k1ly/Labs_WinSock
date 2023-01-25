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
	try
	{
		wchar_t* pipeName = (wchar_t*)L"\\\\.\\pipe\\Tube";

		int n = 0, i = 0;
		cout << "Введите число сообщений" << endl;
		cin >> n;
		while (i < n)
		{
			char buf[50] = "Hello from Client ";
			DWORD lbuf;
			_itoa_s(i, buf + strlen(buf), sizeof(buf) - strlen(buf), 10);
			if (!CallNamedPipe(pipeName, (void*)buf, strlen(buf) + 1, (void*)buf, sizeof(buf), &lbuf, NULL))
				throw SetPipeError("call: ", GetLastError());
			i = atoi(string(buf).substr(strlen("Hello from Client "), strlen(buf)).c_str()) + 1;
		}
		if (!CallNamedPipe(pipeName, (void*)"", 0, (void*)"", 0, NULL, NULL))
			throw SetPipeError("call: ", GetLastError());
	}
	catch (string ErrorPipeText)
	{
		cout << ErrorPipeText << endl;
	}
}