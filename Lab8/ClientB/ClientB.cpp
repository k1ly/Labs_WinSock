#include <string>
#include <iostream>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable:4996)

std::string GetErrorMsgText(int code)
{
	std::string msgText;
	switch (code)
	{
	case	WSAEINTR: msgText = "Работа функции прервана"; break;
	case	WSAEACCES: msgText = "Разрешение отвергнуто"; break;
	case	WSAEFAULT: msgText = "Ошибочный адрес"; break;
	case	WSAEINVAL: msgText = "Ошибка в аргументе"; break;
	case	WSAEMFILE: msgText = "Слишком много файлов открыто"; break;
	case	WSAEWOULDBLOCK: msgText = "Ресурс временно недоступен"; break;
	case	WSAEINPROGRESS: msgText = "Операция в процессе развития"; break;
	case	WSAEALREADY: msgText = "Операция уже выполняется"; break;
	case	WSAENOTSOCK: msgText = "Сокет задан неправильно"; break;
	case	WSAEDESTADDRREQ: msgText = "Требуется адрес расположения"; break;
	case	WSAEMSGSIZE: msgText = "Сообщение слишком длинное"; break;
	case	WSAEPROTOTYPE: msgText = "Неправильный тип протокола для сокета"; break;
	case	WSAENOPROTOOPT: msgText = "Ошибка в опции протокола"; break;
	case	WSAEPROTONOSUPPORT: msgText = "Протокол не поддерживается"; break;
	case	WSAESOCKTNOSUPPORT: msgText = "Тип сокета не поддерживается"; break;
	case	WSAEOPNOTSUPP: msgText = "Операция не поддерживается"; break;
	case	WSAEPFNOSUPPORT: msgText = "Тип протоколов не поддерживается"; break;
	case	WSAEAFNOSUPPORT: msgText = "Тип адресов не поддерживается протоколом"; break;
	case	WSAEADDRINUSE: msgText = "Адрес уже используется"; break;
	case	WSAEADDRNOTAVAIL: msgText = "Запрошенный адрес не может быть использован"; break;
	case	WSAENETDOWN: msgText = "Сеть отключена"; break;
	case	WSAENETUNREACH: msgText = "Сеть не достижима"; break;
	case	WSAENETRESET: msgText = "Сеть разорвала соединение"; break;
	case	WSAECONNABORTED: msgText = "Программный отказ связи"; break;
	case	WSAECONNRESET: msgText = "Связь восстановлена"; break;
	case	WSAENOBUFS: msgText = "Не хватает памяти для буферов"; break;
	case	WSAEISCONN: msgText = "Сокет уже подключен"; break;
	case	WSAENOTCONN: msgText = "Сокет не подключен"; break;
	case	WSAESHUTDOWN: msgText = "Нельзя выполнить send : сокет завершил работу"; break;
	case	WSAETIMEDOUT: msgText = "Закончился отведенный интервал времени"; break;
	case	WSAECONNREFUSED: msgText = "Соединение отклонено"; break;
	case	WSAEHOSTDOWN: msgText = "Хост в неработоспособном состоянии"; break;
	case	WSAEHOSTUNREACH: msgText = "Нет маршрута для хоста"; break;
	case	WSAEPROCLIM: msgText = "Слишком много процессов"; break;
	case	WSASYSNOTREADY: msgText = "Сеть не доступна"; break;
	case	WSAVERNOTSUPPORTED: msgText = "Данная версия недоступна"; break;
	case	WSANOTINITIALISED: msgText = "Не выполнена инициализация WS2_32.DLL"; break;
	case	WSAEDISCON: msgText = "Выполняется отключение"; break;
	case	WSATYPE_NOT_FOUND: msgText = "Класс не найден"; break;
	case	WSAHOST_NOT_FOUND: msgText = "Хост не найден"; break;
	case	WSATRY_AGAIN: msgText = "Неавторизированный хост не найден"; break;
	case	WSANO_RECOVERY: msgText = "Неопределенная ошибка"; break;
	case	WSANO_DATA: msgText = "Нет записи запрошенного типа"; break;
	case	WSA_INVALID_HANDLE: msgText = "Указанный дескриптор события с ошибкой"; break;
	case	WSA_INVALID_PARAMETER: msgText = "Один или более параметров с ошибкой"; break;
	case	WSA_IO_INCOMPLETE: msgText = "Объект ввода - вывода не в сигнальном состоянии"; break;
	case	WSA_IO_PENDING: msgText = "Операция завершится позже"; break;
	case	WSA_NOT_ENOUGH_MEMORY: msgText = "Не достаточно памяти"; break;
	case	WSA_OPERATION_ABORTED: msgText = "Операция отвергнута"; break;
	case	WSASYSCALLFAILURE: msgText = "Аварийное завершение системного вызова"; break;
	default: msgText = "***ERROR***"; break;
	};
	return msgText;
};
std::string SetErrorMsgText(std::string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

SOCKET  cS;

bool GetServer(char* call, short port, SOCKADDR_IN* from, int* flen)
{
	SOCKADDR_IN server;
	memset(&server, 0, sizeof(server));
	char message[50];
	int lc = sizeof(server);
	int  lb = 0;
	if ((lb = recvfrom(cS, message, sizeof(message), NULL, (sockaddr*)&server, &lc)) == SOCKET_ERROR)
		throw  SetErrorMsgText("recfrom:", WSAGetLastError());
	if (GetLastError() == WSAETIMEDOUT) return false;
	printf("GetServer: \n");
	printf("port: %d\n", htons(server.sin_port));
	printf("adress: %s\n", inet_ntoa(server.sin_addr));
	printf("call: %s\n", message);
	*from = server;
	*flen = lc;
	return true;
}

int main()
{
	setlocale(0, "ru");
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw  SetErrorMsgText("Startup:", WSAGetLastError());
		if ((cS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
			throw  SetErrorMsgText("socket:", WSAGetLastError());
		int optval = 1;
		if (setsockopt(cS, SOL_SOCKET, SO_BROADCAST,
			(char*)&optval, sizeof(int)) == SOCKET_ERROR)
			throw  SetErrorMsgText("opt:", WSAGetLastError());
		SOCKADDR_IN all;
		all.sin_family = AF_INET;
		all.sin_port = htons(2000);
		all.sin_addr.s_addr = INADDR_BROADCAST;
		char buf[] = "Hello";
		if ((sendto(cS, buf, sizeof(buf), NULL,
			(sockaddr*)&all, sizeof(all))) == SOCKET_ERROR)
			throw  SetErrorMsgText("sendto:", WSAGetLastError());
		SOCKADDR_IN server;
		int lc = sizeof(server);
		char messageFromServer[50];
		GetServer((char*)"Hello", 2000, &server, &lc);
		if (closesocket(cS) == SOCKET_ERROR)
			throw  SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw  SetErrorMsgText("Cleanup:", WSAGetLastError());
		system("pause");
	}
	catch (std::string errorMsgText)
	{
		printf("%s\n", errorMsgText.c_str());
	}
	return 0;
}