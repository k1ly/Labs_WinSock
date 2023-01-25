#include <string>
#include <iostream>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib")
#pragma warning(disable:4996)

#define SERVER_ADDRESS "127.0.0.1"

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

void GetCommandById(int id, char* buf)
{
	switch (id)
	{
	case 1: strcpy_s(buf, strlen("Echo") + 1, "Echo"); break;
	case 2: strcpy_s(buf, strlen("Time") + 1, "Time"); break;
	case 3: strcpy_s(buf, strlen("Rand") + 1, "Rand"); break;
	case 4: strcpy_s(buf, strlen("Exit") + 1, "Exit"); break;
	default: strcpy_s(buf, 1, "");
	}
}

int main()
{
	setlocale(0, "ru");
	try
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		SOCKET cC;
		if ((cC = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
			throw SetErrorMsgText("socket:", WSAGetLastError());
		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(SERVER_ADDRESS);
		if ((connect(cC, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR)
			throw SetErrorMsgText("connect:", WSAGetLastError());
		char wbuf[50] = "";
		char rbuf[50] = "";
		while (true)
		{
			printf("Input client command\n" \
				"1 - Echo\n" \
				"2 - Time\n" \
				"3 - Rand\n" \
				"4 - Exit\n");
			int s = scanf_s("%s", wbuf, (int)sizeof(wbuf));
			int id = atoi(wbuf);
			if (id != 0)
			{
				GetCommandById(id, wbuf);
				printf("command - %s\n", wbuf);
			}
			int libuf = 0, lobuf = 0;
			if ((lobuf = send(cC, wbuf, strlen(wbuf) + 1, NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("send:", WSAGetLastError());
			printf("send: %s\n", wbuf);
			if ((libuf = recv(cC, rbuf, sizeof(rbuf), NULL)) == SOCKET_ERROR)
				throw SetErrorMsgText("recv:", WSAGetLastError());
			printf("receive: %s\n", rbuf);
			if (strcmp(rbuf, "TimeOUT") == 0) {
				printf("Time out\n");
				break;
			}
			if (strcmp(wbuf, "Exit") == 0)
				break;
			else if (strcmp(wbuf, "Echo") == 0)
			{
				for (int i = 15; i >= 0; --i) {
					Sleep(1000);
					sprintf_s(wbuf, "%d", i);
					if ((lobuf = send(cC, wbuf, strlen(wbuf) + 1, NULL)) == SOCKET_ERROR)
						throw SetErrorMsgText("send:", WSAGetLastError());
					printf("send: %s\n", wbuf);
					if ((libuf = recv(cC, rbuf, sizeof(rbuf), NULL)) == SOCKET_ERROR)
						throw SetErrorMsgText("recv:", WSAGetLastError());
					printf("receive: %s\n", rbuf);
				}
			}
			else
				printf("receive: %s\n", rbuf);
		}
		if (closesocket(cC) == SOCKET_ERROR)
			throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (std::string errorMsgText)
	{
		printf("%s\n", errorMsgText.c_str());
	}
	system("pause");
	return 0;
}