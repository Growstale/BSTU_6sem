#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <tchar.h>
#include <time.h>
#include "Winsock2.h"
#include <map>
#include <vector>
#pragma comment(lib, "WS2_32.lib")

#pragma comment(lib, "mpr.lib") // Нужно для WNetGetUniversalNameW

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

struct CA
{
	char ipaddr[15];
	char resurce[40];
	char clientID[20];
	enum Status
	{
		NOINIT, // начальное состояние
		INIT, // выполнена инициализация
		ENTER, // выполнен вход в секцию
		LEAVE, // выполнен выход из секции
		WAIT // ожидание входа
	} status;
};

string removeDriveLetter(const string& path) {
	char fullPath[MAX_PATH];
	if (GetFullPathNameA(path.c_str(), MAX_PATH, fullPath, nullptr) == 0) {
		return path; 
	}

	if (fullPath[1] == ':') {
		return std::string(fullPath + 2);
	}
	return string(fullPath);
}

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKET sS;
	WSADATA wsaData;
	vector <CA> resources;
	try
	{
		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = INADDR_ANY;

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) throw SetErrorMsgText("bind:", WSAGetLastError());

		cout << "The coordinator has started his work" << endl;

		while (true)
		{
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int clnt_size = sizeof(clnt);

			int libuf = 0;
			int lobuf = 0;
			CA ibuf;

			if ((libuf = recvfrom(sS, (char*)&ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &clnt_size)) == SOCKET_ERROR) throw SetErrorMsgText("recv:", WSAGetLastError());
			
			if (ibuf.status == ibuf.ENTER || ibuf.status == ibuf.WAIT)
			{
				strcpy(ibuf.resurce, removeDriveLetter(ibuf.resurce).c_str());

				CA* s = nullptr;
				for (auto& res : resources) {
					if (strcmp(res.resurce, ibuf.resurce) == 0) {
						s = &res;
						break;
					}
				}

				if (s != nullptr) {
					ibuf.status = CA::WAIT; // Ресурс занят
				}
				else {
					cout << "Enter in Critical section " << ibuf.clientID << endl;
					resources.push_back(ibuf);
					ibuf.status = CA::ENTER;
				}
			}
			else if (ibuf.status == ibuf.LEAVE)
			{
				auto it = find_if(resources.begin(), resources.end(), [&ibuf](const CA& res) {
					return strcmp(res.resurce, ibuf.resurce) == 0;
				});

				if (it != resources.end()) {
					resources.erase(it);
				}
				cout << "Leave from Critical section " << ibuf.clientID << endl;
			}
			else if (ibuf.status == ibuf.INIT)
			{
				cout << "Init " << ibuf.clientID << endl;
			}
			else if (ibuf.status == ibuf.NOINIT)
			{
				cout << "Close " << ibuf.clientID << endl;
			}

			if ((lobuf = sendto(sS, (char*)&ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, sizeof(clnt))) == SOCKET_ERROR) throw SetErrorMsgText("sendto:", WSAGetLastError());
		}
		if (closesocket(sS) == SOCKET_ERROR) throw SetErrorMsgText("closesocket:", WSAGetLastError());
		if (WSACleanup() == SOCKET_ERROR) throw SetErrorMsgText("Cleanup:", WSAGetLastError());
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
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