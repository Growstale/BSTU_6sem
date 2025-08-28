#include "ConcurrentServer.h"
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>


DWORD WINAPI ResponseServer(LPVOID pPrm) {
	DWORD rc = 0;
	SOCKADDR_IN client;  
	int clientsockaddrlen = sizeof(client);
	cout << "ResponseServer start\n";
	try
	{
		if (OpenBroadcastPort(udpport) == 0) {

			while (*((TalkersCommand*)pPrm) != EXIT) {
				char ibuf[50];
				char obuf[100];
				int  libuf = 0;

				memset(ibuf, 0, sizeof(ibuf));
				for (auto& it : broadcastPortMap) {

					if ((libuf = recvfrom(it.second, ibuf, sizeof(ibuf) - 1, NULL, (LPSOCKADDR)&client, &clientsockaddrlen)) == SOCKET_ERROR) {
						int err = WSAGetLastError();
						if (err == WSAEWOULDBLOCK) Sleep(1000);
						else throw SetErrorMsgText("Recv:", WSAGetLastError());
					}
					if (libuf > 0) {
						if (strcmp(ibuf, servercall) == 0) {
							if ((libuf = sendto(it.second, ibuf, strlen(ibuf) + 1, NULL, (LPSOCKADDR)&client, sizeof(client))) == SOCKET_ERROR)
								throw SetErrorMsgText("Sendto:", WSAGetLastError());
						}
					}
				}
			}
		}
	}
	catch (string errorMsgText) {
		cout << errorMsgText << endl;
	}

	for (auto& it : broadcastPortMap) {
		ClosePort(it.first);
	}

	cout << "ResponseServer is stopped\n" << endl;
	ExitThread(rc);
}


int OpenBroadcastPort(int port) {

	if (broadcastPortMap.find(port) != broadcastPortMap.end()) {
		return 1;
	}

	SOCKET serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		cout << "Socket creation failed on port " << port << endl;
		return 2;
	}

	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(port);
	serverAddr.sin_addr.s_addr = INADDR_ANY;

	u_long nonblk;
	if (ioctlsocket(serverSocket, FIONBIO, &(nonblk = 1)) == SOCKET_ERROR) {
		cout << "Ioctlsocket failed on port " << port << " (broadcast)" << endl;
		closesocket(serverSocket);
		return 2;
	}

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cout << "Bind failed on port " << port << " (broadcast)" << endl;
		closesocket(serverSocket);
		return 2;
	}

	broadcastPortMap[port] = serverSocket;

	cout << "Port " << port << " (broadcast) is now open and accepting connections" << endl;
	return 0;
}

int CloseBroadcastPort(int port) {
	auto it = broadcastPortMap.find(port);
	if (it == broadcastPortMap.end()) {
		return 1;
	}

	if (broadcastPortMap.size() == 1) {
		return 2;
	}

	closesocket(it->second);
	broadcastPortMap.erase(it);
	cout << "Port " << port << " (broadcast) has been closed and freed" << endl;
	return 0;
}


