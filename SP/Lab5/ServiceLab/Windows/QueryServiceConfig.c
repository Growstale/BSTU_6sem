#include <stdio.h>
#include <windows.h>


int main() {
  char service_name[] = "DemoService"; // имя сервиса
  SC_HANDLE hServiceControlManager, hService;

  // указатель на буфер для информации о конфигурации сервиса
  QUERY_SERVICE_CONFIG *lpQueryServiceConfig;
  // переменная для размера структуры в случае неудачи функции
  DWORD dwBytesNeeded;

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

  // захватываем память под буфер
  lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG)malloc(4096);

  // определяем конфигурацию сервиса
  if (!QueryServiceConfig(
          hService, // дескриптор сервиса
          lpQueryServiceConfig, // адрес структуры конфигурации сервиса
          4096,          // размер этой структуры
          &dwBytesNeeded // необходимое количество байтов
          )) {
    fprintf(stdout,
            "Query service configuartion failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();
    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }
  // распечатываем информацию о конфигурации
  fprintf(stdout,
          "Service type: %lu\n"
          "Start type: %lu\n"
          "Error control: %lu\n"
          "Binary path name: %s\n"
          "Service start name: %s\n"
          "Display name: %s\n",
          lpQueryServiceConfig->dwServiceType,
          lpQueryServiceConfig->dwStartType,
          lpQueryServiceConfig->dwErrorControl,
          lpQueryServiceConfig->lpBinaryPathName,
          lpQueryServiceConfig->lpServiceStartName,
          lpQueryServiceConfig->lpDisplayName);
  // освобождаем память
  free(lpQueryServiceConfig);

  fprintf(stdout, "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hService);

  return 0;
}