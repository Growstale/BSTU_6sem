#include <Windows.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <direct.h>
#include <shlwapi.h>
#include <shlobj.h>

#define _CRT_SECURE_NO_WARNINGS

#define APPDATA_SUBDIR "DemoService"
#define RESERVED_DIR_NAME "Reserved"
#define LOG_DIR_NAME "Logs"
#define DEFAULT_INTERVAL 1

#define FALLBACK_CONFIG_FILE "C:\\TestSource\\DemoService\\config.ini"
#define FALLBACK_BASE_DIR "C:\\TestSource\\DemoService"

char service_name[] = "DemoService";
SERVICE_STATUS service_status;
SERVICE_STATUS_HANDLE hServiceStatus;
HANDLE hLogFile = INVALID_HANDLE_VALUE;
char log_path[MAX_PATH];
char source_dir[MAX_PATH];
char reserved_dir[MAX_PATH];
char log_dir[MAX_PATH];
char config_file_path[MAX_PATH];
int copy_interval;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);
void LogMessage(const char* message, BOOL is_error);
BOOL ReadConfig();
void CopyFiles();
BOOL CreateOrCheckDir(const char* path, const char* dir_type);
BOOL InitAppDataPaths();


int main() {
    if (!InitAppDataPaths()) {
        fprintf(stderr, "Critical error: Failed to determine AppData paths.\n");
        return 1;
    }

    SERVICE_TABLE_ENTRY service_table[] = {
        {service_name, ServiceMain},
        {NULL, NULL}
    };

    if (!StartServiceCtrlDispatcher(service_table)) {
        LogMessage("StartServiceCtrlDispatcher failed", TRUE);
        fprintf(stderr, "StartServiceCtrlDispatcher failed (%lu)\n", GetLastError());
        return 1;
    }
    return 0;

}


BOOL InitAppDataPaths() {
    char appdata_path[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA | CSIDL_FLAG_CREATE, NULL, 0, appdata_path))) {

        snprintf(log_dir, MAX_PATH, "%s\\%s\\%s", appdata_path, APPDATA_SUBDIR, LOG_DIR_NAME);
        snprintf(reserved_dir, MAX_PATH, "%s\\%s\\%s", appdata_path, APPDATA_SUBDIR, RESERVED_DIR_NAME);
        return TRUE;
    }
    else {
        fprintf(stderr, "Warning: SHGetFolderPath failed (%lu). Falling back to hardcoded paths.\n", GetLastError());
        snprintf(log_dir, MAX_PATH, "%s\\%s", FALLBACK_BASE_DIR, LOG_DIR_NAME);
        snprintf(reserved_dir, MAX_PATH, "%s\\%s", FALLBACK_BASE_DIR, RESERVED_DIR_NAME);

        return TRUE;
    }
}


VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR* lpszArgv) {
    hServiceStatus = RegisterServiceCtrlHandler(service_name, ServiceCtrlHandler);
    if (!hServiceStatus) {
        return;
    }

    service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    service_status.dwCurrentState = SERVICE_START_PENDING;
    service_status.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE | SERVICE_ACCEPT_SHUTDOWN;
    service_status.dwWin32ExitCode = NO_ERROR;
    service_status.dwServiceSpecificExitCode = 0;
    service_status.dwCheckPoint = 1;
    service_status.dwWaitHint = 5000;
    SetServiceStatus(hServiceStatus, &service_status);


    if (!CreateOrCheckDir(log_dir, "log")) {
        service_status.dwCurrentState = SERVICE_STOPPED;
        service_status.dwWin32ExitCode = ERROR_CANT_ACCESS_FILE;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(log_path, MAX_PATH, "%s\\%04d%02d%02d%02d%02d%02d-service.log",
        log_dir, t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec);


    LogMessage("Log directory check/creation successful.", FALSE);


    if (!CreateOrCheckDir(reserved_dir, "reserved")) {
        LogMessage("Failed to create/find reserved directory. Stopping service.", TRUE);
        service_status.dwCurrentState = SERVICE_STOPPED;
        service_status.dwWin32ExitCode = ERROR_CANT_ACCESS_FILE;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }
    LogMessage("Reserved directory check/creation successful.", FALSE);


    if (dwArgc > 1 && lpszArgv[1] != NULL && strlen(lpszArgv[1]) > 0) {
        strcpy_s(config_file_path, MAX_PATH, lpszArgv[1]);
        char msg[MAX_PATH + 100];
        snprintf(msg, sizeof(msg), "Using configuration file path from argument: %s", config_file_path);
        LogMessage(msg, FALSE);
    }
    else {
        strcpy_s(config_file_path, MAX_PATH, FALLBACK_CONFIG_FILE);
        char msg[MAX_PATH + 100];
        snprintf(msg, sizeof(msg), "No configuration file argument provided. Using default: %s", config_file_path);
        LogMessage(msg, FALSE);
    }

    service_status.dwCheckPoint = 2;
    SetServiceStatus(hServiceStatus, &service_status);


    if (!ReadConfig()) {

        service_status.dwCurrentState = SERVICE_STOPPED;
        service_status.dwWin32ExitCode = ERROR_BAD_CONFIGURATION;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }

    service_status.dwCheckPoint = 3;
    SetServiceStatus(hServiceStatus, &service_status);


    if (!PathFileExists(source_dir)) {
        char errorMsg[MAX_PATH + 100];
        snprintf(errorMsg, sizeof(errorMsg), "Source directory specified in config ('%s') does not exist. Stopping service.", source_dir);
        LogMessage(errorMsg, TRUE);
        service_status.dwCurrentState = SERVICE_STOPPED;

        service_status.dwWin32ExitCode = ERROR_INVALID_PARAMETER;
        SetServiceStatus(hServiceStatus, &service_status);
        return;
    }
    else {
        char msg[MAX_PATH + 100];
        snprintf(msg, sizeof(msg), "Source directory '%s' found.", source_dir);
        LogMessage(msg, FALSE);
    }

    service_status.dwCheckPoint = 4;
    SetServiceStatus(hServiceStatus, &service_status);


    char params[MAX_PATH * 2 + 50];
    snprintf(params, sizeof(params), "source='%s', reserved='%s', interval=%d min", source_dir, reserved_dir, copy_interval);
    char msg[sizeof(params) + 100];
    snprintf(msg, sizeof(msg), "Success! Service %s started with parameters %s", service_name, params);
    LogMessage(msg, FALSE);

    service_status.dwCurrentState = SERVICE_RUNNING;
    service_status.dwCheckPoint = 0;
    service_status.dwWaitHint = 0;
    SetServiceStatus(hServiceStatus, &service_status);


    while (service_status.dwCurrentState == SERVICE_RUNNING || service_status.dwCurrentState == SERVICE_PAUSED) {
        if (service_status.dwCurrentState == SERVICE_RUNNING) {
            CopyFiles();
        }
        Sleep(copy_interval * 60 * 1000);
    }
    LogMessage("Service stopping.", FALSE);
    if (hLogFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hLogFile);
        hLogFile = INVALID_HANDLE_VALUE;
    }
}


VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
    char msg[512];
    DWORD old_state = service_status.dwCurrentState;

    switch (dwControl) {
    case SERVICE_CONTROL_STOP:
    case SERVICE_CONTROL_SHUTDOWN:
        snprintf(msg, sizeof(msg), "Success! Service %s received STOP signal. Changing state from %lu to STOP_PENDING",
            service_name, old_state);
        LogMessage(msg, FALSE);
        service_status.dwCurrentState = SERVICE_STOP_PENDING;
        service_status.dwCheckPoint = 1;
        service_status.dwWaitHint = 5000;
        break;

    case SERVICE_CONTROL_PAUSE:
        if (service_status.dwCurrentState == SERVICE_RUNNING) {
            snprintf(msg, sizeof(msg), "Success! Service %s received PAUSE signal. Changing state from RUNNING to PAUSED", service_name);
            LogMessage(msg, FALSE);
            service_status.dwCurrentState = SERVICE_PAUSED;
        }
        break;

    case SERVICE_CONTROL_CONTINUE:
        if (service_status.dwCurrentState == SERVICE_PAUSED) {
            snprintf(msg, sizeof(msg), "Success! Service %s received CONTINUE signal. Changing state from PAUSED to RUNNING", service_name);
            LogMessage(msg, FALSE);
            service_status.dwCurrentState = SERVICE_RUNNING;
        }
        break;


    case 131:
        snprintf(msg, sizeof(msg), "Hello, this is a test code (131) from service %s!", service_name);
        LogMessage(msg, FALSE);
        break;

    case 200:
        LogMessage("Received signal (200) to reload configuration.", FALSE);
        if (ReadConfig()) {

            if (!PathFileExists(source_dir)) {
                char errorMsg[MAX_PATH + 100];
                snprintf(errorMsg, sizeof(errorMsg), "Reload config failed: New source directory '%s' does not exist. Service will stop.", source_dir);
                LogMessage(errorMsg, TRUE);

                service_status.dwCurrentState = SERVICE_STOP_PENDING;
                service_status.dwCheckPoint = 1;
                service_status.dwWaitHint = 1000;
                service_status.dwWin32ExitCode = ERROR_BAD_CONFIGURATION;
            }
            else {
                char successMsg[MAX_PATH + 100];
                snprintf(successMsg, sizeof(successMsg), "Success! Service %s reloaded configuration. New source: '%s', Interval: %d min",
                    service_name, source_dir, copy_interval);
                LogMessage(successMsg, FALSE);
            }
        }
        break;

    default:
        snprintf(msg, sizeof(msg), "Received unknown control code: %lu", dwControl);
        LogMessage(msg, TRUE);
        break;
    }

    SetServiceStatus(hServiceStatus, &service_status);
}


void LogMessage(const char* message, BOOL is_error) {

    if (hLogFile == INVALID_HANDLE_VALUE && strlen(log_path) > 0) {

        hLogFile = CreateFile(log_path,
            FILE_APPEND_DATA,
            FILE_SHARE_READ,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        if (hLogFile == INVALID_HANDLE_VALUE) {

            fprintf(stderr, "ERROR: Cannot open log file '%s' (%lu). Message: %s\n", log_path, GetLastError(), message);
            return;
        }
    }
    else if (hLogFile == INVALID_HANDLE_VALUE) {

        fprintf(stderr, "Log file handle invalid, cannot write: %s\n", message);
        return;
    }

    char timestamp[32];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);

    snprintf(timestamp, sizeof(timestamp), "[%02d:%02d:%02d] ", t->tm_hour, t->tm_min, t->tm_sec);

    char full_msg[2048];
    snprintf(full_msg, sizeof(full_msg), "%s%s%s\r\n",
        timestamp,
        is_error ? "ERROR! " : "INFO: ",
        message);

    DWORD bytes_written;
    if (!WriteFile(hLogFile, full_msg, (DWORD)strlen(full_msg), &bytes_written, NULL)) {

        fprintf(stderr, "ERROR: Failed to write to log file '%s' (%lu)\n", log_path, GetLastError());

        CloseHandle(hLogFile);
        hLogFile = INVALID_HANDLE_VALUE;
    }

    FlushFileBuffers(hLogFile);

}

BOOL ReadConfig() {

    if (!PathFileExists(config_file_path)) {
        char errorMsg[MAX_PATH + 100];
        snprintf(errorMsg, sizeof(errorMsg), "Configuration file '%s' not found.", config_file_path);
        LogMessage(errorMsg, TRUE);
        return FALSE;
    }


    DWORD readResult = GetPrivateProfileString(
        "Settings",
        "SourceDir",
        "",
        source_dir,
        MAX_PATH,
        config_file_path);

    if (readResult == 0 || strlen(source_dir) == 0) {
        char errorMsg[MAX_PATH + 100];
        snprintf(errorMsg, sizeof(errorMsg), "Key 'SourceDir' not found or empty in section [Settings] of config file '%s'.", config_file_path);
        LogMessage(errorMsg, TRUE);
        return FALSE;
    }

    copy_interval = GetPrivateProfileInt(
        "Settings",
        "Interval",
        DEFAULT_INTERVAL,
        config_file_path);

    if (copy_interval < 1) {
        char warnMsg[100];
        snprintf(warnMsg, sizeof(warnMsg), "Interval value (%d) in config is less than 1 minute. Using 1 minute instead.", copy_interval);
        LogMessage(warnMsg, TRUE);
        copy_interval = 1;
    }

    char successMsg[MAX_PATH + 50];
    snprintf(successMsg, sizeof(successMsg), "Configuration read successfully. SourceDir='%s', Interval=%d.", source_dir, copy_interval);
    LogMessage(successMsg, FALSE);

    return TRUE;

}


void CopyFiles() {
    char search_path[MAX_PATH];

    snprintf(search_path, sizeof(search_path), "%s\\*.*", source_dir);

    WIN32_FIND_DATA fd;
    HANDLE hFind = FindFirstFile(search_path, &fd);

    if (hFind == INVALID_HANDLE_VALUE) {
        char errorMsg[MAX_PATH + 100];
        snprintf(errorMsg, sizeof(errorMsg), "Failed to access source directory '%s' for copying (%lu).", source_dir, GetLastError());
        LogMessage(errorMsg, TRUE);

        return;
    }

    int files_copied = 0;
    int files_failed = 0;
    do {

        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            char src_file[MAX_PATH];
            char dst_file[MAX_PATH];
            snprintf(src_file, sizeof(src_file), "%s\\%s", source_dir, fd.cFileName);

            snprintf(dst_file, sizeof(dst_file), "%s\\%s", reserved_dir, fd.cFileName);


            if (CopyFile(src_file, dst_file, FALSE)) {
                files_copied++;
            }
            else {
                char errorMsg[MAX_PATH + 100];
                snprintf(errorMsg, sizeof(errorMsg), "Failed to copy file '%s' to '%s' (%lu).",
                    src_file, dst_file, GetLastError());
                LogMessage(errorMsg, TRUE);
                files_failed++;
            }
        }
    } while (FindNextFile(hFind, &fd));

    FindClose(hFind);

    char summaryMsg[200];
    if (files_failed == 0) {
        snprintf(summaryMsg, sizeof(summaryMsg), "Success! Service %s completed backup operation. Copied %d file(s).", service_name, files_copied);
        LogMessage(summaryMsg, FALSE);
    }
    else {
        snprintf(summaryMsg, sizeof(summaryMsg), "Warning! Service %s completed backup operation with errors. Copied: %d, Failed: %d.", service_name, files_copied, files_failed);
        LogMessage(summaryMsg, TRUE);
    }
}


BOOL CreateOrCheckDir(const char* path, const char* dir_type) {
    char msg[MAX_PATH + 100];
    DWORD attributes = GetFileAttributes(path);

    if (attributes != INVALID_FILE_ATTRIBUTES) {
        if (attributes & FILE_ATTRIBUTE_DIRECTORY) {

            if (hLogFile != INVALID_HANDLE_VALUE || strcmp(dir_type, "log") != 0) {
                snprintf(msg, sizeof(msg), "Success! Service %s found existing %s directory '%s'", service_name, dir_type, path);
                LogMessage(msg, FALSE);
            }
            return TRUE;
        }
        else {
            snprintf(msg, sizeof(msg), "Operation failed! Path '%s' for %s directory exists but is not a directory.", path, dir_type);
            LogMessage(msg, TRUE);
            return FALSE;
        }
    }


    DWORD lastError = GetLastError();
    if (lastError == ERROR_FILE_NOT_FOUND || lastError == ERROR_PATH_NOT_FOUND) {

        DWORD createResult = SHCreateDirectoryEx(NULL, path, NULL);
        if (createResult == ERROR_SUCCESS || createResult == ERROR_ALREADY_EXISTS || createResult == ERROR_FILE_EXISTS) {

            if (hLogFile != INVALID_HANDLE_VALUE || strcmp(dir_type, "log") == 0) {
                snprintf(msg, sizeof(msg), "Success! Service %s created %s directory '%s'", service_name, dir_type, path);
                LogMessage(msg, FALSE);
            }

            return TRUE;
        }
        else {
            snprintf(msg, sizeof(msg), "Operation failed! Failed to create %s directory '%s' (SHCreateDirectoryEx error code: %lu)", dir_type, path, createResult);
            LogMessage(msg, TRUE);
            return FALSE;
        }
    }
    else {
        snprintf(msg, sizeof(msg), "Operation failed! Could not get attributes for %s directory path '%s' (Error code: %lu)", dir_type, path, lastError);
        LogMessage(msg, TRUE);
        return FALSE;
    }
}