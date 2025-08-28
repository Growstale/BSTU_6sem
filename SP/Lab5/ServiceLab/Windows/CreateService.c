#include <stdio.h>
#include <windows.h>


int main() {
  SC_HANDLE hServiceControlManager, hService;

  // связываемся с менеджером сервисов
  hServiceControlManager =
      OpenSCManager(NULL, // локальная машина
                    NULL, // активная база данных сервисов
                    SC_MANAGER_CREATE_SERVICE // возможно создание сервиса
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

  // устанавливаем новый сервис
  hService =
      CreateService(hServiceControlManager, // дескриптор менеджера сервисов
                    "DemoService", // внутреннее имя сервиса, используемое SCM
                    "Demo_Service", // внешнее имя сервиса в панели управления
                    SERVICE_ALL_ACCESS, // полный контроль над сервисом
                    SERVICE_WIN32_OWN_PROCESS, // сервис является процессом
                    SERVICE_DEMAND_START, // запускается сервис по требованию
                    SERVICE_ERROR_NORMAL, // обработка ошибок нормальная
                    "C:\\DemoService.exe", // путь к сервису
                    NULL, // сервис не принадлежит к группе
                    NULL, // тэг группы не изменяется
                    NULL, // сервис не зависит от других сервисов
                    NULL, // имя совпадает с текущим именем учетной записи
                    NULL  // пароля нет
      );
  if (hService == NULL) {
    fprintf(stdout,
            "Create service failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();

    // закрываем дескриптор менеджера сервисов
    CloseServiceHandle(hServiceControlManager);

    return 0;
  }

  fprintf(stdout, "Service is installed.\n"
                  "Press any key to exit.\n");
  getchar();

  // закрываем дескрипторы
  CloseServiceHandle(hService);
  CloseServiceHandle(hServiceControlManager);

  return 0;
}