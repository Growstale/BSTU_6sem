#include <windows.h>
#include <stdio.h>
#include <string.h>

void CreateServiceFn(const char* service_name, const char* binary_path);
void StartServiceFn(const char* service_name, const char* config_path);
void StopServiceFn(const char* service_name);
void DeleteServiceFn(const char* service_name);
void PauseServiceFn(const char* service_name);
void ContinueServiceFn(const char* service_name);
void InfoServiceFn(const char* service_name);
void TestServiceFn(const char* service_name);
void ConfigServiceFn(const char* service_name);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: ServiceCtl <service_name> <operation> [params]\n");
        printf("Operations: Create, Start, Stop, Delete, Pause, Continue, Info, Test, Config\n");
        return 1;
    }

    const char* service_name = argv[1];
    const char* operation = argv[2];

    if (_stricmp(operation, "Create") == 0) {
        if (argc != 4) {
            printf("Usage: ServiceCtl %s Create <binary_path>\n", service_name);
            return 1;
        }
        CreateServiceFn(service_name, argv[3]);
    }
    else if (_stricmp(operation, "Start") == 0) {
        if (argc != 4) {
            printf("Usage: ServiceCtl %s Start <config_path>\n", service_name);
            return 1;
        }
        StartServiceFn(service_name, argv[3]);
    }
    else if (_stricmp(operation, "Stop") == 0) {
        StopServiceFn(service_name);
    }
    else if (_stricmp(operation, "Delete") == 0) {
        DeleteServiceFn(service_name);
    }
    else if (_stricmp(operation, "Pause") == 0) {
        PauseServiceFn(service_name);
    }
    else if (_stricmp(operation, "Continue") == 0) {
        ContinueServiceFn(service_name);
    }
    else if (_stricmp(operation, "Info") == 0) {
        InfoServiceFn(service_name);
    }
    else if (_stricmp(operation, "Test") == 0) {
        TestServiceFn(service_name);
    }
    else if (_stricmp(operation, "Config") == 0) {
        ConfigServiceFn(service_name);
    }
    else {
        printf("Invalid operation: %s\n", operation);
        return 1;
    }
    return 0;
}

void CreateServiceFn(const char* service_name, const char* binary_path) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);  // MachineName DatabaseName DesiredAcces
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = CreateService(scm, service_name, "Demo_Service",
        SERVICE_ALL_ACCESS, SERVICE_WIN32_OWN_PROCESS, 
        SERVICE_DEMAND_START, SERVICE_ERROR_NORMAL,
        binary_path, NULL, NULL, NULL, NULL, NULL); // BinaryPathName LoadOrderGroup TagId Dependencies ServiceStartName Password
    if (service) {
        printf("Service %s created\n", service_name);
        CloseServiceHandle(service);
    }
    else {
        printf("CreateService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(scm);
}

void StartServiceFn(const char* service_name, const char* config_path) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_START);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    const char* params[] = { config_path };
    if (StartService(service, 1, params)) {
        printf("Service %s started\n", service_name);
    }
    else {
        printf("StartService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void StopServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_STOP);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (ControlService(service, SERVICE_CONTROL_STOP, &status)) {
        printf("Service %s stopped\n", service_name);
    }
    else {
        printf("ControlService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void DeleteServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, DELETE);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    if (DeleteService(service)) {
        printf("Service %s deleted\n", service_name);
    }
    else {
        printf("DeleteService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void PauseServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_PAUSE_CONTINUE);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (ControlService(service, SERVICE_CONTROL_PAUSE, &status)) {
        printf("Service %s paused\n", service_name);
    }
    else {
        printf("ControlService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void ContinueServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_PAUSE_CONTINUE);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (ControlService(service, SERVICE_CONTROL_CONTINUE, &status)) {
        printf("Service %s continued\n", service_name);
    }
    else {
        printf("ControlService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void InfoServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_QUERY_STATUS | SERVICE_QUERY_CONFIG);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (QueryServiceStatus(service, &status)) {
        printf("Service %s status: %lu\n", service_name, status.dwCurrentState);
    }
    else {
        printf("QueryServiceStatus failed (%lu)\n", GetLastError());
    }
    QUERY_SERVICE_CONFIG* config = (QUERY_SERVICE_CONFIG*)malloc(4096);
    DWORD bytes_needed;
    if (QueryServiceConfig(service, config, 4096, &bytes_needed)) {
        printf("Binary path: %s\n", config->lpBinaryPathName);
        printf("Display name: %s\n", config->lpDisplayName);
    }
    else {
        printf("QueryServiceConfig failed (%lu)\n", GetLastError());
    }
    free(config);
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void TestServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_USER_DEFINED_CONTROL);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (ControlService(service, 131, &status)) {
        printf("Test signal sent to %s\n", service_name);
    }
    else {
        printf("ControlService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}

void ConfigServiceFn(const char* service_name) {
    SC_HANDLE scm = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
    if (!scm) {
        printf("OpenSCManager failed (%lu)\n", GetLastError());
        return;
    }
    SC_HANDLE service = OpenService(scm, service_name, SERVICE_USER_DEFINED_CONTROL);
    if (!service) {
        printf("OpenService failed (%lu)\n", GetLastError());
        CloseServiceHandle(scm);
        return;
    }
    SERVICE_STATUS status;
    if (ControlService(service, 200, &status)) {
        printf("Test signal sent to %s\n", service_name);
    }
    else {
        printf("ControlService failed (%lu)\n", GetLastError());
    }
    CloseServiceHandle(service);
    CloseServiceHandle(scm);
}