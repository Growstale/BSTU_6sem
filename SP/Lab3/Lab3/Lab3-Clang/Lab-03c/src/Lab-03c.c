#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

typedef int (*IterativeBSearchFunc)(int*, int, int);
typedef int (*RecursiveBSearchFunc)(int*, int, int, int);

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Incorrect number of arguments entered\n");
        printf("Lab-03c <library> <function> [value]\n");
        return 1;
    }

    void* handle = dlopen(argv[1], RTLD_NOW);
    if (!handle) {
        fprintf(stderr, "Error during library loading (%s)\n", dlerror());
        return 1;
    }

    const int* array = dlsym(handle, "array");
    if (array == NULL) {
        fprintf(stderr, "Error during getting array: (%s)\n", dlerror());
        dlclose(handle);
        return 1;
    }

    int x;

    if (argc < 4) {
        printf("Enter the value to search for: ");
        if (scanf("%d", &x) != 1) {
            printf("Invalid input.\n");
            dlclose(handle);
            return 1;
        }
    } else {
        x = atoi(argv[3]);
    }

    if (strcmp(argv[2], "bsearch_vav") == 0) {
        IterativeBSearchFunc func = dlsym(handle, argv[2]);
        if (!func) {
            fprintf(stderr, "Error: Function %s not found in library %s!\n", argv[2], argv[1]);
            dlclose(handle);
            return 1;
        }

        int index = func((int*)array, 1024, x);
        if (index == -1) {
            printf("The number was not found\n");
        }
        else {
            printf("Number %d was found at position %d\n", x, index);
        }

    }
    else if (strcmp(argv[2], "bsearch_r_vav") == 0) {
        RecursiveBSearchFunc func = dlsym(handle, argv[2]);
        if (!func) {
            fprintf(stderr, "Error: Function %s not found in library %s!\n", argv[2], argv[1]);
            dlclose(handle);
            return 1;
        }

        int index = func((int*)array, x, 0, 1023);
        if (index == -1) {
            printf("The number was not found\n");
        }
        else {
            printf("Number %d was found at position %d\n", x, index);
        }

    }
    else {
        printf("Functions that can be used: bsearch_vav, bsearch_r_vav\n");
        dlclose(handle);
        return 1;
    }

    dlclose(handle);
    return 0;
}
