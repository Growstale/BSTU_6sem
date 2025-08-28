#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#define VIEW_OFFSET 0
#define VIEW_SIZE 134217728  // 128 * 1024 * 1024

void handle_error(const char* message) {
    fprintf(stderr, "%s (Error code: %lu)\n", message, GetLastError());
    exit(EXIT_FAILURE);
}

int main() {
    HANDLE hFile = INVALID_HANDLE_VALUE;
    HANDLE hMap = NULL;
    LPVOID pView = NULL;

    hFile = CreateFileW(L"text.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL); 
    if (hFile == INVALID_HANDLE_VALUE) handle_error("Failed to open file");


    hMap = CreateFileMappingW(hFile, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (hMap == NULL) {
        CloseHandle(hFile);
        handle_error("Failed to create file mapping");
    }

    pView = MapViewOfFile(hMap, FILE_MAP_READ, 0, VIEW_OFFSET, VIEW_SIZE);
    if (pView == NULL) {
        CloseHandle(hMap);
        CloseHandle(hFile);
        handle_error("Failed to map view for reading");
    }

    printf("File is mapped for reading\n");

    printf("Contents at offset %d (size %u):\n", VIEW_OFFSET, VIEW_SIZE);
    fwrite(pView, 1, VIEW_SIZE, stdout);
    printf("\n");

    printf("Press enter to continue... ");
    getchar();

    UnmapViewOfFile(pView);

    CloseHandle(hMap);
    CloseHandle(hFile);

    printf("File mapping is ended\n");

    return 0;
}