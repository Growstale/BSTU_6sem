#include "ConcurrentServer.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>

bool AcceptCycle(int squirt) {
    bool rc = false;
    Contact contact(Contact::ACCEPT, "EchoServer");
    while (squirt-- > 0 && rc == false) {
        EnterCriticalSection(&ListContactCriticalSection);
        {
            lock_guard<mutex> lock(portMapMutex);
            for (auto& it : portMap) {
                contact.s = accept(it.second, (sockaddr*)&contact.prms, &contact.lprms);
                if (contact.s != INVALID_SOCKET) {
                    rc = true;
                    contact.hAcceptServer = hAcceptServer;
                    Contacts.push_front(contact);
                    InterlockedIncrement(&CurrentClients);
                    InterlockedIncrement(&TotalClients);
                    SetEvent(AcceptEvent);
                    cout << "New connection accepted on port " << it.first << endl;
                    break;
                }
                else {
                    int err = WSAGetLastError();
                    if (err == WSAECONNRESET || err == WSAECONNABORTED) {
                        cout << "Client disconnected or aborted connection" << endl;
                        break;
                    }
                    else if (err != WSAEWOULDBLOCK) throw SetErrorMsgText("accept: ", WSAGetLastError());
                }
            }
        }
        LeaveCriticalSection(&ListContactCriticalSection);
        if (!rc) {
            Sleep(100);
        }
    }
    return rc;
}

void CommandsCycle(TalkersCommand& cmd)
{
    int squirt = 0;
    while (cmd != EXIT)
    {
        switch (cmd) {
        case START:
            cout << "start" << endl;
            cmd = GETCOMMAND;
            squirt = AS_SQUIRT;
            break;
        case STOP:
            cout << "stop" << endl;
            cmd = GETCOMMAND;
            squirt = 0;
            break;
        case WAIT:
            squirt = 0;
            WaitClients();
            cmd = START;
            break;
        case SHUTDOWN:
            squirt = 0;
            WaitClients();
            cmd = EXIT;
            break;        
        }
        if (cmd != EXIT) {
            if (AcceptCycle(squirt))
            {
                cmd = GETCOMMAND;
            }
            else SleepEx(0, TRUE);
        }
    };
}

DWORD WINAPI AcceptServer(LPVOID pPrm)
{
    WSADATA wsaData;

    try {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw  SetErrorMsgText("startup:", WSAGetLastError());

        if (OpenPort(port) == 0) {

            TalkersCommand* command = (TalkersCommand*)pPrm;
            CommandsCycle(*((TalkersCommand*)command));
        }

        // «акрытие всех портов перед завершением
        lock_guard<mutex> lock(portMapMutex);
        for (auto& it : portMap) {
            ClosePort(it.first);
        }

        if (WSACleanup() == SOCKET_ERROR)
            throw  SetErrorMsgText("cleanup:", WSAGetLastError());
    }
    catch (string errorMsgText)
    {
        cout << endl << errorMsgText;
    }

    DWORD rc = 0;
    ExitThread(rc);
}

void WaitClients() {
    bool EveryoneIsServed = false;
    while (!EveryoneIsServed) {
        EnterCriticalSection(&ListContactCriticalSection);
        EveryoneIsServed = Contacts.empty();
        LeaveCriticalSection(&ListContactCriticalSection);
        SleepEx(0, TRUE);
    }
}

int OpenPort(int port) {
    lock_guard<mutex> lock(portMapMutex);

    if (portMap.find(port) != portMap.end()) {
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
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
        cout << "Ioctlsocket failed on port " << port << endl;
        closesocket(serverSocket);
        return 2;
    }

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind failed on port " << port << endl;
        closesocket(serverSocket);
        return 2;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cout << "Listen failed on port " << port << endl;
        closesocket(serverSocket);
        return 2;
    }

    portMap[port] = serverSocket;

    cout << "Port " << port << " is now open and accepting connections" << endl;
    return 0;
}

int ClosePort(int port) {
    lock_guard<mutex> lock(portMapMutex);

    auto it = portMap.find(port);
    if (it == portMap.end()) {
        return 1;
    }

    if (portMap.size() == 1) {
        return 2;
    }

    EnterCriticalSection(&ListContactCriticalSection);

    for (auto itc = Contacts.begin(); itc != Contacts.end(); ++itc) {
        sockaddr_in peerAddr;
        int addrLen = sizeof(peerAddr);
        if (getsockname(itc->s, (sockaddr*)&peerAddr, &addrLen) == SOCKET_ERROR) {
            cout << "Error getpeername: " << WSAGetLastError() << endl;
        }
        else {
            char ipStr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &peerAddr.sin_addr, ipStr, sizeof(ipStr));

            if (itc->sthread == Contact::WORK && ntohs(peerAddr.sin_port) == it->first) {
                itc->sthread = Contact::FINISH;
                cout << "Contact " << itc->srvname << " was aborted because the administrator closed the port" << endl;
            }
        }
    }

    LeaveCriticalSection(&ListContactCriticalSection);
    Sleep(1000);
    closesocket(it->second);
    portMap.erase(it);
    cout << "Port " << port << " has been closed and freed" << endl;
    return 0;
}


