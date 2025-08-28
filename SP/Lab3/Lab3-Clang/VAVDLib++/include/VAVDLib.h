#ifndef MYLIBAPI
#define MYLIBAPI __declspec(dllexport)
#endif

extern MYLIBAPI const int array[1024];

extern "C" MYLIBAPI int bsearch_vav(const int* a, int n, int x);

extern MYLIBAPI int bsearch_r_vav(const int* a, int x, int i, int j);