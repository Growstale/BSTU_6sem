// emcc functions.c -o public/functions.wasm -s WASM=1 -O3

int sum(int x, int y) {
    return x+y;
}
  
int sub(int x, int y) {
    return x-y;
}
  
int mul(int x, int y) {
    return x*y;
}