#include <windows.h>
#include <iostream>
#include <string>

#define NAMED_PIPE_NAME L"\\\\.\\pipe\\CSConsole"

std::string GetErrorMsgText(int code)
{
	std::string msgText;
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
	default: msgText = std::string("***ERROR***") + "(" + std::to_string(code) + ")"; break;
	};
	return msgText;
};
std::string SetPipeError(std::string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

void GetCommandById(int id, char* buf)
{
	switch (id)
	{
	case 1: strcpy_s(buf, strlen("start") + 1, "start"); break;
	case 2: strcpy_s(buf, strlen("stop") + 1, "stop"); break;
	case 3: strcpy_s(buf, strlen("wait") + 1, "wait"); break;
	case 4: strcpy_s(buf, strlen("statistics") + 1, "statistics"); break;
	case 5: strcpy_s(buf, strlen("shutdown") + 1, "shutdown"); break;
	case 6: strcpy_s(buf, strlen("exit") + 1, "exit"); break;
	default: strcpy_s(buf, 1, "");
	}
}

int main()
{
	setlocale(LC_ALL, "ru");
	DWORD dwRead, dwWrite;
	HANDLE hPipe;
	try
	{
		if ((hPipe = CreateFile(NAMED_PIPE_NAME,
			GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL)) == INVALID_HANDLE_VALUE)
			throw SetPipeError("createfile:", GetLastError());
		char wbuf[40] = "";
		char rbuf[200] = "";
		while (true)
		{
			printf("Input server command\n" \
				"1 - start\n" \
				"2 - stop\n" \
				"3 - wait\n" \
				"4 - statistics\n" \
				"5 - shutdown\n" \
				"6 - exit\n");
			int s = scanf_s("%s", wbuf, (unsigned int)sizeof(wbuf));
			int id = atoi(wbuf);
			if (id != 0)
			{
				GetCommandById(id, wbuf);
				printf("command - %s\n", wbuf);
			}
			if (!WriteFile(hPipe, wbuf, strlen(wbuf) + 1, &dwWrite, NULL))
				throw SetPipeError("writefile:", GetLastError());
			printf("send: %s\n", wbuf);
			if (strcmp(rbuf, "exit") == 0)
				break;
			if (!ReadFile(hPipe, rbuf, sizeof(rbuf), &dwRead, NULL))
				throw SetPipeError("readFile: ", GetLastError());
			printf("receive: %s\n", rbuf);
		}
	}
	catch (std::string errorPipeText)
	{
		printf("%s\n", errorPipeText.c_str());
	}
	CloseHandle(hPipe);
	system("pause");
	return 0;
}