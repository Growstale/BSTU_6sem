#include <stdio.h>
#include <windows.h>


int main() {
  char service_name[] = "DemoService"; // имя сервиса
  char *param[] = {"p1", "p2", "p3"};  // список параметров
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
                  service_name,      // имя сервиса
                  SERVICE_ALL_ACCESS // полный доступ к сервису
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

  // стартуем сервис
  if (!StartService(hService, // дескриптор сервиса
                    3,        // три параметра
                    (const char **)param // указатель на массив параметров
                    )) {
    fprintf(stdout,
            "Start service failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }

  fprintf(stdout, "The service is started.\n"
                  "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hService);

  return 0;
}