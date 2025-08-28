#include <windows.h>
#include <stdio.h>
#include <stdlib.h>


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
  lpQueryServiceConfig = (LPQUERY_SERVICE_CONFIG) malloc(4096);

  // определям конфигурацию сервиса
  QueryServiceConfig(hService, lpQueryServiceConfig, 4096, &dwBytesNeeded);
  // распечатываем информацию о режиме запуска сервиса
  fprintf(stdout, "Old start type: %lu.\n", lpQueryServiceConfig->dwStartType);

  // изменяем режим запуска сервиса
  if (!ChangeServiceConfig(
          hService,          // дескриптор сервиса
          SERVICE_NO_CHANGE, // тип сервиса не изменяем
          SERVICE_AUTO_START, // запуск во время загрузки системы
          SERVICE_NO_CHANGE, // режим обработки ошибок не изменяем
          NULL, NULL, NULL, NULL, NULL, NULL, NULL // все остальные параметры
                                                   // конфигурации не изменяем
          )) {
    fprintf(stdout,
            "Change service configuration failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескрипторы
    CloseServiceHandle(hServiceControlManager);
    CloseServiceHandle(hService);

    return 0;
  }

  // определям конфигурацию сервиса
  QueryServiceConfig(hService, lpQueryServiceConfig, 4096, &dwBytesNeeded);
  // распечатываем информацию о режиме запуска сервиса
  fprintf(stdout, "New start type: %lu.\n", lpQueryServiceConfig->dwStartType);

  // восстанавливаем режим запуска сервиса
  ChangeServiceConfig(
      hService,             // дескриптор сервиса
      SERVICE_NO_CHANGE,    // тип сервиса не изменяем
      SERVICE_DEMAND_START, // запуск во время загрузки системыпо требованию
      SERVICE_NO_CHANGE, // режим обработки ошибок не изменяем
      NULL, NULL, NULL, NULL, NULL, NULL, NULL // все остальные параметры
                                               // конфигурации не изменяем
  );
  // определям конфигурацию сервиса
  QueryServiceConfig(hService, lpQueryServiceConfig, 4096, &dwBytesNeeded);
  // распечатываем информацию о режиме запуска сервиса
  fprintf(stdout, "Old start type: %lu.\n", lpQueryServiceConfig->dwStartType);

  // освобождаем память
  free(lpQueryServiceConfig);

  fprintf(stdout, "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hService);

  return 0;
}