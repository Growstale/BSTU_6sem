#include "ConcurrentServer.h"

DWORD WINAPI DispatchServer(LPVOID pPrm) {
	cout << "DispathServer started working" << endl;
	char buffer[1024], Error[15] = "ErrorInquiry";
	int bytesReceived, bytesSent;

	try {
		Contact* client = NULL;
		while (*((TalkersCommand*)pPrm) != EXIT) {
			if (WaitForSingleObject(AcceptEvent, 100) == WAIT_OBJECT_0) {
				EnterCriticalSection(&ListContactCriticalSection);
				for (auto it = Contacts.begin(); it != Contacts.end(); it++) {
					if (it->type == Contact::ACCEPT)
					{
						client = &(*it);
						bool flag = false;
						while (flag == false) {
							if ((bytesReceived = recv(client->s, buffer, sizeof(buffer) - 1, 0)) == SOCKET_ERROR) {
								if (WSAGetLastError() == WSAEWOULDBLOCK) {
									Sleep(100);
								}
								else if (WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAECONNRESET || WSAGetLastError() == WSAENOTSOCK) {
									closesocket(client->s);
									client->s = INVALID_SOCKET;
									client->sthread = Contact::FINISH;
									break;
								}
								else {
									cerr << SetErrorMsgText("recv failed: ", GetLastError()) << endl;
									break;
								}
							}
							else flag = true;
						}
						if (client && client->s == INVALID_SOCKET) {
							client->sthread = Contact::FINISH;
							break; 
						}
						if (flag)
						{
							buffer[bytesReceived] = '\0';
							cout << "\tMessage from client: " << buffer << endl;
							client->type = Contact::CONTACT;
							client->htimer = CreateWaitableTimer(NULL, false, NULL);
							if (client->htimer == NULL) {
								cerr << SetErrorMsgText("CreateWaitableTimer failed", GetLastError()) << endl;
							}
							_int64 time = -3000000000;
							if (!SetWaitableTimer(client->htimer, (LARGE_INTEGER*)&time, 0, ASWTimer, client, false)) {
								cerr << SetErrorMsgText("SetWaitableTimer failed", GetLastError()) << endl;
								CloseHandle(client->htimer);
								client->htimer = NULL;
							}
							strcpy_s(client->srvname, buffer);

							client->hthread = sss(buffer, client);
							if (!client->hthread) {
								flag = false;
							}
						}
						if (!flag) {
							InterlockedIncrement(&RejectedClients);
							if ((bytesSent = send(client->s, Error, sizeof(Error), NULL)) == SOCKET_ERROR)
								throw SetErrorMsgText("Send: ", WSAGetLastError());
							if (client->htimer != NULL) {
								if (!CancelWaitableTimer(client->htimer)) {
									cerr << SetErrorMsgText("CancelWaitableTimer failed", GetLastError()) << endl;
									CloseHandle(client->htimer);
									client->htimer = NULL;
								}
							}
							client->sthread = Contact::ABORT;
							closesocket(client->s);
						}
					}
				}
				LeaveCriticalSection(&ListContactCriticalSection);
			}
			SleepEx(0, TRUE);
		}
	}
	catch (string errorMsgText)
	{
		cout << endl << errorMsgText;
	}
	DWORD rc = 0;
	ExitThread(rc);
}

void CALLBACK ASWTimer(LPVOID Lprm, DWORD, DWORD) {
	char obuf[50] = "Close timer";
	Contact* client = (Contact*)Lprm;
	char* name = client->srvname;
	EnterCriticalSection(&ListContactCriticalSection);

	if ((send(client->s, obuf, sizeof(obuf), NULL)) == SOCKET_ERROR) throw  SetErrorMsgText("Send:", WSAGetLastError());

	client->TimerOff = true;
	client->sthread = Contact::TIMEOUT;
	TerminateThread(client->hthread, 0);
	LeaveCriticalSection(&ListContactCriticalSection);

	SYSTEMTIME stt;
	GetLocalTime(&stt);
	printf("%d.%d.%d %d:%02d %s timeout\n", stt.wDay, stt.wMonth, stt.wYear, stt.wHour, stt.wMinute, name);
	TimeoutClients++;
}