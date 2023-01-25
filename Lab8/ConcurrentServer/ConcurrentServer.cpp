#include "ConcurrentServer.h"
#define MAX_PORT_NUMBER 0x10000
#define AS_SQUIRT 10

int port = 2000;
wchar_t slName[50] = L"ServiceLibrary";
wchar_t npName[50] = L"CSConsole";

HMODULE sl;
HANDLE(*hfSSS)(char*, LPVOID);

HANDLE hAcceptServer,
hDispatchServer,
hConsolePipe,
hGarbageCleaner,
hResponseServer;
HANDLE hClientConnectedEvent = CreateEvent(NULL, FALSE, FALSE, L"ClientConnected");

volatile TalkersCommand cmd = START,
prev_cmd = GETCOMMAND;

SOCKET sS;
SOCKET sSUDP;
ListContact contacts;
CRITICAL_SECTION scListContact;

volatile long accepted = 0,
failed = 0,
finished = 0,
active = 0;

int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		int param = atoi(argv[1]);
		if (param > 0 && param < MAX_PORT_NUMBER)
			port = param;
	}
	if (argc > 2)
	{
		size_t num;
		mbstowcs_s(&num, slName, strlen(argv[2]) * 2, argv[2], strlen(argv[2]));
	}
	if (argc > 3)
	{
		size_t num;
		mbstowcs_s(&num, npName, strlen(argv[3]) * 2, argv[3], strlen(argv[3]));
	}

	setlocale(LC_ALL, "ru");
	wprintf(L"Номер используемого порта: %d\n", port);
	wprintf(L"Имя DLL библиотеки: %s\n", slName);
	wprintf(L"Имя именованного канала : %s\n", npName);

	sl = LoadLibrary(slName);
	if (sl != NULL)
		hfSSS = (HANDLE(*)(char*, LPVOID))GetProcAddress(sl, "SSS");

	InitializeCriticalSection(&scListContact);

	hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
	hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);
	hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
	hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
	hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);

	SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);
	SetThreadPriority(hResponseServer, THREAD_PRIORITY_ABOVE_NORMAL);
	SetThreadPriority(hDispatchServer, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hConsolePipe, THREAD_PRIORITY_NORMAL);
	SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_BELOW_NORMAL);

	WaitForSingleObject(hAcceptServer, INFINITE);
	CloseHandle(hAcceptServer);
	WaitForSingleObject(hConsolePipe, INFINITE);
	CloseHandle(hConsolePipe);
	WaitForSingleObject(hGarbageCleaner, INFINITE);
	CloseHandle(hGarbageCleaner);

	TerminateThread(hDispatchServer, 0);
	printf("DispatchServer shutdown");
	TerminateThread(hResponseServer, 0);
	printf("ResponseServer shutdown");
	CloseHandle(hDispatchServer);
	CloseHandle(hResponseServer);

	DeleteCriticalSection(&scListContact);

	if (sl != NULL)
		FreeLibrary(sl);
}

DWORD WINAPI AcceptServer(LPVOID pPrm)
{
	DWORD rc = 0;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		CommandsCycle(*((TalkersCommand*)pPrm));
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		printf("\n%s", errorMsgText.c_str());
	}
	printf("AcceptServer shutdown");
	ExitThread(rc);
}

bool AcceptCycle(int squirt)
{
	bool rc = false;
	Contact c(Contact::ACCEPT, "EchoServer");
	while (squirt-- > 0 && !rc)
	{
		if ((c.s = accept(sS, (sockaddr*)&c.prms, &c.lprms)) == INVALID_SOCKET)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
				throw SetErrorMsgText("accept:", WSAGetLastError());
		}
		else
		{
			rc = true;
			EnterCriticalSection(&scListContact);
			contacts.push_front(c);
			LeaveCriticalSection(&scListContact);
			printf("Client connected");
			InterlockedIncrement(&accepted);
			//InterlockedIncrement(&active);
		}
	}
	return rc;
};

void openSocket() {
	SOCKADDR_IN serv;
	sockaddr_in clnt;
	u_long nonblk = 1;
	if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
		throw  SetErrorMsgText("socket:", WSAGetLastError());
	int lclnt = sizeof(clnt);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;
	if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw  SetErrorMsgText("bind:", WSAGetLastError());
	if (listen(sS, SOMAXCONN) == SOCKET_ERROR)
		throw  SetErrorMsgText("listen:", WSAGetLastError());
	if (ioctlsocket(sS, FIONBIO, &nonblk) == SOCKET_ERROR)
		throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
}

void closeSocket() {
	if (closesocket(sS) == SOCKET_ERROR)
		throw  SetErrorMsgText("closesocket:", WSAGetLastError());
}

void CommandsCycle(TalkersCommand& cmd)
{
	int squirt = 0;
	while (cmd != EXIT)
	{
		switch (cmd)
		{
		case START: cmd = GETCOMMAND;
			if (prev_cmd != START) {
				squirt = AS_SQUIRT;
				printf("---------- START ----------\n");
				openSocket();
				prev_cmd = START;
			}
			else printf("Already started...");
			break;
		case STOP: cmd = GETCOMMAND;
			if (prev_cmd != STOP) {
				squirt = 0;
				printf("---------- STOP ----------\n");
				closeSocket();
				prev_cmd = STOP;
			}
			else printf("Already stopped...");
			break;
		case WAIT: cmd = GETCOMMAND;
			squirt = 0;
			printf("---------- WAIT ----------\n");
			printf("Socket closed for waiting other clients\n");
			closeSocket();
			while (contacts.size() != 0);
			printf("Size of contacts %d\n", (int)contacts.size());
			cmd = START;
			prev_cmd = WAIT;
			printf("Socket is open");
			break;
		case SHUTDOWN:
			squirt = 0;
			printf("---------- SHUTDOWN ----------\n");
			closeSocket();
			while (contacts.size() != 0);
			printf("Size of contacts %d\n", (int)contacts.size());
			cmd = EXIT;
			break;
		case GETCOMMAND: cmd = GETCOMMAND;
			break;
		}
		if (cmd != STOP) {
			if (AcceptCycle(squirt))
			{
				cmd = GETCOMMAND;
				SetEvent(hClientConnectedEvent);
			}
			else SleepEx(0, TRUE);
		}
	};
}

DWORD WINAPI DispatchServer(LPVOID pPrm)
{
	DWORD rc = 0;
	printf("DispatchServer started\n");
	TalkersCommand& command = *(TalkersCommand*)pPrm;
	while (command != EXIT)
	{
		if (command != STOP)
		{
			WaitForSingleObject(hClientConnectedEvent, INFINITE);
			ResetEvent(hClientConnectedEvent);
			while (true)
			{
				for (auto i = contacts.begin(); i != contacts.end(); i++)
				{
					if (i->type == Contact::ACCEPT)
					{
						/*u_long nonblk = 0;
						if (ioctlsocket(i->s, FIONBIO, &nonblk) == SOCKET_ERROR)
							throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());*/
						char service[10];
						if (recv(i->s, service, sizeof(service), NULL) < 1)
							continue;
						clock_t start = clock();
						strcpy_s(i->msg, service);
						clock_t timeOut = clock() - start;
						if (timeOut > 60 * 1000)
						{
							printf("Request timed out!\n");
							if ((send(i->s, "TimeOUT", strlen("TimeOUT") + 1, NULL)) == SOCKET_ERROR)
								throw SetErrorMsgText("send:", WSAGetLastError());
							i->sthread = Contact::TIMEOUT;
							i->type = Contact::EMPTY;
							if (closesocket(i->s) == SOCKET_ERROR)
								throw SetErrorMsgText("closesocket:", WSAGetLastError());
							continue;
						}
						else {
							if (!strcmp(i->msg, "close")) {
								if ((send(i->s, "Server close", strlen("Server close") + 1, NULL)) == SOCKET_ERROR)
									throw  SetErrorMsgText("send:", WSAGetLastError());
								i->sthread = Contact::FINISH;
								i->type = Contact::EMPTY;
								continue;
							}
							else if (strcmp(i->msg, "Echo") && strcmp(i->msg, "Time") && strcmp(i->msg, "Rand"))
							{
								if ((send(i->s, "ErrorInquiry", strlen("ErrorInquiry") + 1, NULL)) == SOCKET_ERROR)
									throw SetErrorMsgText("send:", WSAGetLastError());
								i->sthread = Contact::ABORT;
								i->type = Contact::EMPTY;
								if (closesocket(i->s) == SOCKET_ERROR)
									throw SetErrorMsgText("closesocket:", WSAGetLastError());
							}
							else
							{
								i->type = Contact::CONTACT;
								i->hthread = hAcceptServer;
								i->hsthread = hfSSS(i->msg, (LPVOID) & (*i));
								i->htimer = CreateWaitableTimer(0, FALSE, 0);
								LARGE_INTEGER Li;
								Li.QuadPart = -(60 * 10000000);
								SetWaitableTimer(i->htimer, &Li, 0, ASWTimer, (LPVOID) & (*i), FALSE);
								SleepEx(0, TRUE);
							}
						}
					}
				}
				Sleep(200);
			}
		}
	}
	ExitThread(rc);
}

void CALLBACK ASWTimer(LPVOID pPrm, DWORD, DWORD) {
	Contact* contact = (Contact*)pPrm;
	printf("ASWTimer is calling %p\n", contact->hthread);
	TerminateThread(contact->hsthread, NULL);
	send(contact->s, "TimeOUT", strlen("TimeOUT") + 1, NULL);
	EnterCriticalSection(&scListContact);
	CancelWaitableTimer(contact->htimer);
	contact->type = contact->EMPTY;
	contact->sthread = contact->TIMEOUT;
	LeaveCriticalSection(&scListContact);
}

TalkersCommand SetParam(char* param) {
	if (!strcmp(param, "start")) return START;
	if (!strcmp(param, "stop")) return STOP;
	if (!strcmp(param, "exit")) return EXIT;
	if (!strcmp(param, "wait")) return WAIT;
	if (!strcmp(param, "shutdown")) return SHUTDOWN;
	if (!strcmp(param, "statistics")) return STATISTICS;
	if (!strcmp(param, "getcommand")) return GETCOMMAND;
}

DWORD WINAPI ConsolePipe(LPVOID pPrm)
{
	printf("ConsolePipe started\n");
	DWORD rc = 0;
	char rbuf[100] = "";
	DWORD dwRead, dwWrite;
	HANDLE hPipe;
	try
	{
		wchar_t npNameBuf[50] = L"\\\\.\\pipe\\";
		wcscat_s(npNameBuf, npName);
		if ((hPipe = CreateNamedPipe(npNameBuf, PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_WAIT, 1, NULL, NULL, INFINITE, NULL)) == INVALID_HANDLE_VALUE)
			throw SetErrorMsgText("create:", GetLastError());
		if (!ConnectNamedPipe(hPipe, NULL))
			throw SetErrorMsgText("connect:", GetLastError());
		TalkersCommand& param = *((TalkersCommand*)pPrm);
		while (param != EXIT && param != SHUTDOWN) {
			wprintf(L"---- Connecting to Named Pipe \"%s\" ----\n", npName);
			ConnectNamedPipe(hPipe, NULL);
			while (ReadFile(hPipe, rbuf, sizeof(rbuf), &dwRead, NULL))
			{
				printf("Remote console message: %s\n", rbuf);
				param = SetParam(rbuf);
				if (param == STATISTICS)
				{
					char stat[200];
					sprintf_s(stat, sizeof(stat),
						"\n---- STATISTICS ----\n"\
						"Connections count: %d\n" \
						"Failed: %d\n" \
						"Completed successfully: %d\n" \
						"Active connections: %d\n" \
						"-------------------------------------", accepted, failed, finished, (int)contacts.size());
					WriteFile(hPipe, stat, strlen(stat) + 1, &dwWrite, NULL);
				}
				if (param != STATISTICS)
					WriteFile(hPipe, rbuf, strlen(rbuf) + 1, &dwWrite, NULL);
				if (param == EXIT || param == SHUTDOWN)
					break;
			}
			printf("-------------- Pipe Closed -----------------\n");
			DisconnectNamedPipe(hPipe);
		}
	}
	catch (std::string errorPipeText)
	{
		printf("\n%s", errorPipeText.c_str());
		rc = -1;
	}
	if (hPipe != NULL)
		CloseHandle(hPipe);
	printf("ConsolePipe stopped\n");
	ExitThread(rc);
}

DWORD WINAPI GarbageCleaner(LPVOID pPrm)
{
	printf("GarbageCleaner started\n");
	DWORD rc = 0;
	while (*((TalkersCommand*)pPrm) != EXIT) {
		if (contacts.size() != 0) {
			for (auto i = contacts.begin(); i != contacts.end();) {
				if (i->type == Contact::EMPTY)
				{
					EnterCriticalSection(&scListContact);
					if (i->sthread == Contact::FINISH)
						InterlockedIncrement(&finished);
					if (i->sthread == Contact::ABORT || i->sthread == Contact::TIMEOUT)
						InterlockedIncrement(&failed);
					i = contacts.erase(i);
					LeaveCriticalSection(&scListContact);
					Sleep(2000);
				}
				else ++i;
			}
		}
	}
	printf("GarbageCleaner stopped\n");
	ExitThread(rc);
}

bool PutAnswerToClient(char* name, sockaddr* to, int* lto) {

	char msg[] = "You can connect to server ";
	if ((sendto(sSUDP, msg, strlen(msg) + 1, NULL, to, *lto)) == SOCKET_ERROR)
		throw SetErrorMsgText("sendto:", WSAGetLastError());
	return false;
}

bool GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen)
{
	SOCKADDR_IN clnt;
	int lc = sizeof(clnt);
	ZeroMemory(&clnt, lc);
	char ibuf[500];
	int lb = 0;
	int optval = 1;
	int TimeOut = 10;
	setsockopt(sSUDP, SOL_SOCKET, SO_BROADCAST, (char*)&optval, sizeof(int));
	setsockopt(sSUDP, SOL_SOCKET, SO_RCVTIMEO, (char*)&TimeOut, sizeof(TimeOut));
	while (true) {
		if ((lb = recvfrom(sSUDP, ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &lc)) == SOCKET_ERROR) return false;
		printf("Server - %s\n", ibuf);
		if (!strcmp(name, ibuf)) {
			*from = clnt;
			*flen = lc;
			return true;
		}
		printf("\nBad name!\n");
	}
	return false;
}

DWORD WINAPI ResponseServer(LPVOID pPrm)
{
	printf("ResponseServer started\n");
	DWORD rc = 0;
	WSADATA wsaData;
	SOCKADDR_IN serv;
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		throw SetErrorMsgText("startup:", WSAGetLastError());
	if ((sSUDP = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket:", WSAGetLastError());
	serv.sin_family = AF_INET;
	serv.sin_port = htons(port);
	serv.sin_addr.s_addr = INADDR_ANY;
	if (bind(sSUDP, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR)
		throw SetErrorMsgText("bind:", WSAGetLastError());
	SOCKADDR_IN someServer;
	int serverSize = sizeof(someServer);
	SOCKADDR_IN from;
	int lc = sizeof(from);
	ZeroMemory(&from, lc);
	int numberOfClients = 0;
	while (*(TalkersCommand*)pPrm != EXIT)
	{
		try
		{
			if (GetRequestFromClient((char*)"Hello", port, &from, &lc))
			{
				printf("\nConnected Client: №%d, port: %d, ip: %s", ++numberOfClients, htons(from.sin_port), inet_ntoa(from.sin_addr));
				PutAnswerToClient((char*)"Hello", (sockaddr*)&from, &lc);
			}
		}
		catch (std::string errorMsgText)
		{
			printf("\n%s", errorMsgText.c_str());
		}
	}
	if (closesocket(sSUDP) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket:", WSAGetLastError());
	if (WSACleanup() == SOCKET_ERROR)
		throw SetErrorMsgText("cleanup:", WSAGetLastError());
	printf("ResponseServer stopped\n");
	ExitThread(rc);
}