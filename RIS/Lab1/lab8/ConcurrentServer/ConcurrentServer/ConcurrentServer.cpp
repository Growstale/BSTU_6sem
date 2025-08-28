#include "ConcurrentServer.h"
#include "..\..\ServiceLibrary\ServiceLibrary\DefineTableService.h"
HANDLE hAcceptServer, hDispatchServer, hGarbageCleaner, hConsolePipe, hResponseServer;
int port = 2000; 
int udpport = 2000;
const char* servercall = "vodchyts_server";
CRITICAL_SECTION ListContactCriticalSection;
ListContact Contacts;                
HANDLE(*sss)(char*, LPVOID);
HMODULE st;
const char* dllname = "D:\\6sem_BSTU\\RIS\\lab8\\ServiceLibrary\\x64\\Debug\\ServiceLibrary.dll";
wstring pipename = L"RConsole";
volatile LONG CurrentClients;
volatile LONG TotalClients;
volatile LONG RejectedClients;
volatile LONG TimeoutClients;
bool serverIsStop = false;
map<int, SOCKET> portMap;
mutex portMapMutex;
map<int, SOCKET> broadcastPortMap;


HANDLE AcceptEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

int main(int argc, char* argv[]) {

    if (argc > 1) {
        try {
            port = stoi(argv[1]);
        }
        catch (exception& e) {
            cerr << "Invalid port number: " << argv[1] << ". Using default port 2000" << endl;
            port = 2000;
        }
    }
    if (argc > 2) { 
        dllname = argv[2];
    }
    if (argc > 3) {
        pipename = wstring(argv[3], argv[3] + strlen(argv[3]));
    }
    if (argc > 4) {
        udpport = stoi(argv[4]);
    }
    if (argc > 5) {
        servercall = argv[5];
    }

    cout << "Using port in main: " << port << endl;

    wchar_t wDllName[MAX_PATH];
    MultiByteToWideChar(CP_ACP, 0, dllname, -1, wDllName, MAX_PATH);

    st = LoadLibrary(wDllName);

    if (!st) {
        cout << "Error loading DLL: " << GetLastError() << endl;
        return 1;
    } 
    sss = (HANDLE(*)(char*, LPVOID))GetProcAddress(st, "SSS");
    if (!sss) {
        cout << "Error finding function: " << GetLastError() << endl;
        FreeLibrary(st);
        return 1;
    }


    InitializeCriticalSection(&ListContactCriticalSection);

    volatile TalkersCommand cmd = START; //  не оптимизировать доступ к переменной, поскольку её значение может 
                                         //  изменяться вне текущего потока выполнения

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        SetErrorMsgText("WSAStartup failed", WSAGetLastError());
        return 1;
    }
    CurrentClients = 0;
    TotalClients = 0;
    RejectedClients = 0;
    TimeoutClients = 0;

    hAcceptServer = CreateThread(NULL, NULL, AcceptServer, (LPVOID)&cmd, NULL, NULL);
    hDispatchServer = CreateThread(NULL, NULL, DispatchServer, (LPVOID)&cmd, NULL, NULL);
    hGarbageCleaner = CreateThread(NULL, NULL, GarbageCleaner, (LPVOID)&cmd, NULL, NULL);
    hConsolePipe = CreateThread(NULL, NULL, ConsolePipe, (LPVOID)&cmd, NULL, NULL);
    hResponseServer = CreateThread(NULL, NULL, ResponseServer, (LPVOID)&cmd, NULL, NULL);

    SetThreadPriority(hAcceptServer, THREAD_PRIORITY_HIGHEST);			
    SetThreadPriority(hGarbageCleaner, THREAD_PRIORITY_LOWEST);

    if (hAcceptServer != NULL && hDispatchServer != NULL && hGarbageCleaner != NULL && hConsolePipe != NULL && hResponseServer != NULL) {
        WaitForSingleObject(hAcceptServer, INFINITE);
        WaitForSingleObject(hDispatchServer, INFINITE);
        WaitForSingleObject(hGarbageCleaner, INFINITE);
        WaitForSingleObject(hConsolePipe, INFINITE);
        WaitForSingleObject(hResponseServer, INFINITE);

        if (!CloseHandle(hAcceptServer)) { 
            cout << SetErrorMsgText("CloseHandle failed", GetLastError()) << endl;
        }
        if (!CloseHandle(hDispatchServer)) {
            cout << SetErrorMsgText("DispatchServer failed", GetLastError()) << endl;
        }
        if (!CloseHandle(hGarbageCleaner)) {
            cout << SetErrorMsgText("GarbageCleaner failed", GetLastError()) << endl;
        }
        if (!CloseHandle(hConsolePipe)) {
            cout << SetErrorMsgText("hConsolePipe failed", GetLastError()) << endl;
        }
        if (!CloseHandle(hResponseServer)) {
            cout << SetErrorMsgText("hResponseServer failed", GetLastError()) << endl;
        }
    }
    DeleteCriticalSection(&ListContactCriticalSection);
    WSACleanup();
    FreeLibrary(st);

    return 0;
}

string GetErrorMsgText(int code)
{
    string msgText;
    switch (code)
    {
    case WSAEINTR:          msgText = "WSAEINTR: Function interrupted";
        break;
    case WSAEACCES:         msgText = "WSAEACCES: Permission denied";
        break;
    case WSAEFAULT:         msgText = "WSAEFAULT: Invalid address";
        break;
    case WSAEINVAL:         msgText = "WSAEINVAL: Invalid argument";
        break;
    case WSAEMFILE:         msgText = "WSAEMFILE: Too many open files";
        break;
    case WSAEWOULDBLOCK:    msgText = "WSAEWOULDBLOCK: Resource temporarily unavailable";
        break;
    case WSAEINPROGRESS:    msgText = "WSAEINPROGRESS: Operation in progress";
        break;
    case WSAEALREADY:       msgText = "WSAEALREADY: Operation already in progress";
        break;
    case WSAENOTSOCK:       msgText = "WSAENOTSOCK: Socket operation on nonsocket";
        break;
    case WSAEDESTADDRREQ:   msgText = "WSAEDESTADDRREQ: Destination address required";
        break;
    case WSAEMSGSIZE:       msgText = "WSAEMSGSIZE: Message too long";
        break;
    case WSAEPROTOTYPE:     msgText = "WSAEPROTOTYPE: Protocol wrong type for socket";
        break;
    case WSAENOPROTOOPT:    msgText = "WSAENOPROTOOPT: Protocol not available";
        break;
    case WSAEPROTONOSUPPORT: msgText = "WSAEPROTONOSUPPORT: Protocol not supported";
        break;
    case WSAESOCKTNOSUPPORT: msgText = "WSAESOCKTNOSUPPORT: Socket type not supported";
        break;
    case WSAEOPNOTSUPP:     msgText = "WSAEOPNOTSUPP: Operation not supported";
        break;
    case WSAEPFNOSUPPORT:   msgText = "WSAEPFNOSUPPORT: Protocol family not supported";
        break;
    case WSAEAFNOSUPPORT:   msgText = "WSAEAFNOSUPPORT: Address family not supported by protocol";
        break;
    case WSAEADDRINUSE:     msgText = "WSAEADDRINUSE: Address already in use";
        break;
    case WSAEADDRNOTAVAIL:  msgText = "WSAEADDRNOTAVAIL: Cannot assign requested address";
        break;
    case WSAENETDOWN:       msgText = "WSAENETDOWN: Network is down";
        break;
    case WSAENETUNREACH:    msgText = "WSAENETUNREACH: Network is unreachable";
        break;
    case WSAENETRESET:      msgText = "WSAENETRESET: Network dropped connection";
        break;
    case WSAECONNABORTED:   msgText = "WSAECONNABORTED: Software caused connection abort";
        break;
    case WSAECONNRESET:     msgText = "WSAECONNRESET: Connection reset by peer";
        break;
    case WSAENOBUFS:        msgText = "WSAENOBUFS: No buffer space available";
        break;
    case WSAEISCONN:        msgText = "WSAEISCONN: Socket is already connected";
        break;
    case WSAENOTCONN:       msgText = "WSAENOTCONN: Socket is not connected";
        break;
    case WSAESHUTDOWN:      msgText = "WSAESHUTDOWN: Cannot send after socket shutdown";
        break;
    case WSAETIMEDOUT:      msgText = "WSAETIMEDOUT: Connection timed out";
        break;
    case WSAECONNREFUSED:   msgText = "WSAECONNREFUSED: Connection refused";
        break;
    case WSAEHOSTDOWN:      msgText = "WSAEHOSTDOWN: Host is down";
        break;
    case WSAEHOSTUNREACH:   msgText = "WSAEHOSTUNREACH: No route to host";
        break;
    case WSAEPROCLIM:       msgText = "WSAEPROCLIM: Too many processes";
        break;
    case WSASYSNOTREADY:    msgText = "WSASYSNOTREADY: Network subsystem is unavailable";
        break;
    case WSAVERNOTSUPPORTED: msgText = "WSAVERNOTSUPPORTED: Version not supported";
        break;
    case WSANOTINITIALISED: msgText = "WSANOTINITIALISED: Winsock not initialized";
        break;
    case WSAEDISCON:        msgText = "WSAEDISCON: Graceful disconnect in progress";
        break;
    case WSATYPE_NOT_FOUND: msgText = "WSATYPE_NOT_FOUND: Class not found";
        break;
    case WSAHOST_NOT_FOUND: msgText = "WSAHOST_NOT_FOUND: Host not found";
        break;
    case WSATRY_AGAIN:      msgText = "WSATRY_AGAIN: Try again later";
        break;
    case WSANO_RECOVERY:    msgText = "WSANO_RECOVERY: Non-recoverable error";
        break;
    case WSANO_DATA:        msgText = "WSANO_DATA: No data of requested type";
        break;
    case WSA_INVALID_HANDLE: msgText = "WSA_INVALID_HANDLE: Invalid event handle";
        break;
    case WSA_INVALID_PARAMETER: msgText = "WSA_INVALID_PARAMETER: Invalid parameter";
        break;
    case WSA_IO_INCOMPLETE: msgText = "WSA_IO_INCOMPLETE: Overlapped I/O operation not completed";
        break;
    case WSA_IO_PENDING:    msgText = "WSA_IO_PENDING: Operation will complete later";
        break;
    case WSA_NOT_ENOUGH_MEMORY: msgText = "WSA_NOT_ENOUGH_MEMORY: Not enough memory";
        break;
    case WSA_OPERATION_ABORTED: msgText = "WSA_OPERATION_ABORTED: Operation aborted";
        break;
    case WSASYSCALLFAILURE: msgText = "WSASYSCALLFAILURE: System call failure";
        break;
    default:                msgText = "***ERROR***";
        break;
    };
    return msgText;
};


string SetErrorMsgText(string msgText, int code)
{
    return  msgText + GetErrorMsgText(code);
};