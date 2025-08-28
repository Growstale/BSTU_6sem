#pragma once
#include "pch.h"
#include "EchoServer.h"

DWORD WINAPI SynchroServer(LPVOID pPrm) {
    cout << "SynchroServer is working" << endl;
    int request_number = 1;
    int average_correction[100];
    int sum_corretion;

    SETSINCRO setsinchro, getsinchro;
    strcpy_s(setsinchro.cmd, "SINC");
    Contact* client = (Contact*)pPrm;
    char ibuf[100];
    memset(ibuf, 0, sizeof(ibuf));

    char Error[15] = "ErrorInquiry", Start[20] = "Start Message";

    int recv_len;
    try
    {
        QueueUserAPC(ASStartMessage, client->hAcceptServer, (ULONG_PTR)_strdup(client->srvname));

        bool work = false;
        while (!client->TimerOff && request_number != 100) {
            client->sthread = Contact::WORK;

            if ((recv(client->s, (char*)&getsinchro, sizeof(getsinchro), NULL)) == SOCKET_ERROR) {
                if (WSAGetLastError() == WSAEWOULDBLOCK) {
                    Sleep(100);
                }
                else if (WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAECONNRESET) {
                    client->sthread = Contact::FINISH;
                    break;
                }
                else throw  SetErrorMsgText("EchoServer Recv:", WSAGetLastError());
            }

            else {
                if (client->TimerOff) break;

                setsinchro.correction = GetTimeValue() - getsinchro.correction;

                if ((send(client->s, (char*)&setsinchro, sizeof(setsinchro), NULL)) == SOCKET_ERROR) {
                    if (WSAGetLastError() == WSAECONNABORTED || WSAGetLastError() == WSAECONNRESET) {
                        client->sthread = Contact::FINISH;
                        break;
                    }
                    else throw  SetErrorMsgText("EchoServer Send:", WSAGetLastError());
                }

                sum_corretion = 0;

                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(client->prms.sin_addr), clientIP, sizeof(clientIP));
                if (request_number != 1) {
                    average_correction[request_number - 2] = setsinchro.correction;
                    for (int i = 0; i < request_number - 1; i++) {
                        sum_corretion += average_correction[i];
                    }
                }


                cout << "==========CORRECTION==========" << endl;
                cout << "Client IP: " << clientIP << endl;
                cout << "Correction: " << setsinchro.correction << endl;
                cout << "Request #" << request_number << endl;
                if (request_number == 1) cout << "Average correction: -" << endl;
                else
                    cout << "Average correction: " << sum_corretion/(request_number - 1) << endl;
                cout << "==============================" << endl;

                request_number++;
            }
        }

        QueueUserAPC(ASFinishMessage, client->hAcceptServer, (ULONG_PTR)_strdup(client->srvname));
        client->sthread = Contact::FINISH;
        CancelWaitableTimer(client->htimer);
    }
    catch (string errorMsgText)
    {
        client->sthread = Contact::ABORT;
        CancelWaitableTimer(client->htimer);
        cout << endl << errorMsgText;
    }
    DWORD rc = 0;
    ExitThread(rc);
}