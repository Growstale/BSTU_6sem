#include <windows.h>
#include <stdio.h> 

int main() {
    HINSTANCE hDll = LoadLibraryA("mydll.dll");
    DWORD lastError; 

    if (!hDll) {
        lastError = GetLastError(); 
        fprintf(stderr, "Error: Could not load mydll.dll. Code: %lu\n", lastError);
        return 1;
    }

    printf("mydll.dll loaded successfully.\n");

    // Access data variable exported by 1, NONAME
    int* dataPtr = (int*)GetProcAddress(hDll, MAKEINTRESOURCE(1));

    if (dataPtr) {
        printf("Accessing sharedDataVariable (ordinal 1): Value = %d\n", *dataPtr);
    } else {
        lastError = GetLastError();
        fprintf(stderr, "Error: Could not get address of data at ordinal 1. Code: %lu\n", lastError);

    }

    void* failedPtr = GetProcAddress(hDll, "sharedDataVariable"); 
    if (!failedPtr) {
         DWORD nameError = GetLastError();
         printf("As expected, failed to get data address using name 'sharedDataVariable' due to NONAME. (Error: %lu)\n", nameError);
    } else {
         fprintf(stderr, "Unexpected: Succeeded in getting data address using name 'sharedDataVariable'!\n");
    }

    FreeLibrary(hDll);
    printf("mydll.dll unloaded.\n");

    return 0;
}