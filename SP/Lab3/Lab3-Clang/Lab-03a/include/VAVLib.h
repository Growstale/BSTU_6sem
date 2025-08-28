#include <stdio.h>

extern const int array[1024];

int bsearch_vav(const int* a, int n, int x);

int bsearch_r_vav(const int* a, int x, int i, int j);