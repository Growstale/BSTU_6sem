#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <string>
#include <tchar.h>
#include <time.h>
#include "Winsock2.h"
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <thread>
#include <mutex>

#pragma comment(lib, "WS2_32.lib")

using namespace std;
const size_t BUFFER_SIZE = 1024;  

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);

#pragma pack(push, 1)
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
	char content[BUFFER_SIZE];
	enum Command
	{
		NOCOMMAND,
		READ,
		WRITE,
		LIST,
		DROP
	} command;
};
#pragma pack(pop)

const std::string DIRECTORY = "C:\\MyAppDirectory";


void CreateDirectoryOnce() {
	CreateDirectoryA(DIRECTORY.c_str(), NULL);
}

void CreateOrAppendFile(const string& filename, const string& content) {
	string filepath = DIRECTORY + "\\" + filename;
	HANDLE hFile = CreateFileA(filepath.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		cout << "Error opening file " << filename << endl;
		return;
	}
	DWORD bytesWritten;
	for (int i = 0; i < 10; i++) {
		WriteFile(hFile, content.c_str(), content.size(), &bytesWritten, NULL);
		Sleep(1000);
	}
	CloseHandle(hFile);
}

string ReadFileContent(const string& filename) {
	string filepath = DIRECTORY + "\\" + filename;
	HANDLE hFile = CreateFileA(filepath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return "Error opening file";
	}
	char buffer[1024] = { 0 };
	DWORD bytesRead;
	ReadFile(hFile, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
	CloseHandle(hFile);
	return string(buffer, bytesRead);
}

void DeleteFileByNameA(const string& filename) {
	string filepath = DIRECTORY + "\\" + filename;
	DeleteFileA(filepath.c_str());
}

string ListDirectoryContents(const string& directory) {
	string result = "";
	wstring searchPath = wstring(directory.begin(), directory.end()) + L"\\*";
	WIN32_FIND_DATAW findFileData;
	HANDLE hFind = FindFirstFileW(searchPath.c_str(), &findFileData);

	if (hFind == INVALID_HANDLE_VALUE) {
		return "";
	}

	do {
		if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			// Конвертация wide-строки в обычную строку
			int sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, findFileData.cFileName, -1, NULL, 0, NULL, NULL);
			std::string fileName(sizeNeeded - 1, '\0');
			WideCharToMultiByte(CP_UTF8, 0, findFileData.cFileName, -1, &fileName[0], sizeNeeded, NULL, NULL);

			result += fileName + "\n";
		}
	} while (FindNextFileW(hFind, &findFileData));

	FindClose(hFind);
	return result;
}

void handleClientRequest(SOCKET sS, CA ibuf, SOCKADDR_IN clnt, vector<CA>& resources) {

	if (ibuf.status == ibuf.INIT && ibuf.command == ibuf.NOCOMMAND) {
		cout << "Init " << ibuf.clientID << endl;
	}

	else if (ibuf.status == ibuf.INIT && ibuf.command == ibuf.READ) {
		while (ibuf.status != CA::LEAVE) {
			CA* s = nullptr;
			for (auto& res : resources) {
				if (strcmp(res.resurce, ibuf.resurce) == 0) {
					s = &res;
					break;
				}
			}

			if (s != nullptr) {
				ibuf.status = CA::WAIT;
			}
			else {
				cout << "Enter in Critical section " << ibuf.clientID << endl;
				resources.push_back(ibuf);
				ibuf.status = CA::ENTER;
				string result = ReadFileContent(ibuf.resurce);
				cout << "Client " << ibuf.clientID << " read file " << ibuf.resurce << endl;
				if (result.length() < BUFFER_SIZE) {
					strcpy(ibuf.content, result.c_str());
				}
				else {
					cout << "Error: file " << ibuf.resurce << " is too big for the buffer" << endl;
				}
				ibuf.status = CA::LEAVE;
			}
			if (ibuf.status != CA::LEAVE) Sleep(2000);
		}
	}
	else if (ibuf.status == ibuf.INIT && ibuf.command == ibuf.LIST) {
		::string result = ListDirectoryContents(DIRECTORY);

		if (result.length() < BUFFER_SIZE) {
			strcpy(ibuf.content, result.c_str());
		}
		else {
			strcpy(ibuf.content, "File is too big for the buffer");
		}
		cout << "Client " << ibuf.clientID << " read directory " << endl;
	}
	else if (ibuf.status == ibuf.INIT && ibuf.command == ibuf.DROP) {
		CA* s = nullptr;
		for (auto& res : resources) {
			if (strcmp(res.resurce, ibuf.resurce) == 0) {
				s = &res;
				break;
			}
		}

		if (s != nullptr) {
			strcpy(ibuf.content, "This file is busy, you can't delete it");
		}
		else {
			DeleteFileByNameA(ibuf.resurce);
			cout << "Client " << ibuf.clientID << " delete file " << ibuf.resurce << endl;
			strcpy(ibuf.content, "You deleted file");
		}
	}
	else if (ibuf.status == ibuf.INIT && ibuf.command == ibuf.WRITE) {
		while (ibuf.status != CA::LEAVE) {
			CA* s = nullptr;
			for (auto& res : resources) {
				if (strcmp(res.resurce, ibuf.resurce) == 0) {
					s = &res;
					break;
				}
			}

			if (s != nullptr) {
				ibuf.status = CA::WAIT;
			}
			else {
				cout << "Enter in Critical section " << ibuf.clientID << endl;
				resources.push_back(ibuf);
				ibuf.status = CA::ENTER;
				CreateOrAppendFile(ibuf.resurce, ibuf.content);
				strcpy(ibuf.content, "You entered the information");
				ibuf.status = CA::LEAVE;
				cout << "Client " << ibuf.clientID << " write file " << ibuf.resurce << endl;
			}
			if (ibuf.status != CA::LEAVE) Sleep(2000);
		}
	}
	else if (ibuf.status == ibuf.NOINIT)
	{
		cout << "Close " << ibuf.clientID << endl;
		strcpy(ibuf.content, "You are noinit");
	}

	if (ibuf.status == ibuf.LEAVE)
	{
		auto it = find_if(resources.begin(), resources.end(), [&ibuf](const CA& res) {
			return strcmp(res.resurce, ibuf.resurce) == 0;
			});

		if (it != resources.end()) {
			resources.erase(it);
		}
		cout << "Leave from Critical section " << ibuf.clientID << endl;
	}

	int lobuf;
	int clnt_size = sizeof(clnt);
	if ((lobuf = sendto(sS, (char*)&ibuf, sizeof(ibuf), 0, (sockaddr*)&clnt, clnt_size)) == SOCKET_ERROR) {
		cerr << "Error when sending data to the client!" << endl;
	}
}

int main()
{
	SOCKET sS;
	WSADATA wsaData;
	vector <CA> resources;
	try
	{
		CreateDirectoryOnce();

		if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0) throw SetErrorMsgText("startup:", WSAGetLastError());
		if ((sS = socket(AF_INET, SOCK_DGRAM, NULL)) == INVALID_SOCKET) throw SetErrorMsgText("socket:", WSAGetLastError());

		SOCKADDR_IN serv;
		serv.sin_family = AF_INET;;
		serv.sin_port = htons(2000);
		serv.sin_addr.s_addr = INADDR_ANY;

		if (bind(sS, (LPSOCKADDR)&serv, sizeof(serv)) == SOCKET_ERROR) throw SetErrorMsgText("bind:", WSAGetLastError());

		cout << "The server has started his work" << endl;

		while (true)
		{
			SOCKADDR_IN clnt;
			memset(&clnt, 0, sizeof(clnt));
			int clnt_size = sizeof(clnt);

			int libuf = 0;
			int lobuf = 0;
			CA ibuf;

			if ((libuf = recvfrom(sS, (char*)&ibuf, sizeof(ibuf), NULL, (sockaddr*)&clnt, &clnt_size)) == SOCKET_ERROR) throw SetErrorMsgText("recv:", WSAGetLastError());

			std::thread clientThread(handleClientRequest, sS, ibuf, clnt, std::ref(resources));
			clientThread.detach(); // Поток работает независимо
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