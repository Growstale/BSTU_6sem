#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define FILE_MAPPING_NAME L"Lab-02"
#define VIEW_SIZE (SIZE_T)65536 // 64 * 1024
#define ITERATIONS 10
#define ARRAY_SIZE 655360       // 640 * 1024
#define INT_ARRAY_SIZE (ARRAY_SIZE / sizeof(int))
#define MUTEX_NAME L"Lab-02_Mutex"
#define DATA_READY_EVENT_NAME L"Lab-02_DataReady" 
#define DATA_READ_EVENT_NAME L"Lab-02_DataRead" 

void handle_error(const char* message) {
    fprintf(stderr, "%s (Error code: %lu)\n", message, GetLastError());
    exit(EXIT_FAILURE);
}

int main() {
    HANDLE hFileMapping;
    LPVOID pView;
    HANDLE hMutex;
    HANDLE hDataReadyEvent;
    HANDLE hDataReadEvent; 
    int* data;

    hFileMapping = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, FILE_MAPPING_NAME);

    if (hFileMapping == NULL) {
        handle_error("Failed to open file mapping");
    }

    hMutex = OpenMutexW(SYNCHRONIZE, FALSE, MUTEX_NAME);

    if (hMutex == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Could not open mutex");
    }

    hDataReadyEvent = OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, DATA_READY_EVENT_NAME);
    if (hDataReadyEvent == NULL) {
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not open DataReady event");
    }

    hDataReadEvent = OpenEventW(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, DATA_READ_EVENT_NAME);
    if (hDataReadEvent == NULL) {
        CloseHandle(hDataReadyEvent);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not open DataRead event");
    }

    pView = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, VIEW_SIZE);

    if (pView == NULL) {
        CloseHandle(hDataReadEvent);
        CloseHandle(hDataReadyEvent);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Failed to map view for reading");
    }

    data = (int*)pView;

    for (int i = 0; i < ITERATIONS; ++i) {
        WaitForSingleObject(hDataReadyEvent, INFINITE);

        WaitForSingleObject(hMutex, INFINITE);
        printf("Reader: Reading iteration %d\n", i + 1);
        for (int j = 0; j < VIEW_SIZE / sizeof(int); ++j) { 
            printf("%d ", data[j]);
        }

        ReleaseMutex(hMutex);
        SetEvent(hDataReadEvent);
    }

    printf("Reader: Finished reading. Press Enter to release resources\n");
    getchar();

    UnmapViewOfFile(pView);
    CloseHandle(hDataReadEvent);
    CloseHandle(hDataReadyEvent);
    CloseHandle(hMutex);
    CloseHandle(hFileMapping);


    printf("Reader: Resources released. Exiting\n");
    return 0;
}