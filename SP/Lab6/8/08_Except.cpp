#include <windows.h>
#include <iostream>
#include <float.h>

using namespace std;

int main()
{
  double  a = 0;

  // получить управл¤ющее слово, заданное по умолчанию
  int cw = _controlfp( 0, 0 );
  // разрешить обработку исключений с плавающей точкой
  cw &=~(EM_OVERFLOW | EM_UNDERFLOW 
        | EM_INEXACT | EM_ZERODIVIDE | EM_DENORMAL);
  // установить новое управл¤ющее слово
  _controlfp( cw, _MCW_EM );
  // теперь можно обрабатывать исключени¤
  __try
  {
    double  b;

    b = 1/a;  // ошибка, деление на 0
  }
  __except(EXCEPTION_EXECUTE_HANDLER)
  {
    DWORD  ec = GetExceptionCode();  // получаем код исключени¤

    if (ec == EXCEPTION_FLT_DIVIDE_BY_ZERO)
      cout << "Exception float divide by zero." << endl;
    else
      cout << "Some other exception." << endl;

  }

  return 0;
}