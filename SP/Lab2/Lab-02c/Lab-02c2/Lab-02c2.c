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

    hFileMapping = OpenFileMapping(
        FILE_MAP_ALL_ACCESS,
        FALSE,
        FILE_MAPPING_NAME
    );

    if (hFileMapping == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Failed to create file mapping");
    }

    hMutex = OpenMutex(
        SYNCHRONIZE,
        FALSE,
        MUTEX_NAME
    );

    if (hMutex == NULL) {
        CloseHandle(hFileMapping);
        handle_error("Could not create mutex");
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

        printf("Reader: Reading iteration %d\n", i + 1);
        for (int j = 0; j < VIEW_SIZE / sizeof(int); ++j) {
            printf("%d ", data[j]);
        }
        printf("\n");
        ReleaseMutex(hMutex);
        Sleep(150);

    }

    printf("Reader: Finished reading. Press Enter to release resources\n");
    getchar();

    CloseHandle(hMutex);
    UnmapViewOfFile(pView);
    CloseHandle(hFileMapping);

    printf("Reader: Resources released. Exiting\n");
    return 0;
}