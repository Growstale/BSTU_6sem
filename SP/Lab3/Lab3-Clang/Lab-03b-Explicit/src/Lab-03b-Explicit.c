#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

typedef int (*IterativeBSearchFunc)(const int*, int, int);
typedef int (*RecursiveBSearchFunc)(const int*, int, int, int);
typedef const int* LibArrayPtr;

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        if (argc < 2) {
            printf("The library to download is not specified\n");
        }
        else {
            printf("The function being called is not specified\n");
        }
        printf("Usage: %s <LibraryName.dll> <FunctionName|Number> [NumberToSearch]\n", argv[0]);
        printf("Example Labs: VAVDLib.dll, VAVDLib++.dll\n");
        printf("Example Funcs: 1. bsearch_vav, 2. bsearch_r_vav\n");
        return 1;
    }

    char* libFileName = argv[1];
    char* funcIdentifier = argv[2];

    HMODULE hLib = LoadLibrary(libFileName);

    if (NULL == hLib)
    {
        printf("The downloaded library was not found (%s, Error: %lu)\n", libFileName, GetLastError());
        return 1;
    }

    IterativeBSearchFunc funcIterative = NULL;
    RecursiveBSearchFunc funcRecursive = NULL;
    LibArrayPtr          loadedArray = NULL;

    if (0 == strcmp(libFileName, "VAVDLib.dll"))
    {
        funcIterative = (IterativeBSearchFunc)GetProcAddress(hLib, "iterative_binary_search");
        funcRecursive = (RecursiveBSearchFunc)GetProcAddress(hLib, "bsearch_r_vav");
        loadedArray = (LibArrayPtr)GetProcAddress(hLib, "array");
    }
    else if (0 == strcmp(libFileName, "VAVDLib++.dll"))
    {
        funcIterative = (IterativeBSearchFunc)GetProcAddress(hLib, "bsearch_vav");
        funcRecursive = (RecursiveBSearchFunc)GetProcAddress(hLib, "?bsearch_r_vav@@YAHPEBHHHH@Z");
        loadedArray = (LibArrayPtr)GetProcAddress(hLib, "?array@@3QBHB");
    }

    if (funcIterative == NULL && funcRecursive == NULL)
    {
        printf("The required functions were not found in the library %s\n", libFileName);
        FreeLibrary(hLib);
        return 1;
    }
    if (loadedArray == NULL) {
        printf("Could not load array from library %s\n", libFileName);
        FreeLibrary(hLib);
        return 1;
    }


    int numberToSearch;
    if (argc >= 4) {
        numberToSearch = atoi(argv[3]);
    }
    else {
        printf("Enter the number: ");
        if (scanf("%d", &numberToSearch) != 1) {
            printf("Error\n");
            FreeLibrary(hLib);
            return 1;
        }
    }


    int foundPosition = -1;
    int isIterativeCall = (strcmp(funcIdentifier, "bsearch_vav") == 0 || strcmp(funcIdentifier, "1") == 0);
    int isRecursiveCall = (strcmp(funcIdentifier, "bsearch_r_vav") == 0 || strcmp(funcIdentifier, "2") == 0);

    if (isIterativeCall)
    {
        if (funcIterative != NULL) { 
            foundPosition = funcIterative((const int*)loadedArray, 1024, numberToSearch);
        }
        else {
            printf("Iterative function pointer is invalid for %s!\n", libFileName);
            FreeLibrary(hLib);
            return 1;
        }
    }
    else if (isRecursiveCall)
    {
        if (funcRecursive != NULL) { 
            foundPosition = funcRecursive((const int*)loadedArray, numberToSearch, 0, 1023);
        }
        else {
            printf("Recursive function pointer is invalid for %s!\n", libFileName);
            FreeLibrary(hLib);
            return 1;
        }
    }
    else
    {
        printf("The required %s function is not supported!\n", funcIdentifier);
        FreeLibrary(hLib);
        return 1;
    }

    if (foundPosition != -1) {
        printf("%s: The number %d was found at position %d!\n", funcIdentifier, numberToSearch, foundPosition);
    }
    else {
        printf("%s: The specified number was not found!\n", funcIdentifier);
    }

    FreeLibrary(hLib);

    return 0;
}