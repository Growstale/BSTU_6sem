#include <stdio.h>
#include <windows.h>

int main() {
  char service_name[] = "DemoService"; // имя сервиса
  SC_HANDLE hServiceControlManager; // дескриптор базы данных сервисов
  SC_LOCK hScLock; // дескриптор блокировки базы данных сервисов

  // указатель на буфер для состояния о блокировке
  LPQUERY_SERVICE_LOCK_STATUS lpLockStatus;
  // длина буфера
  DWORD dwBufferSize;
  // переменная для длины буфера в случае неудачи функции
  DWORD dwBytesNeeded;

  // связываемся с менеджером сервисов
  hServiceControlManager = OpenSCManager(
      NULL, // локальная машина
      NULL, // активная база данных сервисов
      SC_MANAGER_LOCK | // разрешено блокировка базы данных
          SC_MANAGER_QUERY_LOCK_STATUS // и определения состояния блокировки
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

  // блокируем базу данных сервисов
  hScLock = LockServiceDatabase(hServiceControlManager);
  if (hScLock == NULL) {
    fprintf(stdout,
            "Lock service database failed.\n"
            "The last error code: %lu\n"
            "Press any key to continue.\n",
            GetLastError());
    getchar();
    // закрываем дескриптор базы данных сервисов
    CloseServiceHandle(hServiceControlManager);

    return 0;
  }

  fprintf(stdout, "\tThe service database is locked.\n");

  // захватываем память под буфер
  dwBufferSize = 4096;
  lpLockStatus = (LPQUERY_SERVICE_LOCK_STATUS)malloc(dwBufferSize);
  // определяем состояние блокировки
  QueryServiceLockStatus(hServiceControlManager, lpLockStatus, dwBufferSize,
                         &dwBytesNeeded);
  // выводим на консоль состояние блокировки
  fprintf(stdout,
          "Lock state: %lu\n"
          "Lock owner: %s\n"
          "Lock duration: %lu\n",
          lpLockStatus->fIsLocked, lpLockStatus->lpLockOwner,
          lpLockStatus->dwLockDuration);

  // разблокируем базу данных сервисов
  UnlockServiceDatabase(hScLock);

  fprintf(stdout, "\tThe service database is unlocked.\n");

  // определяем состояние блокировки
  QueryServiceLockStatus(hServiceControlManager, lpLockStatus, dwBufferSize,
                         &dwBytesNeeded);
  // выводим на консоль состояние блокировки
  fprintf(stdout,
          "Lock state: %lu\n"
          "Lock owner: %s\n"
          "Lock duration: %lu\n"
          "Press any key to exit.\n",
          lpLockStatus->fIsLocked, lpLockStatus->lpLockOwner,
          lpLockStatus->dwLockDuration);

  getchar();
  free(lpLockStatus);
  // закрываем дескрипторы
  CloseServiceHandle(hServiceControlManager);
  CloseServiceHandle(hScLock);

  return 0;
}