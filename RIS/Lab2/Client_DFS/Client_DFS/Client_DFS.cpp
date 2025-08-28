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
const size_t BUFFER_SIZE = 1024;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

#pragma pack(push, 1)
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

	char content[BUFFER_SIZE];
	enum COMMAND
	{
		NOCOMMAND,
		READ,
		WRITE,
		LIST,
		DROP
	} command;

};
#pragma pack(pop)



int main()
{
	CA s;
	string ipAddress = "26.69.212.139";
	string resource = "R:\\test.txt";
	string clientID = "client";

	CA ca;
	SOCKET cC;
	WSADATA wsaData;

	try {
		cout << "Enter your username" << endl;
		cin >> clientID;
		cin.ignore();
		CA ca = {};
		ca.status = ca.INIT;
		ca.command = ca.NOCOMMAND;
		strcpy(ca.clientID, const_cast<char*>(clientID.c_str())); 
		strcpy(ca.ipaddr, const_cast<char*>(ipAddress.c_str()));

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((cC = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = inet_addr(ca.ipaddr);

		int lobuf = 0;
		int libuf = 0;
		int serv_size = sizeof(serv);

		if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
		if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
		
		if (ca.status != ca.INIT) return 0;

		while (true) {
			if (ca.status == ca.LEAVE) ca.status = ca.INIT;
			int c = 0;
			cout << "1. READ" << endl <<
					"2. WRITE" << endl <<
					"3. LIST" << endl <<
					"4. DELETE" << endl <<
					"0. EXIT" << endl;

			cin >> c;

			switch (c)
			{
			case 1: {
				cout << "Enter file name" << endl;
				cin >> resource;
				cin.ignore();
				ca.command = ca.READ;
				strcpy(ca.resurce, const_cast<char*>(resource.c_str()));
				if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
				if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
				cout << ca.content << endl;
				break;
			}
			case 2: {
				string content;
				cout << "Enter file name" << endl;
				cin >> resource;
				cin.ignore();
				cout << "Enter content" << endl;
				getline(cin, content);

				ca.command = ca.WRITE;
				strcpy(ca.resurce, const_cast<char*>(resource.c_str()));
				strcpy(ca.content, const_cast<char*>(content.c_str()));

				if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
				if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
				cout << ca.content << endl;
				break;
			}
			case 3: {
				ca.command = ca.LIST;
				strcpy(ca.resurce, const_cast<char*>(resource.c_str()));
				if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
				if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
				cout << ca.content << endl;
				break;
			}
			case 4: {
				cout << "Enter file name" << endl;
				cin >> resource;
				cin.ignore();
				ca.command = ca.DROP;
				strcpy(ca.resurce, const_cast<char*>(resource.c_str()));
				if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
				if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
				cout << ca.content << endl;
				break;
			}
			case 0: {
				ca.command = ca.NOCOMMAND;
				ca.status = ca.NOINIT;
				strcpy(ca.resurce, const_cast<char*>(resource.c_str()));
				if ((lobuf = sendto(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
				if ((libuf = recvfrom(cC, (char*)&ca, sizeof(ca), NULL, (sockaddr*)&serv, &serv_size)) == SOCKET_ERROR) throw SetErrorMsgText("recvfrom 1:", WSAGetLastError());
				cout << ca.content << endl;
				return 0;
				break;
			}
			default:
				cout << "Wrong command" << endl;
				break;
			}
		}

	}
	catch (string errorMsgText)
	{
		cout << errorMsgText << endl;
	}

	system("pause");
	return 0;
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