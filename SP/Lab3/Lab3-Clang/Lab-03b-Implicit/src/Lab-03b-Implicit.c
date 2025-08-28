#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

__declspec(dllimport) int iterative_binary_search(const int*, int, int);
__declspec(dllimport) int bsearch_r_vav(const int*, int, int, int);
__declspec(dllimport) const int array[1024];

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("The function being called is not specified\n");
        printf("Usage: %s <FunctionName|Number> [NumberToSearch]\n", argv[0]);
        printf("Example Funcs: 1. bsearch_vav, 2. bsearch_r_vav\n");
        return 1;
    }

    char* funcIdentifier = argv[1];


    int numberToSearch;
    if (argc >= 3) {
        numberToSearch = atoi(argv[2]);
    }
    else {
        printf("Enter the number: ");
        if (scanf("%d", &numberToSearch) != 1) {
            printf("Error\n");
            return 1;
        }
    }


    int foundPosition = -1;
    int isIterativeCall = (strcmp(funcIdentifier, "bsearch_vav") == 0 || strcmp(funcIdentifier, "1") == 0);
    int isRecursiveCall = (strcmp(funcIdentifier, "bsearch_r_vav") == 0 || strcmp(funcIdentifier, "2") == 0);

    if (isIterativeCall) foundPosition = iterative_binary_search(array, 1024, numberToSearch);
    else if (isRecursiveCall) foundPosition = bsearch_r_vav(array, numberToSearch, 0, 1023);
    else
    {
        printf("The required %s function is not supported!\n", funcIdentifier);
        return 1;
    }

    if (foundPosition != -1) {
        printf("%s: The number %d was found at position %d!\n", funcIdentifier, numberToSearch, foundPosition);
    }
    else {
        printf("%s: The specified number was not found!\n", funcIdentifier);
    }

    return 0;
}