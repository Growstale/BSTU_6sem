#include <windows.h>
#include <stdio.h>

int main()
{
  char  service_name[] = "DemoService";  // имя сервиса
  SC_HANDLE  hServiceControlManager;   // дескриптор базы данных сервисов

  // указатель на буфер для внутреннего имени сервиса
  char*  lpszServiceName;
  // переменная для длины буфера в случае неудачи функции
  DWORD  dwBufferSize;

  // связываемся с менеджером сервисов
  hServiceControlManager = OpenSCManager(
    NULL,      // локальная машина
    NULL,      // активная база данных сервисов
    SC_MANAGER_CONNECT   // соединение с менеджером сервисов
    );
  if (hServiceControlManager == NULL)
  {
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

  // захватываем память под буфер
  dwBufferSize = 256;
  lpszServiceName = (char*) malloc(dwBufferSize);

  // определение внутреннего имени сервиса
  if (!GetServiceKeyName(
    hServiceControlManager,    // дескриптор базы данных сервисов
    "Demo_Service",    // внешнее имя сервиса
    lpszServiceName,   // буфер для имени сервиса
    &dwBufferSize      // количество необходимых байтов
    ))
  {
fprintf(stdout,
            "Get service key name failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();
    // закрываем дескриптор базы данных сервисов
    CloseServiceHandle(hServiceControlManager);

    return 0;
  }

  // выводим на консоль внутреннее имя сервиса
  fprintf(stdout,
          "Service key name: %s\n"
          "Press any key to exit.\n",
          lpszServiceName);

  getchar();
  free(lpszServiceName);

  // закрываем дескриптор базы данных сервисов
  CloseServiceHandle(hServiceControlManager);

  return 0;
}