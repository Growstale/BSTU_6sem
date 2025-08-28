#include <Windows.h>
#include <handleapi.h>
#include <stdio.h>
#include <winnt.h>

char service_name[] = "DemoService"; // имя сервиса
char log_file[] = "C:\\ServiceFile.log";

SERVICE_STATUS service_status;
SERVICE_STATUS_HANDLE hServiceStatus;

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv);
VOID WINAPI ServiceCtrlHandler(DWORD dwControl);

HANDLE hOut; // выходной файл для протокола работы сервиса
int nCount;  // счетчик

// главная функция приложения
int main() {
  //  инициализируем структуру сервисов
  SERVICE_TABLE_ENTRY service_table[] = {
      {service_name, ServiceMain}, // имя сервиса и функция сервиса
      {NULL, NULL}                 // больше сервисов нет
  };

  // запускаем диспетчер сервиса
  if (!StartServiceCtrlDispatcher(service_table)) {
    hOut = CreateFileA(log_file, GENERIC_READ | GENERIC_WRITE,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    WriteFile(hOut, "Start service control dispatcher failed.\n", 42, NULL,
              NULL);
    CloseHandle(hOut);

    return 0;
  }

  return 0;
}

VOID WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpszArgv) {
  // регистрируем обработчик управляющих команд для сервиса
  hServiceStatus = RegisterServiceCtrlHandler(
      service_name,      // имя сервиса
      ServiceCtrlHandler // обработчик управляющих команд
  );
  if (!hServiceStatus) {
    hOut = CreateFileA(log_file, FILE_APPEND_DATA,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointer(hOut, 0l, 0l, FILE_END );
    WriteFile(hOut, "Register service control handler failed.\n", 42, NULL,
              NULL);
    CloseHandle(hOut);
    return;
  }

  // инициализируем структуру состояния сервиса
  service_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
  service_status.dwCurrentState = SERVICE_START_PENDING;
  service_status.dwControlsAccepted =
      SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
  service_status.dwWin32ExitCode = ERROR_SERVICE_SPECIFIC_ERROR;
  service_status.dwServiceSpecificExitCode = 0;
  service_status.dwCheckPoint = 0;
  service_status.dwWaitHint = 5000;

  // устанавливаем состояние сервиса
  if (!SetServiceStatus(hServiceStatus, &service_status)) {
    hOut = CreateFileA(log_file, FILE_APPEND_DATA,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointer(hOut, 0l, 0l, FILE_END );
    WriteFile(hOut, "Set service status 'SERVICE_START_PENDING' failed.\n", 52,
              NULL, NULL);
    CloseHandle(hOut);
    return;
  }

  // определяем сервис как работающий
  service_status.dwCurrentState = SERVICE_RUNNING;
  // нет ошибок
  service_status.dwWin32ExitCode = NO_ERROR;
  // устанавливаем новое состояние сервиса
  if (!SetServiceStatus(hServiceStatus, &service_status)) {
    hOut = CreateFileA(log_file, FILE_APPEND_DATA,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointer(hOut, 0l, 0l, FILE_END );
    WriteFile(hOut, "Set service status 'START_PENDING' failed.\n", 44, NULL,
              NULL);
    CloseHandle(hOut);
    return;
  }

  // открываем файл протокола работы сервиса

  hOut = CreateFileA(log_file, FILE_APPEND_DATA,
                     FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL, NULL);
  SetFilePointer(hOut, 0l, 0l, FILE_END );
  WriteFile(hOut, "The service is started.\n", 25, NULL, NULL);
  DWORD length = strlen(lpszArgv[0]) + 25;
  char *buffer = malloc(length);
  memset(buffer, 0, length);
  sprintf(buffer, "My name is: %s\n", lpszArgv[0]);
  WriteFile(hOut, buffer, strlen(buffer), NULL, NULL);
  free(buffer);
  // out << "My name is: " << lpszArgv[0] << endl << flush;
  for (int i = 1; i < (int)dwArgc; ++i) {
    length = strlen(lpszArgv[i]) + 25;
    buffer = malloc(length);
    memset(buffer, 0, length);
    sprintf(buffer, "My %d parameter = %s\n", i, lpszArgv[i]);
    WriteFile(hOut, buffer, strlen(buffer), NULL, NULL);
    free(buffer);
  }
  CloseHandle(hOut);
  // рабочий цикл сервиса
  while (service_status.dwCurrentState == SERVICE_RUNNING) {
    ++nCount;
    Sleep(3000);
  }
}

VOID WINAPI ServiceCtrlHandler(DWORD dwControl) {
  switch (dwControl) {
  case SERVICE_CONTROL_STOP: // остановить сервис
    // записываем конечное значение счетчика
    hOut = CreateFileA(log_file,  FILE_APPEND_DATA,
                       FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL, NULL);
    SetFilePointer(hOut, 0l, 0l, FILE_END );
    DWORD length = 25;
    char *buffer = malloc(length);
    memset(buffer, 0, length);
    sprintf(buffer, "Count = %d\n", nCount);
    WriteFile(hOut, buffer, strlen(buffer), NULL, NULL);
    WriteFile(hOut, "The service is finished.\n", 26, NULL, NULL);
    free(buffer);
    CloseHandle(hOut);
    // устанавливаем состояние остановки
    service_status.dwCurrentState = SERVICE_STOPPED;

    break;

  case SERVICE_CONTROL_SHUTDOWN: // завершить сервис
    service_status.dwCurrentState = SERVICE_STOPPED;
    break;

  default:
    // увеличиваем значение контрольной точки
    ++service_status.dwCheckPoint;
    // оставляем состояние сервиса без изменения
    break;
  }
  // изменить состояние сервиса
  SetServiceStatus(hServiceStatus, &service_status);
  return;
}