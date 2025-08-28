#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define VIEW_OFFSET 65536 // 64 * 1024
#define VIEW_SIZE_1 ((SIZE_T)1024)
#define VIEW_SIZE_2 ((SIZE_T)5120) // 5 * 1024

void handle_error(const char* message) {
    fprintf(stderr, "%s (Error code: %lu)\n", message, GetLastError());
    exit(EXIT_FAILURE);
}

int main() {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    LPVOID pView = NULL;

    hFile = CreateFileW(L"file.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) handle_error("Failed to open or create file");


    hMap = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (hMap == NULL) {
        CloseHandle(hFile);
        handle_error("Failed to create file mapping");
    }

    pView = MapViewOfFile(hMap, FILE_MAP_READ, 0, VIEW_OFFSET, VIEW_SIZE_1);
    if (pView == NULL) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        handle_error("Failed to map view for reading");
    }

    printf("File is mapped for reading\n");

    printf("Contents at offset %ld (size %zu):\n", (long)VIEW_OFFSET, VIEW_SIZE_1);

    fwrite(pView, 1, VIEW_SIZE_1, stdout);
    printf("\n");
    

    printf("Press enter to continue... ");
    getchar();

    UnmapViewOfFile(pView);

    pView = MapViewOfFile(hMap, FILE_MAP_WRITE, 0, VIEW_OFFSET, VIEW_SIZE_2);
    if (pView == NULL) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        handle_error("Failed to map view for writing");
    }

    printf("File is mapped for writing\n");

    memset(pView, '0', VIEW_SIZE_2);

    printf("Press enter to continue... ");
    getchar();

    FlushViewOfFile(pView, VIEW_SIZE_2);
    UnmapViewOfFile(pView);

    CloseHandle(hMap);
    CloseHandle(hFile);

    printf("File mapping is ended\n");
    printf("Press enter to continue... ");
    getchar();

    return 0;
}