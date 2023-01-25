#pragma once
#include "ServiceLibrary.h"
#include <list>

enum TalkersCommand
{
	START,
	STATISTICS,
	STOP,
	EXIT,
	WAIT,
	SHUTDOWN,
	GETCOMMAND
};

typedef std::list<Contact> ListContact;

DWORD WINAPI AcceptServer(LPVOID pPrm);
DWORD WINAPI ResponseServer(LPVOID pPrm);
DWORD WINAPI DispatchServer(LPVOID pPrm);
DWORD WINAPI ConsolePipe(LPVOID pPrm);
DWORD WINAPI GarbageCleaner(LPVOID pPrm);

bool AcceptCycle(int squirt);
void CommandsCycle(TalkersCommand& cmd);
bool PutAnswerToClient(char* name, sockaddr* to, int* lto);
bool GetRequestFromClient(char* name, short port, SOCKADDR_IN* from, int* flen);

void CALLBACK ASWTimer(LPVOID Prm, DWORD, DWORD);