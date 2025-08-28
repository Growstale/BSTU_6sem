#include <stdio.h>
#include <windows.h>

int main() {
  char service_name[] = "DemoService"; // имя сервиса
  SERVICE_STATUS service_status;       // состояние сервиса
  SC_HANDLE hServiceControlManager, hService;

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
                  service_name,               // имя сервиса
                  SERVICE_ALL_ACCESS | DELETE // любой доступ к сервису
                                              // и удаление из базы данных
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

  // получаем состояние сервиса
  if (!QueryServiceStatus(hService, &service_status)) {
    fprintf(stdout,
            "Open service status failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();
    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }

  // если сервис работает, то останавливаем его
  if (service_status.dwCurrentState != SERVICE_STOPPED) {
    fprintf(stdout, "Service is working. It will be stoped\n");
    if (!ControlService(hService, SERVICE_CONTROL_STOP, &service_status)) {
      fprintf(stdout,
              "Control service failed.\n"
              "The last error code: %lu\n"
              "Press any key to continue.\n",
              GetLastError());

      // закрываем дескрипторы
      CloseServiceHandle(hServiceControlManager);
      CloseServiceHandle(hService);

      return 0;
    }

    // ждем, пока сервис остановится
    Sleep(500);
  }

  // удаляем сервис
  if (!DeleteService(hService)) {
    fprintf(stdout,
            "Delete service failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }

  fprintf(stdout, "The service is deleted.\n"
                  "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hService);

  return 0;
}