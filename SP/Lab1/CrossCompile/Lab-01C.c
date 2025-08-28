#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>

int main() {
    int x, k = 0;

    printf("Enter a three-digit number: ");
    scanf_s("%d", &x);

    if (x < 100 || x > 999) {
        printf("Error: a non-three-digit number was entered\n");
    }
    else {
        while (x > 0) {
            k += x % 10;
            x /= 10;
        }

        printf("The sum of the digits of a number: %d\n", k);
    }

    printf("Press Enter to exit...\n");
    getchar();
    getchar();

    return 0;
}