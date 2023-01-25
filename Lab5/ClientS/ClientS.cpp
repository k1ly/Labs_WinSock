#include <iostream>
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

string GetErrorMsgText(int code)
{
	string msgText;
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

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};

bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen);

int main()
{
	setlocale(LC_ALL, "ru");
	try
	{
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
			throw SetErrorMsgText("Startup: ", WSAGetLastError());
		char name[50] = "KIRILL-PROBOOK";
		char call[50] = "Server1";
		sockaddr from;
		((SOCKADDR_IN*)&from)->sin_port = htons(2000);
		int flen;
		GetServerByName(name, call, &from, &flen);
		if (WSACleanup() == SOCKET_ERROR)
			throw SetErrorMsgText("Cleanup: ", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
}

bool GetServerByName(char* name, char* call, struct sockaddr* from, int* flen)
{
	bool rc = false;
	SOCKET cC;
	if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET)
		throw SetErrorMsgText("socket:", WSAGetLastError());
	hostent* addr = gethostbyname(name);
	if (addr == NULL)
		throw SetErrorMsgText("gethostbyname:", WSAGetLastError());
	if (addr->h_addrtype == AF_INET)
	{
		int i = 0;
		((SOCKADDR_IN*)from)->sin_family = AF_INET;
		while (addr->h_addr_list[i] != NULL)
			((SOCKADDR_IN*)from)->sin_addr.s_addr = *(u_long*)(addr->h_addr_list[i++]);
	}
	if (sendto(cC, call, strlen(call) + 1, NULL,
		from, sizeof(*(SOCKADDR_IN*)from)) == SOCKET_ERROR)
		throw SetErrorMsgText("sendto:", WSAGetLastError());
	*flen = sizeof(*(SOCKADDR_IN*)from);
	char ibuf[50];
	int libuf = 0;
	if ((libuf = recvfrom(cC, ibuf, sizeof(ibuf), NULL,
		from, flen)) == SOCKET_ERROR)
	{
		int errorCode = WSAGetLastError();
		if (errorCode != WSAETIMEDOUT)
			throw SetErrorMsgText("recvfrom:", errorCode);
	}
	if (strcmp(ibuf, call) == 0) {
		cout << "Server IP - " << inet_ntoa(((SOCKADDR_IN*)from)->sin_addr) << " (" << call << ")" << endl;
		rc = true;
	}
	if (closesocket(cC) == SOCKET_ERROR)
		throw SetErrorMsgText("closesocket: ", WSAGetLastError());
	return rc;
}