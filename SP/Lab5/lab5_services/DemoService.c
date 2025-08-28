#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")
#define _CRT_SECURE_NO_WARNINGS 

#define CONFIG_FILE "C:\\TestSource\\DemoService\\config.ini"
#define DEFAULT_RESERVED_DIR "C:\\TestSource\\DemoService\\Reserved"
#define DEFAULT_LOG_DIR "C:\\TestSource\\DemoService\\Logs"
#define DEFAULT_INTERVAL 60 

char service_name[] = "DemoService";
SERVICE_STATUS service_status;
SERVICE_STATUS_HANDLE hServiceStatus;
HANDLE hLogFile = INVALID_HANDLE_VALUE;
char log_path[MAX_PATH];
char source_dir[MAX_PATH];
char reserved_dir[MAX_PATH];
int copy_interval;
BOOL is_paused = FALSE;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
void LogMessage(const char* message, BOOL is_error);
void ReadConfig();
void CopyFiles();
void CreateOrCheckDir(const char* path, const char* dir_type);

int main() {
    SERVICE_TABLE_ENTRY service_table[] = {
        {service_name, ServiceMain},
        {NULL, NULL}
    };
    if (!StartServiceCtrlDispatcher(service_table)) {
        LogMessage("StartServiceCtrlDispatcher failed", TRUE);
        return 1;
    }
    return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    hServiceStatus = RegisterServiceCtrlHandler(service_name, ServiceCtrlHandler);
    if (!hServiceStatus) {
        LogMessage("RegisterServiceCtrlHandler failed", TRUE);
        return;
    }

    // Инициализируем статус сервиса
    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_START_PENDING;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_PRESHUTDOWN;
    service_status.dwWin32ExitCode = NO_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 5000;
    SetServiceStatus(hServiceStatus, &service_status);

    // Создаем путь к логам с текущей датой и временем
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(log_path, MAX_PATH, "%s\\%04d%02d%02d%02d%02d%02d-service.log",
        DEFAULT_LOG_DIR, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);

    // Проверяем/создаем каталоги
    CreateOrCheckDir(DEFAULT_LOG_DIR, "log");
    CreateOrCheckDir(DEFAULT_RESERVED_DIR, "reserved");

    // Читаем конфигурацию
    ReadConfig();

    // Проверяем существование исходного каталога
    if (!PathFileExists(source_dir)) {
        LogMessage("Source directory does not exist", TRUE);
        service_status.dwCurrentState = SERVICE_STOPPED;
        service_status.dwWin32ExitCode = ERROR_INVALID_PARAMETER;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    // Логируем запуск
    char params[512];
    snprintf(params, sizeof(params), "source=%s, reserved=%s, interval=%d min",
        source_dir, reserved_dir, copy_interval);
    char msg[512];
    snprintf(msg, sizeof(msg), "Success! Service %s started with parameters %s", service_name, params);
    LogMessage(msg, FALSE);

    // Переходим в состояние RUNNING
    service_status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(hServiceStatus, &service_status);

    // Основной цикл
    while (service_status.dwCurrentState == SERVICE_RUNNING) {
        if (!is_paused) {
            CopyFiles();
        }
        Sleep(copy_interval * 60 * 1000);
    }
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
    char msg[512];
    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        snprintf(msg, sizeof(msg), "Success! Service %s changed state from %s to STOPPED",
            service_name, is_paused ? "PAUSED" : "RUNNING");
        LogMessage(msg, FALSE);
        service_status.dwCurrentState = SERVICE_STOPPED;
        if (hLogFile != INVALID_HANDLE_VALUE) CloseHandle(hLogFile);
        break;
    case SERVICE_CONTROL_PAUSE:
        is_paused = TRUE;
        service_status.dwCurrentState = SERVICE_PAUSED;
        snprintf(msg, sizeof(msg), "Success! Service %s changed state from RUNNING to PAUSED", service_name);
        LogMessage(msg, FALSE);
        break;
    case SERVICE_CONTROL_CONTINUE:
        is_paused = FALSE;
        service_status.dwCurrentState = SERVICE_RUNNING;
        snprintf(msg, sizeof(msg), "Success! Service %s changed state from PAUSED to RUNNING", service_name);
        LogMessage(msg, FALSE);
        break;
    case SERVICE_CONTROL_PRESHUTDOWN:
        ReadConfig();
        LogMessage("Configuration reloaded", FALSE);
        break;
    case 128: // Пользовательский код
        snprintf(msg, sizeof(msg), "Hello, this is a test code from service %s!", service_name);
        LogMessage(msg, FALSE);
        break;
    default:
        service_status.dwCheckPoint++;
        break;
    }
    SetServiceStatus(hServiceStatus, &service_status);
}

void LogMessage(const char* message, BOOL is_error) {
    if (hLogFile == INVALID_HANDLE_VALUE) {
        hLogFile = CreateFile(log_path, FILE_APPEND_DATA, FILE_SHARE_READ, NULL,
            OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    }
    if (hLogFile != INVALID_HANDLE_VALUE) {
        char timestamp[32];
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        snprintf(timestamp, sizeof(timestamp), "[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);
        char full_msg[1024];
        snprintf(full_msg, sizeof(full_msg), "%s%s%s\n", timestamp,
            is_error ? "Operation failed! " : "", message);
        DWORD bytes_written;
        WriteFile(hLogFile, full_msg, (DWORD)strlen(full_msg), &bytes_written, NULL);
    }
}

void ReadConfig() {
    // Устанавливаем значения по умолчанию
    strcpy_s(source_dir, MAX_PATH, "C:\\TestSource\\DemoService\\Source");
    strcpy_s(reserved_dir, MAX_PATH, DEFAULT_RESERVED_DIR);
    copy_interval = DEFAULT_INTERVAL;

    // Читаем конфигурацию
    GetPrivateProfileString("Settings", "SourceDir", source_dir, source_dir, MAX_PATH, CONFIG_FILE);
    GetPrivateProfileString("Settings", "ReservedDir", reserved_dir, reserved_dir, MAX_PATH, CONFIG_FILE);
    copy_interval = GetPrivateProfileInt("Settings", "Interval", copy_interval, CONFIG_FILE);
}

void CopyFiles() {
    char search_path[MAX_PATH];
    snprintf(search_path, sizeof(search_path), "%s\\*.*", source_dir);
    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);
    if (hFind == INVALID_HANDLE_VALUE) {
        LogMessage("Failed to access source directory", TRUE);
        return;
    }

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char src_file[MAX_PATH], dst_file[MAX_PATH];
            snprintf(src_file, sizeof(src_file), "%s\\%s", source_dir, fd.cFileName);
            snprintf(dst_file, sizeof(dst_file), "%s\\%s", reserved_dir, fd.cFileName);
            if (CopyFile(src_file, dst_file, FALSE)) {
                char msg[512];
                snprintf(msg, sizeof(msg), "Success! Service %s created backup of file %s", service_name, fd.cFileName);
                LogMessage(msg, FALSE);
            }
            else {
                char msg[512];
                snprintf(msg, sizeof(msg), "Operation failed! Failed to copy file %s", fd.cFileName);
                LogMessage(msg, TRUE);
            }
        }
    } while (FindNextFile(hFind, &fd));
    FindClose(hFind);
    LogMessage("Success! Service DemoService completed backup operation", FALSE);
}

void CreateOrCheckDir(const char* path, const char* dir_type) {
    char msg[512];
    if (PathFileExists(path)) {
        snprintf(msg, sizeof(msg), "Success! Service %s found %s directory %s", service_name, dir_type, path);
    }
    else {
        if (_mkdir(path) == 0) {
            snprintf(msg, sizeof(msg), "Success! Service %s created %s directory %s", service_name, dir_type, path);
        }
        else {
            snprintf(msg, sizeof(msg), "Operation failed! Failed to create %s directory %s", dir_type, path);
        }
    }
    LogMessage(msg, strstr(msg, "failed") != NULL);
}