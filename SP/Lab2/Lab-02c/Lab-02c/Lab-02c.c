#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#define FILE_MAPPING_NAME L"Lab-02"
#define VIEW_SIZE 65536 // 64 * 1024
#define ITERATIONS 10
#define ARRAY_SIZE 655360 // 640 * 1024
#define INT_ARRAY_SIZE (ARRAY_SIZE / sizeof(int)) // Number of ints in the array
#define MUTEX_NAME L"Lab-02_Mutex"

void handle_error(const char* message) {
    fprintf(stderr, "%s (Error code: %lu)\n", message, GetLastError());
    exit(EXIT_FAILURE);
}

int main() {
    HANDLE hFileMapping;
    LPVOID pView;
    HANDLE hMutex;
    int* data;

    hFileMapping = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        ARRAY_SIZE,
        FILE_MAPPING_NAME
    );

    if (hFileMapping == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Failed to create file mapping");
    }


    hMutex = CreateMutex(
        NULL,
        FALSE,
        MUTEX_NAME
    );

    if (hMutex == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Could not create mutex");
    }

    int* write_data = (int*)malloc(ARRAY_SIZE);
    if (write_data == NULL) {
        CloseHandle(hMutex);
        CloseHandle(hFileMapping);
        handle_error("Could not allocate memory for data to write");
    }
    for (int i = 0; i < INT_ARRAY_SIZE; ++i) {
        write_data[i] = i;
    }

    for (int i = 0; i < ITERATIONS; ++i) {
        pView = MapViewOfFile(
            hFileMapping,
            FILE_MAP_ALL_ACCESS,
            0,
            i * VIEW_SIZE,
            VIEW_SIZE
        );

        if (pView == NULL) {
            CloseHandle(hFileMapping);
            handle_error("Failed to map view for reading");
        }

        data = (int*)pView;

        WaitForSingleObject(hMutex, INFINITE);
        memcpy(data, write_data + (i * (VIEW_SIZE / sizeof(int))), VIEW_SIZE); // copy one VIEW_SIZE chunk each time
        ReleaseMutex(hMutex);
        printf("Writer: Wrote iteration %d\n", i + 1);
        Sleep(100);
    }

    free(write_data);

    printf("Writer: Finished writing. Press Enter to release resources\n");
    getchar();

    CloseHandle(hMutex);
    UnmapViewOfFile(pView);
    CloseHandle(hFileMapping);

    printf("Writer: Resources released. Exiting\n");
    return 0;
}