#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include <tchar.h>
#include <ctime>
#include <fstream>
#include "Winsock2.h"
#pragma comment(lib, "WS2_32.lib") 

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

#pragma comment(lib, "mpr.lib") // Нужно для WNetGetUniversalNameW

struct CA // блок управления секцией
{
	char ipaddr[15]; // ip-адрес (xxx.xxx.xxx.xxx) координатора
	char resurce[40]; // имя ресурса
	char clientID[20];

	enum STATUS
	{
		NOINIT, // начальное состояние
		INIT, // выполнена инициализация
		ENTER, // выполнен вход в секцию
		LEAVE, // выполнен выход из секции
		WAIT // ожидание входа
	} status; // состояние
};

CA InitCA( // инициализировать критическую секцию status -> INIT/NOINIT
	char ipaddr[15], // ip адрес (xxx.xxx.xxx.xxx) координатора
	char resurce[40], // имя ресурса
	char clientID[20]
);

bool EnterCA( // войти в секцию status -> ENTER/WAIT -> ENTER
	CA& ca // блок управления секцией
);

bool LeaveCA( // покинуть секцию status -> LEAVE
	CA& ca // блок управления секцией
);

bool CloseCA( // закрыть секцию  status - > NOINIT
	CA& ca //блок управления секцией 
);


int main()
{
	CA s;
	string ipAddress = "26.69.212.139";
	string resource = "N:\\test.txt";
	string clientID = "client";
	try
	{
		cout << "Enter your username" << endl;
		cin >> clientID;

		s = InitCA(const_cast<char*>(ipAddress.c_str()), const_cast<char*>(resource.c_str()), const_cast<char*>(clientID.c_str()));
		EnterCA(s);
		string line;
		ofstream ss(resource, ios::app);
		for (int i = 0; i < 5; i++)
		{
			time_t rawtime;
			struct tm* timeinfo;
			time(&rawtime);
			timeinfo = localtime(&rawtime);
			if (!ss.is_open()) {
				cout << "Failed to open file: " << s.resurce << endl;
				return 1;
			}
			if (ss.is_open())
			{
				ss << "Client (" << s.clientID << ") #" << i + 1 << " : " << asctime(timeinfo) << endl;
			}
			Sleep(3000);
		}
		ss.close();
		LeaveCA(s);
		CloseCA(s);
	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
		CloseCA(s);
	}
	system("pause");
	return 0;
}

CA InitCA(char ipaddr[15], char resurce[20], char clientID[20])
{
	CA ca;
	strcpy(ca.ipaddr, ipaddr);
	strcpy(ca.resurce, resurce);
	strcpy(ca.clientID, clientID);
	SOCKET cC;
	WSADATA wsaData;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(ca.ipaddr);

		int lobuf = 0;
		int libuf = 0;
		int serv_size = sizeof(serv);

		ca.status = ca.INIT;

		if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
		if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
		if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("cleanup:", WSAGetLastError());
		return ca;
	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
		return ca;
	}
}

bool EnterCA(CA& ca)
{
	if (ca.status == ca.NOINIT) return false;
	SOCKET cC;
	WSADATA wsaData;

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(ca.ipaddr);

		int lobuf = 0;
		int libuf = 0;
		int serv_size = sizeof(serv);

		SOCKADDR_IN clnt;
		memset(&clnt, 0, sizeof(clnt));
		int lc = sizeof(clnt);

		while (ca.status != ca.ENTER)
		{
			ca.status = ca.ENTER;

			if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
			if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 2:", WSAGetLastError());
			if (ca.status == ca.ENTER) cout << "The section is free! You are starting to write to this resource" << endl;
		}

		if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("cleanup:", WSAGetLastError());
		return true;
	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
		return false;
	}
}

bool LeaveCA(CA& ca)
{
	if (ca.status == ca.NOINIT) return false;
	SOCKET cC;
	WSADATA wsaData;

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(ca.ipaddr);

		int lobuf = 0;
		int libuf = 0;
		int serv_size = sizeof(serv);

		SOCKADDR_IN clnt;
		memset(&clnt, 0, sizeof(clnt));
		int lc = sizeof(clnt);

		while (ca.status != ca.LEAVE)
		{
			ca.status = ca.LEAVE;

			if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
			if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 3:", WSAGetLastError());
		}

		if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("cleanup:", WSAGetLastError());
		return true;
	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
		return false;
	}
}

bool CloseCA(CA& ca)
{
	if (ca.status == ca.NOINIT) return false;
	SOCKET cC;
	WSADATA wsaData;

	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(ca.ipaddr);

		int lobuf = 0;
		int libuf = 0;
		int serv_size = sizeof(serv);

		SOCKADDR_IN clnt;
		memset(&clnt, 0, sizeof(clnt));
		int lc = sizeof(clnt);

		while (ca.status != ca.NOINIT)
		{
			ca.status = ca.NOINIT;

			if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
			if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 4:", WSAGetLastError());
		}

		if (closesocket(cC) == SOCKET_ERROR) throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("cleanup:", WSAGetLastError());
		return true;
	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
		return false;
	}
}

string GetErrorMsgText(int code)
{
	string msgText;
	switch (code)
	{
	case WSAEINTR: msgText = "WSAEINTR"; break;
	case WSAEACCES: msgText = "WSAEACCES"; break;
	case WSAEFAULT: msgText = "WSAEFAULT"; break;
	case WSAEINVAL: msgText = "WSAEINVAL"; break;
	case WSAEMFILE: msgText = "WSAEMFILE"; break;
	case WSAEWOULDBLOCK: msgText = "WSAEWOULDBLOCK"; break;
	case WSAEINPROGRESS: msgText = "WSAEINPROGRESS"; break;
	case WSAEALREADY: msgText = "WSAEALREADY"; break;
	case WSAENOTSOCK: msgText = "WSAENOTSOCK"; break;
	case WSAEDESTADDRREQ: msgText = "WSAEDESTADDRREQ"; break;
	case WSAEMSGSIZE: msgText = "WSAEMSGSIZE"; break;
	case WSAEPROTOTYPE: msgText = "WSAEPROTOTYPE"; break;
	case WSAENOPROTOOPT: msgText = "WSAENOPROTOOPT"; break;
	case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT"; break;
	case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT"; break;
	case WSAEOPNOTSUPP: msgText = "WSAEOPNOTSUPP"; break;
	case WSAEPFNOSUPPORT: msgText = "WSAEPFNOSUPPORT"; break;
	case WSAEAFNOSUPPORT: msgText = "WSAEAFNOSUPPORT"; break;
	case WSAEADDRINUSE: msgText = "WSAEADDRINUSE"; break;
	case WSAEADDRNOTAVAIL: msgText = "WSAEADDRNOTAVAIL"; break;
	case WSAENETDOWN: msgText = "WSAENETDOWN"; break;
	case WSAENETUNREACH: msgText = "WSAENETUNREACH"; break;
	case WSAENETRESET: msgText = "WSAENETRESET"; break;
	case WSAECONNABORTED: msgText = "WSAECONNABORTED"; break;
	case WSAECONNRESET: msgText = "WSAECONNRESET"; break;
	case WSAENOBUFS: msgText = "WSAENOBUFS"; break;
	case WSAEISCONN: msgText = "WSAEISCONN"; break;
	case WSAENOTCONN: msgText = "WSAENOTCONN"; break;
	case WSAESHUTDOWN: msgText = "WSAESHUTDOWN"; break;
	case WSAETIMEDOUT: msgText = "WSAETIMEDOUT"; break;
	case WSAECONNREFUSED: msgText = "WSAECONNREFUSED"; break;
	case WSAEHOSTDOWN: msgText = "WSAEHOSTDOWN"; break;
	case WSAEHOSTUNREACH: msgText = "WSAEHOSTUNREACH"; break;
	case WSAEPROCLIM: msgText = "WSAEPROCLIM"; break;
	case WSASYSNOTREADY: msgText = "WSASYSNOTREADY"; break;
	case WSAVERNOTSUPPORTED: msgText = "WSAVERNOTSUPPORTED"; break;
	case WSANOTINITIALISED: msgText = "WSANOTINITIALISED"; break;
	case WSAEDISCON: msgText = "WSAEDISCON"; break;
	case WSATYPE_NOT_FOUND: msgText = "WSATYPE_NOT_FOUND"; break;
	case WSAHOST_NOT_FOUND: msgText = "WSAHOST_NOT_FOUND"; break;
	case WSATRY_AGAIN: msgText = "WSATRY_AGAIN"; break;
	case WSANO_RECOVERY: msgText = "WSANO_RECOVERY"; break;
	case WSANO_DATA: msgText = "WSANO_DATA"; break;
	case WSA_INVALID_HANDLE: msgText = "WSA_INVALID_HANDLE"; break;
	case WSA_INVALID_PARAMETER: msgText = "WSA_INVALID_PARAMETER"; break;
	case WSA_IO_INCOMPLETE: msgText = "WSA_IO_INCOMPLETE"; break;
	case WSA_IO_PENDING: msgText = "WSA_IO_PENDING"; break;
	case WSA_NOT_ENOUGH_MEMORY: msgText = "WSA_NOT_ENOUGH_MEMORY"; break;
	case WSA_OPERATION_ABORTED: msgText = "WSA_OPERATION_ABORTED"; break;
	case WSASYSCALLFAILURE: msgText = "WSASYSCALLFAILURE"; break;
	default: msgText = "***ERROR***"; break;
	};
	return msgText;
};

string SetErrorMsgText(string msgText, int code)
{
	return msgText + GetErrorMsgText(code);
};