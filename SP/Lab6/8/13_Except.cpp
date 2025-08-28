#include <eh.h>
#include <iostream>
#include <windows.h>

using namespace std;

void se_trans_func(unsigned code, EXCEPTION_POINTERS *info) {
  EXCEPTION_RECORD er;
  CONTEXT c;
  EXCEPTION_POINTERS ep = {&er, &c};
  er = *(info->ExceptionRecord);
  c = *(info->ContextRecord);
  throw ep;
}

int main() {
  int a = 10, b = 0;

  // устанавливаем функцию транслятор
  _set_se_translator(se_trans_func);
  // перехватываем структурное исключение средствами С++
  try {
    a /= b; // ошибка, деление на ноль
  } catch (EXCEPTION_POINTERS ep) {
    cout << "Exception code = " << hex << ep.ExceptionRecord->ExceptionCode
         << endl;
  }

  return 0;
}