#include "Winsock2.h"                
#pragma comment(lib, "WS2_32.lib") 
#include <ws2tcpip.h>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <windows.h>

using namespace std;

string GetErrorMsgText(int code);
string SetErrorMsgText(string msgText, int code);


struct GETSINCHRO	// запрос клиента на синхронизацию счетчика времени
{
    char cmd[5];    // всегда значение SINC
    int curvalue;	// текущее значение счетчика времени
};

int main(int argc, char* argv[])
{
    GETSINCHRO getsinchro, setsinchro;
    strcpy_s(getsinchro.cmd, "SINC");
    int Tc = 1000;
    getsinchro.curvalue = 0;

    if (argc > 1) {
        try {
            Tc = stoi(argv[1]);
        }
        catch (exception& e) {
            cerr << "Invalid tc: " << argv[1] << ". Using default Tc = 1000" << endl;
            Tc = 1000;
        }
    }

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    SOCKET  sS;
    WSADATA wsaData;
    char ip[INET_ADDRSTRLEN];

    try {
        if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
            throw  SetErrorMsgText("startup:", WSAGetLastError());

        if ((sS = socket(AF_INET, SOCK_STREAM, NULL)) == INVALID_SOCKET)
            throw  SetErrorMsgText("socket:", WSAGetLastError());

        SOCKADDR_IN serv;
        serv.sin_family = AF_INET;
        serv.sin_port = htons(2000);

        if (inet_pton(AF_INET, "26.69.212.139", &serv.sin_addr) <= 0)
            throw SetErrorMsgText("inet_pton:", WSAGetLastError());

        if ((connect(sS, (sockaddr*)&serv, sizeof(serv))) == SOCKET_ERROR) {
            int err = WSAGetLastError();
            if (err == WSAECONNABORTED) {
                cout << "Connection terminated" << endl;
                return 0;
            }
            else if (err == WSAECONNREFUSED) {
                cout << "The server rejected or did not accept the connection" << endl;
                return 0;
            }
            else throw SetErrorMsgText("send: ", WSAGetLastError());
        }
        cout << "Connected" << endl;

        int fbuf = 0;
        char firbuf[100];
        snprintf(firbuf, sizeof(firbuf), "sync");
        if ((fbuf = send(sS, firbuf, strlen(firbuf) + 1, NULL)) == SOCKET_ERROR)
            throw  SetErrorMsgText("send:", WSAGetLastError());

        cout << "Client sent to the server: " << firbuf << endl;

        int lobuf = 0, libuf = 0;

        Sleep(100);

        for (int i = 0; i < 10; i++) {

            if ((lobuf = send(sS, (char*)&getsinchro, sizeof(getsinchro), NULL)) == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err == WSAECONNABORTED) {
                    cout << "Connection terminated" << endl;
                    break;
                }
                else if (err == WSAECONNREFUSED) {
                    cout << "The server rejected or did not accept the connection" << endl;
                    break;
                }
                else throw SetErrorMsgText("send: ", WSAGetLastError());
            }

            cout << "Client sent to the server: " << getsinchro.curvalue << endl;

            if ((libuf = recv(sS, (char*)&setsinchro, sizeof(setsinchro), NULL)) == SOCKET_ERROR) {
                int err = WSAGetLastError();
                if (err == WSAECONNABORTED) {
                    cout << "Connection terminated" << endl;
                    break;
                }
                else if (err == WSAECONNREFUSED) {
                    cout << "The server rejected or did not accept the connection" << endl;
                    break;
                }
                else throw SetErrorMsgText("recv: ", WSAGetLastError());
            }
            cout << "Client received from the server: " << setsinchro.curvalue << endl;

            getsinchro.curvalue += setsinchro.curvalue + Tc;

            Sleep(Tc);
        }


        if (closesocket(sS) == SOCKET_ERROR)
            throw  SetErrorMsgText("closesocket:", WSAGetLastError());

        if (WSACleanup() == SOCKET_ERROR)
            throw  SetErrorMsgText("cleanup:", WSAGetLastError());
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
    case WSAEINTR:          msgText = "WSAEINTR: Работа функции прервана";
        break;
    case WSAEACCES:         msgText = "WSAEACCES: Разрешение отвергнуто";
        break;
    case WSAEFAULT:         msgText = "WSAEFAULT: Ошибочный адрес";
        break;
    case WSAEINVAL:         msgText = "WSAEINVAL: Ошибка в аргументе";
        break;
    case WSAEMFILE:         msgText = "WSAEMFILE: Слишком много файлов открыто";
        break;
    case WSAEWOULDBLOCK:    msgText = "WSAEWOULDBLOCK: Ресурс временно недоступен";
        break;
    case WSAEINPROGRESS:    msgText = "WSAEINPROGRESS: Операция в процессе развития";
        break;
    case WSAEALREADY:       msgText = "WSAEALREADY: Операция уже выполняется";
        break;
    case WSAENOTSOCK:       msgText = "WSAENOTSOCK: Сокет задан неправильно";
        break;
    case WSAEDESTADDRREQ:   msgText = "WSAEDESTADDRREQ: Требуется адрес расположения";
        break;
    case WSAEMSGSIZE:       msgText = "WSAEMSGSIZE: Сообщение слишком длинное";
        break;
    case WSAEPROTOTYPE:     msgText = "WSAEPROTOTYPE: Неправильный тип протокола для сокета";
        break;
    case WSAENOPROTOOPT:    msgText = "WSAENOPROTOOPT: Ошибка в опции протокола";
        break;
    case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT: Протокол не поддерживается";
        break;
    case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT: Тип сокета не поддерживается";
        break;
    case WSAEOPNOTSUPP:     msgText = "WSAEOPNOTSUPP: Операция не поддерживается";
        break;
    case WSAEPFNOSUPPORT:   msgText = "WSAEPFNOSUPPORT: Тип протоколов не поддерживается";
        break;
    case WSAEAFNOSUPPORT:   msgText = "WSAEAFNOSUPPORT: Тип адресов не поддерживается протоколом";
        break;
    case WSAEADDRINUSE:     msgText = "WSAEADDRINUSE: Адрес уже используется";
        break;
    case WSAEADDRNOTAVAIL:  msgText = "WSAEADDRNOTAVAIL: Запрошенный адрес не может быть использован";
        break;
    case WSAENETDOWN:       msgText = "WSAENETDOWN: Сеть отключена";
        break;
    case WSAENETUNREACH:    msgText = "WSAENETUNREACH: Сеть не достижима";
        break;
    case WSAENETRESET:      msgText = "WSAENETRESET: Сеть разорвала соединение";
        break;
    case WSAECONNABORTED:   msgText = "WSAECONNABORTED: Программный отказ связи";
        break;
    case WSAECONNRESET:     msgText = "WSAECONNRESET: Связь восстановлена";
        break;
    case WSAENOBUFS:        msgText = "WSAENOBUFS: Не хватает памяти для буферов";
        break;
    case WSAEISCONN:        msgText = "WSAEISCONN: Сокет уже подключен";
        break;
    case WSAENOTCONN:       msgText = "WSAENOTCONN: Сокет не подключен";
        break;
    case WSAESHUTDOWN:      msgText = "WSAESHUTDOWN: Нельзя выполнить send: сокет завершил работу";
        break;
    case WSAETIMEDOUT:      msgText = "WSAETIMEDOUT: Закончился отведенный интервал времени";
        break;
    case WSAECONNREFUSED:   msgText = "WSAECONNREFUSED: Соединение отклонено";
        break;
    case WSAEHOSTDOWN:      msgText = "WSAEHOSTDOWN: Хост в неработоспособном состоянии";
        break;
    case WSAEHOSTUNREACH:   msgText = "WSAEHOSTUNREACH: Нет маршрута для хоста";
        break;
    case WSAEPROCLIM:       msgText = "WSAEPROCLIM: Слишком много процессов";
        break;
    case WSASYSNOTREADY:    msgText = "WSASYSNOTREADY: Сеть не доступна";
        break;
    case WSAVERNOTSUPPORTED: msgText = "WSAVERNOTSUPPORTED: Данная версия недоступна";
        break;
    case WSANOTINITIALISED: msgText = "WSANOTINITIALISED: Не выполнена инициализация WS2_32.DLL";
        break;
    case WSAEDISCON:        msgText = "WSAEDISCON: Выполняется отключение";
        break;
    case WSATYPE_NOT_FOUND: msgText = "WSATYPE_NOT_FOUND: Класс не найден";
        break;
    case WSAHOST_NOT_FOUND: msgText = "WSAHOST_NOT_FOUND: Хост не найден";
        break;
    case WSATRY_AGAIN:      msgText = "WSATRY_AGAIN: Неавторизированный хост не найден";
        break;
    case WSANO_RECOVERY:    msgText = "WSANO_RECOVERY: Неопределенная ошибка";
        break;
    case WSANO_DATA:        msgText = "WSANO_DATA: Нет записи запрошенного типа";
        break;
    case WSA_INVALID_HANDLE: msgText = "WSA_INVALID_HANDLE: Указанный дескриптор события с ошибкой";
        break;
    case WSA_INVALID_PARAMETER: msgText = "WSA_INVALID_PARAMETER: Один или более параметров с ошибкой";
        break;
    case WSA_IO_INCOMPLETE: msgText = "WSA_IO_INCOMPLETE: Объект ввода-вывода не в сигнальном состоянии";
        break;
    case WSA_IO_PENDING:    msgText = "WSA_IO_PENDING: Операция завершится позже";
        break;
    case WSA_NOT_ENOUGH_MEMORY: msgText = "WSA_NOT_ENOUGH_MEMORY: Не достаточно памяти";
        break;
    case WSA_OPERATION_ABORTED: msgText = "WSA_OPERATION_ABORTED: Операция отвергнута";
        break;
    case WSASYSCALLFAILURE: msgText = "WSASYSCALLFAILURE: Аварийное завершение системного вызова";
        break;
    default:                msgText = "***ERROR***";        break;
    };
    return msgText;
};

string SetErrorMsgText(string msgText, int code)
{
    return  msgText + GetErrorMsgText(code);
};