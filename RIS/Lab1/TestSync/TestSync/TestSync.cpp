#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
using namespace std;
const uint32_t NTP_EPOCH = 2208988800U;
#pragma comment(lib, "ws2_32.lib")

struct NTPPacket {
    uint8_t li_vn_mode;
    uint8_t stratum;
    uint8_t poll;
    uint8_t precision;
    uint32_t rootDelay;
    uint32_t rootDispersion;
    uint32_t refId;
    uint32_t refTm_s;
    uint32_t refTm_f;
    uint32_t origTm_s;
    uint32_t origTm_f;
    uint32_t rxTm_s;
    uint32_t rxTm_f;
    uint32_t txTm_s;
    uint32_t txTm_f;
};

void getTimeFromNTP(NTPPacket& packet) {
    WSADATA wsaData;
    SOCKET sock;
    SOCKADDR_IN serverAddr;
    const char* ntpServer = "pool.ntp.org";

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock" << endl;
        return;
    }

    sock = socket(AF_INET, SOCK_DGRAM, NULL);
    if (sock == INVALID_SOCKET) {
        cerr << "Ошибка создания сокета" << endl;
        WSACleanup();
        return;
    }

    addrinfo hints{}, * res;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(ntpServer, "123", &hints, &res) != 0) {
        cerr << "Ошибка получения IP-адреса NTP-сервера" << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    serverAddr = *reinterpret_cast<sockaddr_in*>(res->ai_addr);
    freeaddrinfo(res);

    packet.li_vn_mode = (0 << 6) | (4 << 3) | (3 << 0);

    if (sendto(sock, (char*)&packet, sizeof(NTPPacket), 0,
        (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка отправки запроса" << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    sockaddr_in fromAddr;
    int fromLen = sizeof(fromAddr);
    if (recvfrom(sock, (char*)&packet, sizeof(NTPPacket), 0,
        (sockaddr*)&fromAddr, &fromLen) == SOCKET_ERROR) {
        cerr << "Ошибка получения ответа" << endl;
        closesocket(sock);
        WSACleanup();
        return;
    }

    closesocket(sock);
    WSACleanup();
}

void startServer() {
    WSADATA wsaData;
    SOCKET serverSocket;
    SOCKADDR_IN serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "Ошибка инициализации Winsock" << endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Ошибка создания серверного сокета" << endl;
        WSACleanup();
        return;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(4000); 

    if (bind(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Ошибка привязки сокета" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return;
    }

    cout << "Сервер запущен, ожидаю запросы..." << endl;

    while (true) {
        char buffer[1024];
        NTPPacket packet{};

        int bytesReceived = recvfrom(serverSocket, (char*)&packet, sizeof(buffer), 0, (SOCKADDR*)&clientAddr, &clientAddrSize);
        if (bytesReceived == SOCKET_ERROR) {
            cerr << "Ошибка получения данных" << endl;
            continue;
        }

        uint32_t timeSecondsClient = ntohl(packet.txTm_s);  // Преобразуем порядок байтов
        time_t ntpTimeClient = static_cast<time_t>(timeSecondsClient) - NTP_EPOCH;

        getTimeFromNTP(packet);

        uint32_t timeSeconds = ntohl(packet.txTm_s);  // Преобразуем порядок байтов
        time_t ntpTime = static_cast<time_t>(timeSeconds) - NTP_EPOCH;


        struct tm gmTime;
        gmtime_s(&gmTime, &ntpTime);
        char timeStr[26];
        asctime_s(timeStr, sizeof(timeStr), &gmTime);

        SYSTEMTIME st;
        st.wYear = gmTime.tm_year + 1900;
        st.wMonth = gmTime.tm_mon + 1;
        st.wDay = gmTime.tm_mday;
        st.wHour = gmTime.tm_hour;
        st.wMinute = gmTime.tm_min;
        st.wSecond = gmTime.tm_sec;
        st.wMilliseconds = 0;

        cout << "\nПолучено время: " << timeStr << endl;

        double timeDifference = difftime(ntpTime, ntpTimeClient);

        cout << "Разница во времени: " << timeDifference << " секунд" << endl;


        if (sendto(serverSocket, (char*)&packet, sizeof(NTPPacket), 0, (SOCKADDR*)&clientAddr, clientAddrSize) == SOCKET_ERROR) {
            cerr << "Ошибка отправки ответа" << endl;
        }
        else {
            cout << "Время отправлено клиенту" << endl;
        }
    }

    closesocket(serverSocket);
    WSACleanup();
}

int main() {
    setlocale(LC_ALL, "Russian");

    startServer();

    return 0;
}
