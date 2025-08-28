#include <stdio.h>
#include <windows.h>


int main() {
  char service_name[] = "DemoService"; // имя сервиса
  SC_HANDLE hServiceControlManager, hService;
  SERVICE_STATUS service_status;

  // связываемся с менеджером сервисов
  hServiceControlManager =
      OpenSCManager(NULL, // локальная машина
                    NULL, // активная база данных сервисов
                    SC_MANAGER_CONNECT // соединение с менеджером сервисов
      );
  if (hServiceControlManager == NULL) {
    fprintf(stdout,
            "Open service control manager failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    return 0;
  }

  fprintf(stdout, "Service control manager is opened.\n"
                  "Press any key to continue.\n");
  getchar();

  // открываем сервис
  hService =
      OpenService(hServiceControlManager, // дескриптор менеджера сервисов
                  service_name,      // имя сервиса
                  SERVICE_ALL_ACCESS // любой доступ к сервису
      );
  if (hService == NULL) {
    fprintf(stdout,
            "Open service failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескриптор менеджера сервисов
    CloseServiceHandle(hServiceControlManager);

    return 0;
  }

  fprintf(stdout, "Service is opened.\n"
                  "Press any key to continue.\n");
  getchar();

  // определение состояния сервиса
  if (!QueryServiceStatus(hService, // дескриптор сервиса
                          &service_status // адрес структуры состояния сервиса
                          )) {
    fprintf(stdout,
            "Query service sataus failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }

  switch (service_status.dwCurrentState) {
  case SERVICE_STOPPED:
    fprintf(stdout, "The service is stopped\n");
    break;
  case SERVICE_RUNNING:
    fprintf(stdout, "The service is running\n");
    break;
  default:
    fprintf(stdout, "The service status: %lu\n", service_status.dwCurrentState);
    break;
  }

  fprintf(stdout, "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hService);

  return 0;
}