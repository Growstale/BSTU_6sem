#include <tchar.h>
#include "iostream"
#include "Windows.h"                
#include "ctime"
#include <thread>
#include <string>
using std::string;
using namespace std;

string SetPipeError(string msgText, int code);
string GetPipeError(int code);
bool IsServerConnected(HANDLE hPipe);
void MonitorServerConnection(HANDLE hPipe, atomic<bool>& stopFlag);


int main(int argc, _TCHAR* argv[]) {
	setlocale(LC_ALL, "Russian");

	char ReadBuf[200] = "", WriteBuf[30] = "";
	DWORD nBytesRead, nBytesWrite;

	int command = 0;

    atomic<bool> stopFlag(false); // Флаг для завершения потока

	try
	{
        cout << "\n ---------- Доступные команды ---------- \n";
        cout << "1 - start\tразрешить подключение клиентов к серверу\n";
        cout << "2 - stop\tзапретить подключение клиентов к серверу\n";
        cout << "3 - exit\tзавершить работу сервера\n";
        cout << "4 - statistics\tвывод статистики\n";
        cout << "5 - wait\tприостановить подключение клиентов, пока не обслужится последний клиент, подключенный к серверу\n";
        cout << "6 - shutdown\tравносильна последовательности команд: wait, exit\n";
        cout << "OPEN_ACCEPT\tподключить порт\n";
        cout << "CLOSE_ACCEPT\tотключить порт\n";
        cout << "OPEN_ACCEPT_BROADCAST\tподключить порт (широковещание)\n";
        cout << "CLOSE_ACCEPT_BROADCAST\tотключить порт (широковещание)\n";
        cout << "0 - выход из консоли\n";
        cout << "\n ----------------------------------------- \n";

        HANDLE hNamedPipe;
        wstring  pipename;
        wchar_t name[512];
        cout << "Введите имя сервера: ";
        wcin >> pipename;
        cin.ignore();

        swprintf_s(name, sizeof(name) / sizeof(wchar_t), L"\\\\.\\pipe\\%s", pipename.c_str());

        wcout << L"Подключение к каналу: " << name << endl;

            
        if ((hNamedPipe = CreateFile(name,
            GENERIC_READ | GENERIC_WRITE,
            NULL,
            NULL,
            OPEN_EXISTING,
            NULL,
            NULL)) == INVALID_HANDLE_VALUE)
        {
            throw SetPipeError("createfile:", GetLastError());
        }

        // Запуск потока для проверки соединения
        thread monitorThread(MonitorServerConnection, hNamedPipe, ref(stopFlag));


        while (!stopFlag) {
            cout << "Введите команду..." << endl;
            string command;
            getline(cin, command);

            if (command.empty()) {
                continue;
            }

            if (command == "0") {
                break;
            }

            bool isNumber = true;
            for (char c : command) {
                if (!isdigit(c)) {
                    isNumber = false;
                    break;
                }
            }

            if (isNumber) {
                int num = stoi(command);
                if (num >= 1 && num <= 6) {
                    sprintf_s(WriteBuf, "%s", command.c_str());
                    if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
                        throw "WriteFile error ";
                    }
                    if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) {
                        throw "ReadFile error";
                    }
                    cout << ReadBuf << endl;
                    continue;
                }
            }

            if (command.length() > 11 && command.substr(0, 11) == "OPEN_ACCEPT") {
                string portStr = command.substr(11);
                portStr.erase(0, portStr.find_first_not_of(" "));
                portStr.erase(portStr.find_last_not_of(" ") + 1);

                bool isPortNumber = true;
                for (char c : portStr) {
                    if (!isdigit(c)) {
                        isPortNumber = false;
                        break;
                    }
                }

                if (isPortNumber && !portStr.empty()) {
                    int port = stoi(portStr);

                    sprintf_s(WriteBuf, "%s", command.c_str());
                    if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
                        throw "WriteFile error ";
                    }
                    if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) {
                        throw "ReadFile error";
                    }
                    cout << ReadBuf << endl;

                    continue;
                }
            }

            if (command.length() > 12 && command.substr(0, 12) == "CLOSE_ACCEPT") {
                string portStr = command.substr(12);
                portStr.erase(0, portStr.find_first_not_of(" "));
                portStr.erase(portStr.find_last_not_of(" ") + 1);

                bool isPortNumber = true;
                for (char c : portStr) {
                    if (!isdigit(c)) {
                        isPortNumber = false;
                        break;
                    }
                }

                if (isPortNumber && !portStr.empty()) {
                    int port = stoi(portStr);

                    sprintf_s(WriteBuf, "%s", command.c_str());
                    if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
                        throw "WriteFile error ";
                    }
                    if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) {
                        throw "ReadFile error";
                    }
                    cout << ReadBuf << endl;

                    continue;
                }
            }

            if (command.length() > 21 && command.substr(0, 21) == "OPEN_ACCEPT_BROADCAST") {
                string portStr = command.substr(21);
                portStr.erase(0, portStr.find_first_not_of(" "));
                portStr.erase(portStr.find_last_not_of(" ") + 1);

                bool isPortNumber = true;
                for (char c : portStr) {
                    if (!isdigit(c)) {
                        isPortNumber = false;
                        break;
                    }
                }

                if (isPortNumber && !portStr.empty()) {
                    int port = stoi(portStr);

                    sprintf_s(WriteBuf, "%s", command.c_str());
                    if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
                        throw "WriteFile error ";
                    }
                    if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) {
                        throw "ReadFile error";
                    }
                    cout << ReadBuf << endl;

                    continue;
                }
            }

            if (command.length() > 22 && command.substr(0, 22) == "CLOSE_ACCEPT_BROADCAST") {
                string portStr = command.substr(22);
                portStr.erase(0, portStr.find_first_not_of(" "));
                portStr.erase(portStr.find_last_not_of(" ") + 1);

                bool isPortNumber = true;
                for (char c : portStr) {
                    if (!isdigit(c)) {
                        isPortNumber = false;
                        break;
                    }
                }

                if (isPortNumber && !portStr.empty()) {
                    int port = stoi(portStr);

                    sprintf_s(WriteBuf, "%s", command.c_str());
                    if (!WriteFile(hNamedPipe, WriteBuf, strlen(WriteBuf) + 1, &nBytesWrite, NULL)) {
                        throw "WriteFile error ";
                    }
                    if (!ReadFile(hNamedPipe, ReadBuf, sizeof(ReadBuf), &nBytesRead, NULL)) {
                        throw "ReadFile error";
                    }
                    cout << ReadBuf << endl;

                    continue;
                }
            }

            cout << "Неправильная команда. Введите число от 1 до 6, команду в формате OPEN_ACCEPT/CLOSE_ACCEPT/OPEN_ACCEPT_BROADCAST/CLOSE_ACCEPT_BROADCAST XXX (где XXX - число), или 0 для выхода" << endl;
        }

		if (!CloseHandle(hNamedPipe)) 
            throw SetPipeError("CloseHandle error: ", GetLastError());

        stopFlag = true;
        if (monitorThread.joinable())
            monitorThread.join();
	}
	catch (string ErrorPipeText)
	{
		cout << endl << ErrorPipeText;
	}
	return 0;
}

void MonitorServerConnection(HANDLE hPipe, atomic<bool>& stopFlag) {
    while (!stopFlag) {
        if (!IsServerConnected(hPipe)) {
            cout << "Сервер отключился!" << endl;
            break;
        }
        this_thread::sleep_for(std::chrono::seconds(1));
    }
}

bool IsServerConnected(HANDLE hPipe) {
    DWORD availableBytes = 0;
    BOOL result = PeekNamedPipe(
        hPipe,
        NULL,
        0,
        NULL,
        &availableBytes,
        NULL);

    if (!result) {
        if (GetLastError() == ERROR_BROKEN_PIPE) {
            return false; // Сервер отключен
        }
    }
    return true;
}


string SetPipeError(string msgText, int code)
{
    return msgText + GetPipeError(code);
}

string GetPipeError(int code)
{
    string msgText;
    switch (code)
    {
    case WSAEINTR:
        msgText = "Function interrupted";
        break;
    case WSAEACCES:
        msgText = "Permission denied";
        break;
    case WSAEFAULT:
        msgText = "Wrong address";
        break;
    case WSAEINVAL:
        msgText = "Wrong args";
        break;
    case WSAEMFILE:
        msgText = "Too many files have opened";
        break;
    case WSAEWOULDBLOCK:
        msgText = "Resource temporarily unavailable";
        break;
    case WSAEINPROGRESS:
        msgText = "Operation in progress";
        break;
    case WSAEALREADY:
        msgText = "Operation already in progress";
        break;
    case WSAENOTSOCK:
        msgText = "Wrong socket";
        break;
    case WSAEDESTADDRREQ:
        msgText = "Required adress location";
        break;
    case WSAEMSGSIZE:
        msgText = "Message is too long ";
        break;
    case WSAEPROTOTYPE:
        msgText = "Wrong protocol type to socket";
        break;
    case WSAENOPROTOOPT:
        msgText = "Error in protocol option";
        break;
    case WSAEPROTONOSUPPORT:
        msgText = "Protocol is not supported";
        break;
    case WSAESOCKTNOSUPPORT:
        msgText = "Socket type is not supported";
        break;
    case WSAEOPNOTSUPP:
        msgText = "Operation is not supported";
        break;
    case WSAEPFNOSUPPORT:
        msgText = "Protocol type is not supported";
        break;
    case WSAEAFNOSUPPORT:
        msgText = "Addresses type is not supported by protocol";
        break;
    case WSAEADDRINUSE:
        msgText = "Address is already used";
        break;
    case WSAEADDRNOTAVAIL:
        msgText = "Requested address cannot be used";
        break;
    case WSAENETDOWN:
        msgText = "Network disconnected";
        break;
    case WSAENETUNREACH:
        msgText = "Network is unttainable";
        break;
    case WSAENETRESET:
        msgText = "Network dropped the connection";
        break;
    case WSAECONNABORTED:
        msgText = "Connection aborted";
        break;
    case WSAECONNRESET:
        msgText = "Connection restored";
        break;
    case WSAENOBUFS:
        msgText = "Not enough memory for buffers";
        break;
    case WSAEISCONN:
        msgText = "Socket has already connected";
        break;
    case WSAENOTCONN:
        msgText = "Socket has not connected";
        break;
    case WSAESHUTDOWN:
        msgText = "Send cannot be performed: socket was shutdowned";
        break;
    case WSAETIMEDOUT:
        msgText = "Alloted time interval has ended";
        break;
    case WSAECONNREFUSED:
        msgText = "Connection was rejected";
        break;
    case WSAEHOSTDOWN:
        msgText = "Host was shotdowned";
        break;
    case WSAEHOSTUNREACH:
        msgText = "Has not way for host";
        break;
    case WSAEPROCLIM:
        msgText = "Too many processes";
        break;
    case WSASYSNOTREADY:
        msgText = "Network is unavailable";
        break;
    case WSAVERNOTSUPPORTED:
        msgText = "Version is not supported";
        break;
    case WSANOTINITIALISED:
        msgText = "WS2_32.dll is not initialised";
        break;
    case WSAEDISCON:
        msgText = "Connection in progress";
        break;
    case WSATYPE_NOT_FOUND:
        msgText = "Type is not found";
        break;
    case WSAHOST_NOT_FOUND:
        msgText = "Host is not found";
        break;
    case WSATRY_AGAIN:
        msgText = "Try again";
        break;
    case WSANO_RECOVERY:
        msgText = "Unknown error";
        break;
    case WSANO_DATA:
        msgText = "Has not data type";
        break;
    case WSAEINVALIDPROCTABLE:
        msgText = "Invalid provider";
        break;
    case WSAEINVALIDPROVIDER:
        msgText = "Error in provider version";
        break;
    case WSAEPROVIDERFAILEDINIT:
        msgText = "Failed provider initialization";
        break;
    case WSASYSCALLFAILURE:
        msgText = "Abnormal termination of a system call";
        break;
    default:
        msgText = "Unknown error";
        break;
    }
    return msgText;
}