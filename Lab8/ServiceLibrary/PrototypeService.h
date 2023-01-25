#pragma once
#include "pch.h"
#include "ServiceLibrary.h"
#include <string>

std::string runServer;

const std::string GetCurrentDateTime() {
	time_t now = time(0);
	tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
	return buf;
}

void CALLBACK ASStartMessage(DWORD p)
{
	char hostName[4];
	gethostname(hostName, sizeof(hostName));
	printf("RunServer: %s  \nstart time:  %s\n", runServer.c_str(), GetCurrentDateTime().c_str());
}

void CALLBACK ASFinishMessage(DWORD p)
{
	printf("finish time: %s\n", GetCurrentDateTime().c_str());
}

void QueueUserAPCWrapper(PAPCFUNC functionName, Contact* contact) {
	QueueUserAPC(functionName, contact->hthread, 0);
}

void SendMessageToClient(Contact* contact)
{
	if (send(contact->s, contact->msg, sizeof(contact->msg), NULL) == SOCKET_ERROR)
		throw  SetErrorMsgText("send:", WSAGetLastError());
}

DWORD WINAPI EchoServer(LPVOID pPrm)
{
	DWORD rc = 0;
	Contact* contact = (Contact*)(pPrm);
	u_long nonblk = 0;
	try
	{
		runServer = "EchoServer";
		QueueUserAPCWrapper((PAPCFUNC)ASStartMessage, contact);
		int lobuf, libuf;
		contact->sthread = contact->WORK;
		contact->type = contact->CONTACT;
		strcpy(contact->msg, "Start transmission");
		SendMessageToClient(contact);
		if (ioctlsocket(contact->s, FIONBIO, &nonblk) == SOCKET_ERROR)
			throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
		while (true) {

			if ((libuf = recv(contact->s, contact->msg, sizeof(contact->msg), NULL)) == SOCKET_ERROR)
				throw  SetErrorMsgText("recv:", WSAGetLastError());
			SendMessageToClient(contact);
			if (atoi(contact->msg) == 0) break;
		}
	}
	catch (...)
	{
		printf("EchoServer error");
		contact->sthread = contact->ABORT;
		contact->type = contact->EMPTY;
		rc = contact->sthread;
		QueueUserAPCWrapper((PAPCFUNC)ASFinishMessage, contact);
		CancelWaitableTimer(contact->htimer);
		ExitThread(rc);
	}
	/*contact->sthread = contact->FINISH;
	contact->type = contact->EMPTY;
	rc = contact->sthread;*/
	contact->type = contact->ACCEPT;
	nonblk = 1;
	if (ioctlsocket(contact->s, FIONBIO, &nonblk) == SOCKET_ERROR)
		throw SetErrorMsgText("ioctlsocket:", WSAGetLastError());
	QueueUserAPCWrapper((PAPCFUNC)ASFinishMessage, contact);
	CancelWaitableTimer(contact->htimer);
	ExitThread(rc);
}

DWORD WINAPI TimeServer(LPVOID pPrm)
{
	DWORD rc = 0;
	Contact* contact = (Contact*)(pPrm);
	runServer = "TimeServer";
	int lobuf, libuf;
	QueueUserAPCWrapper((PAPCFUNC)ASStartMessage, contact);
	strcpy(contact->msg, GetCurrentDateTime().c_str());
	SendMessageToClient(contact);
	/*contact->sthread = contact->FINISH;
	contact->type = contact->EMPTY;
	rc = contact->sthread;*/
	contact->type = contact->ACCEPT;
	QueueUserAPCWrapper((PAPCFUNC)ASFinishMessage, contact);
	CancelWaitableTimer(contact->htimer);
	ExitThread(rc);
}

DWORD WINAPI RandServer(LPVOID pPrm)
{
	DWORD rc = 0;
	Contact* contact = (Contact*)(pPrm);
	runServer = "RandServer";
	QueueUserAPCWrapper((PAPCFUNC)ASStartMessage, contact);
	srand(time(NULL));
	int lobuf, libuf, randNumber;
	randNumber = rand() % 100 + 1;
	sprintf(contact->msg, "%d", randNumber);
	SendMessageToClient(contact);
	/*contact->sthread = contact->FINISH;
	contact->type = contact->EMPTY;
	rc = contact->sthread;*/
	contact->type = contact->ACCEPT;
	QueueUserAPCWrapper((PAPCFUNC)ASFinishMessage, contact);
	CancelWaitableTimer(contact->htimer);
	ExitThread(rc);
}