#include "pch.h"


CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// создает матрицу преобразования, которая позволяет перевести координаты из мировой системы координат в оконную систему координат
// RS - область в мировых координатах - double
// RW - область в оконных координатах - int
{
	CMatrix M(3, 3);
	CSize sz = RW.Size();	 // Размер области в ОКНЕ
	int dwx = sz.cx;	     // Ширина
	int dwy = sz.cy;	     // Высота
	CSizeD szd = RS.SizeD(); // Размер области в МИРОВЫХ координатах

	double dsx = szd.cx;    // Ширина в мировых координатах
	double dsy = szd.cy;    // Высота в мировых координатах

	double kx = (double)dwx / dsx;   // Масштаб по x
	double ky = (double)dwy / dsy;   // Масштаб по y

	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

CRectD::CRectD(double l, double t, double r, double b) // конструктор инициализирует объект класса CRectD с заданными координатами
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b) // метод позволяет установить новые координаты для прямоугольника
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD() // метод вычисляет размеры прямоугольника (ширину и высоту) и возвращает их в виде объекта CSizeD
{
	CSizeD cz;
	// Используется fabs, чтобы результат был положительным, даже если right < left
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области
	return cz;
}


void CPlot2D::SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX) // устанавливает параметры графика: данные по осям X и Y, а 
// также область в окне, где будет отображаться график
// XX - вектор данных по X 
// YY - вектор данных по Y 
// RWX - область в окне 
{
	// Проверяет, что размеры векторов X и Y совпадают. Если нет, выводит сообщение об ошибке и завершает программу
	int nRowsX = XX.rows();
	int nRowsY = YY.rows();
	if (nRowsX != nRowsY)
	{
		TCHAR* error = _T("SetParams: неправильные размеры массивов данных");
		MessageBox(NULL, error, _T("Ошибка"), MB_ICONSTOP);

		exit(1);
	}
	// Проверяет, что размеры векторов X и Y совпадают. Если нет, выводит сообщение об ошибке и завершает программу
	X.RedimMatrix(nRowsX);
	Y.RedimMatrix(nRowsY);
	X = XX;
	Y = YY;
	// Вычисляет минимальные и максимальные значения по осям X и Y
	double x_max = X.MaxElement();
	double x_min = X.MinElement();
	double y_max = Y.MaxElement();
	double y_min = Y.MinElement();
	// Устанавливает прямоугольник RS в мировой системе координат (МСК) на основе минимальных и максимальных значений
	RS.SetRectD(x_min, y_max, x_max, y_min);
	// Устанавливает прямоугольник RW в оконной системе координат (ОСК)
	RW.SetRect(RWX.left, RWX.top, RWX.right, RWX.bottom);
	K = SpaceToWindow(RS, RW);								// Вычисляет матрицу преобразования K
}
//-------------------------------------------------------------------


void CPlot2D::SetWindowRect(CRect& RWX) // обновляет область в окне, где будет отображаться график, и пересчитывает матрицу преобразования
{
	RW.SetRect(RWX.left, RWX.top, RWX.right, RWX.bottom);	// Обновляет прямоугольник RW в оконной системе координат
	K = SpaceToWindow(RS, RW);			// Пересчитывает матрицу преобразования K
}

//--------------------------------------------------------------------

void CPlot2D::SetPenLine(CMyPen& PLine)
// Установка параметров пера для линии графика
{
	PenLine.PenStyle = PLine.PenStyle;
	PenLine.PenWidth = PLine.PenWidth;
	PenLine.PenColor = PLine.PenColor;
}

//-------------------------------------------------------------------
void CPlot2D::SetPenAxis(CMyPen& PAxis)
// Установка параметров пера для линий осей 
{
	PenAxis.PenStyle = PAxis.PenStyle;
	PenAxis.PenWidth = PAxis.PenWidth;
	PenAxis.PenColor = PAxis.PenColor;
}

void CPlot2D::GetWindowCoords(double xs, double ys, int& xw, int& yw)
// Пересчитывает координаты точки из МСК в оконную
// xs - x- кордината точки в МСК
// ys - y- кордината точки в МСК
// xw - x- кордината точки в оконной СК
// yw - y- кордината точки в оконной СК

{
	CMatrix V(3), W(3); // Создает вектор V с координатами точки в МСК
	V(2) = 1;
	V(0) = xs;
	V(1) = ys;
	W = K * V; // Умножает вектор V на матрицу преобразования K, чтобы получить координаты точки в ОСК
	xw = (int)W(0);
	yw = (int)W(1);
	// Возвращает координаты xw и yw в ОСК
}

//-----------------------------------------------------------------
void CPlot2D::Draw(CDC& dc, int Ind1, int Ind2)
// Рисует график в режиме MM_TEXT - собственный пересчет координат
// dc - ссылка на класс CDC MFC
// Ind1=1/0 - рисовать/не рисовать рамку
// Ind2=1/0 - рисовать/не рисовать оси координат
{
	double xs, ys;		// мировые  координаты точки
	int xw, yw;			// оконные координаты точки
	if (Ind1 == 1)	dc.Rectangle(RW);					// Если Ind1 == 1, рисует рамку вокруг области графика
	if (Ind2 == 1)		// Если Ind2 == 1, рисует оси координат
	{
		CPen MyPen(PenAxis.PenStyle, PenAxis.PenWidth, PenAxis.PenColor);
		CPen* pOldPen = dc.SelectObject(&MyPen);
		// Устанавливает созданное перо как текущее для контекста устройства dc. Возвращает указатель на старое перо, чтобы потом восстановить его
		if (RS.left * RS.right < 0)						// Проверяет, пересекает ли ось Y область графика. Если произведение левой и правой границ 
														// отрицательно, значит, ось Y проходит через область графика
		{
			xs = 0;  ys = RS.top;						// Устанавливает координаты верхней точки оси Y в МСК
			GetWindowCoords(xs, ys, xw, yw);			// Пересчитывает координаты из МСК в ОСК		
			dc.MoveTo(xw, yw);							// Перемещает перо в верхнюю точку оси Y

			xs = 0;  ys = RS.bottom;					// Устанавливает координаты нижней точки оси Y в МСК
			GetWindowCoords(xs, ys, xw, yw);			// Пересчитывает координаты из МСК в ОСК
			dc.LineTo(xw, yw);							// Рисует линию от верхней точки оси Y до нижней
		}

		if (RS.top * RS.bottom < 0)						// Проверяет, пересекает ли ось X область графика. Если произведение верхней и нижней границ отрицательно, 
														// значит, ось X проходит через область графика
		{
			xs = RS.left;  ys = 0;						// Устанавливает координаты левой точки оси X в МСК
			GetWindowCoords(xs, ys, xw, yw);			// Пересчитывает координаты из МСК в ОСК
			dc.MoveTo(xw, yw);							// Перемещает перо в левую точку оси X

			xs = RS.right;  ys = 0;						// Устанавливает координаты правой точки оси X в МСК
			GetWindowCoords(xs, ys, xw, yw);			// Пересчитывает координаты из МСК в ОСК
			dc.LineTo(xw, yw);							// Рисует линию от левой точки оси X до правой
		}
		dc.SelectObject(pOldPen); // Восстанавливает старое перо, чтобы не нарушать настройки контекста устройства

	}

	xs = X(0); ys = Y(0);  // Устанавливает координаты первой точки графика в МСК
	GetWindowCoords(xs, ys, xw, yw);					// Пересчитывает координаты из МСК в ОСК
	CPen MyPen(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor); // Пересчитывает координаты из МСК в ОСК
	CPen* pOldPen = dc.SelectObject(&MyPen); // Устанавливает созданное перо как текущее для контекста устройства
	dc.MoveTo(xw, yw);									// Перемещает перо в начальную точку графика
	for (int i = 1; i < X.rows(); i++) // Проходит по всем точкам графика, начиная со второй
	{
		xs = X(i); ys = Y(i); // Устанавливает координаты текущей точки графика в МСК
		GetWindowCoords(xs, ys, xw, yw);				// Пересчитывает координаты из МСК в ОСК
		dc.LineTo(xw, yw); // Рисует линию от предыдущей точки до текущей
	}
	dc.SelectObject(pOldPen); // Восстанавливает старое перо, чтобы не нарушать настройки контекста устройства
} 

