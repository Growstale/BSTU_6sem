#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define FILE_MAPPING_NAME L"Lab-02"
#define VIEW_SIZE (SIZE_T)65536 // 64 * 1024
#define ITERATIONS 10
#define ARRAY_SIZE 655360 // 640 * 1024
#define INT_ARRAY_SIZE (ARRAY_SIZE / sizeof(int)) // Number of ints in the array
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

    hFileMapping = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        ARRAY_SIZE,
        FILE_MAPPING_NAME
    );

    if (hFileMapping == NULL) {
        handle_error("Failed to create file mapping");
    }

    hMutex = CreateMutexW(
        NULL,
        FALSE,
        MUTEX_NAME
    );

    if (hMutex == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Could not create mutex");
    }

    hDataReadyEvent = CreateEventW(NULL, FALSE, FALSE, DATA_READY_EVENT_NAME);
    if (hDataReadyEvent == NULL) {
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not create DataReady event");
    }

    hDataReadEvent = CreateEventW(NULL, FALSE, FALSE, DATA_READ_EVENT_NAME); 
    if (hDataReadEvent == NULL) {
        CloseHandle(hDataReadyEvent);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not create DataRead event");
    }

    int* write_data = (int*)malloc(ARRAY_SIZE);
    if (write_data == NULL) {
        CloseHandle(hDataReadEvent);
        CloseHandle(hDataReadyEvent);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not allocate memory for data to write");
    }
    for (int i = 0; i < INT_ARRAY_SIZE; ++i) {
        write_data[i] = i;
    }

    pView = MapViewOfFile(
        hFileMapping,
        FILE_MAP_ALL_ACCESS,
        0,
        0,
        VIEW_SIZE
    );

    if (pView == NULL) {
        CloseHandle(hDataReadEvent);
        CloseHandle(hDataReadyEvent);
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Failed to map view for reading");
    }


    for (int i = 0; i < ITERATIONS; ++i) {
        if (i > 0) {
            WaitForSingleObject(hDataReadEvent, INFINITE);
        }

        WaitForSingleObject(hMutex, INFINITE);
        data = (int*)pView;
        memcpy(data, write_data + (i * (VIEW_SIZE / sizeof(int))), VIEW_SIZE);
        printf("Writer: Wrote iteration %d\n", i + 1);

        ReleaseMutex(hMutex);

        SetEvent(hDataReadyEvent);
    }

    printf("Writer: Finished writing. Press Enter to release resources\n");
    getchar();

    UnmapViewOfFile(pView);
    free(write_data);
    CloseHandle(hDataReadEvent);
    CloseHandle(hDataReadyEvent);
    CloseHandle(hMutex);
    CloseHandle(hFileMapping);


    printf("Writer: Resources released. Exiting\n");
    return 0;
}