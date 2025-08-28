#include "stdafx.h"
#include "Lab09.h"

CRectD::CRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

void CPlot2D::SetPenLine(CMyPen& PLine)
// Установка параметров пера для линии графика
{
	PenLine.PenStyle = PLine.PenStyle;
	PenLine.PenWidth = PLine.PenWidth;
	PenLine.PenColor = PLine.PenColor;
}

/*
	P(t) = Сумма от i = 0 до n [ Bᵢⁿ(t) * Pᵢ ],

	где

	Bᵢⁿ(t) = C(n, i) * (1 - t)^(n - i) * t^i
	Pᵢ - контрольные точки

*/



// Этот метод использует контрольные точки this->X и this->Y
// Он итерирует NT раз, вычисляя по одной точке кривой Безье для каждого t и сразу рисуя отрезок к ней

void CPlot2D::DrawBezier(CDC& dc, int NT, int bezierVariantIndex)
// dc - контекст устройства
// NT - число отрезков по параметру t
// Параметр t — это число от 0 до 1, которое говорит "где именно мы на кривой"
{
	double xs, ys;  // мировые координаты точки на кривой
	int xw, yw;     // оконные координаты
	CPen MyPen(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor);
	CPen* pOldPen = dc.SelectObject(&MyPen);

	double dt = 1.0 / NT; // Шаг по параметру t
	int N_ctrl_pts = X.rows(); // Количество контрольных точек
	// X и Y - это члены класса, содержащие контрольные точки

	CMatrix RX(N_ctrl_pts), RY(N_ctrl_pts); // Временные матрицы для алгоритма де Кастельжо

	// Матрицы для НАКОПЛЕНИЯ всех точек кривой
	CMatrix XB_curve_points(NT + 1);
	CMatrix YB_curve_points(NT + 1);

	// Начальная точка кривой - это первая контрольная точка
	xs = X(0);   ys = Y(0);

	int point_idx = 0; // Индекс для записи в XB_curve_points, YB_curve_points
	if (point_idx <= NT) { // Защита от выхода за пределы
		XB_curve_points(point_idx) = xs;
		YB_curve_points(point_idx) = ys;
		point_idx++; // <<< Инкрементируем СРАЗУ ПОСЛЕ ЗАПИСИ ПЕРВОЙ ТОЧКИ
	}

	GetWindowCoords(xs, ys, xw, yw);
	dc.MoveTo(xw, yw);

	// Цикл по параметру t от dt до 1.0 (NT шагов)
	for (int k = 1; k <= NT; k++)
	{
		double t = k * dt; // Текущее значение параметра t

		// 1. Копируем контрольные точки в рабочие массивы RX, RY 
		for (int i = 0; i < N_ctrl_pts; i++)
		{
			RX(i) = X(i);
			RY(i) = Y(i);
		}

		// 2. Алгоритм де Кастельжо
		// Внешний цикл: от N-1 до 1 (уменьшается количество сегментов)
		for (int j = N_ctrl_pts - 1; j > 0; j--)
		{
			// Внутренний цикл: от 0 до j-1
			for (int i = 0; i < j; i++)
			{
				// P_i = P_i - t * P_i + t * P_i+1
				RX(i) = RX(i) + t * (RX(i + 1) - RX(i)); 
				RY(i) = RY(i) + t * (RY(i + 1) - RY(i));
			}
		}

		/*
		    На каждой итерации создаются новые точки между предыдущими — это линейная интерполяция
			Первый проход: находит точки между соседними контрольными
			Второй проход: находит точки между результатами первого прохода
			И так далее, пока не останется одна точка — она и есть точка на кривой для текущего t
		*/

		// 3. Результат P_0^(N) находится в RX(0), RY(0)
		xs = RX(0);   ys = RY(0);

		// НАКАПЛИВАЕМ точку кривой
		if (point_idx <= NT) { // Убедимся, что не выходим за пределы (NT+1 точек)
			XB_curve_points(point_idx) = xs;
			YB_curve_points(point_idx) = ys;
			point_idx++;
		}

		GetWindowCoords(xs, ys, xw, yw);
		dc.LineTo(xw, yw); // Рисуем линию к новой точке на кривой

	}
	dc.SelectObject(pOldPen);

	// --- ЗАПИСЬ ВСЕХ ТОЧЕК КРИВОЙ В ФАЙЛЫ ПОСЛЕ ЦИКЛА ---
	CString filenameX, filenameY;
	// Формируем имя файла на основе bezierVariantIndex
	// bezierVariantIndex будет соответствовать (Index из ChildView - смещение)
	filenameX.Format(_T("Bezier_Curve_X_Variant%d.txt"), bezierVariantIndex);
	filenameY.Format(_T("Bezier_Curve_Y_Variant%d.txt"), bezierVariantIndex);

	WriteMatrixToFile(XB_curve_points, filenameX);
	WriteMatrixToFile(YB_curve_points, filenameY);

}



double Lagr(CMatrix& X, CMatrix& Y, double x, int size)
// double x: точка, в которой нужно вычислить значение полинома
// X: CMatrix (вектор) с x-координатами узлов интерполяции
// Y: CMatrix (вектор) с y-координатами узлов интерполяции
// size: количество узлов интерполяции
{
	double lagrange_pol = 0; // Переменная для накопления суммы L(x)
	double basics_pol;       // Переменная для вычисления текущего базисного полинома l_i(x)

	// Σ_{i=0..N}
	for (int i = 0; i < size; i++)
	{
		basics_pol = 1; 

		// Π_{j=0..N, j≠i}
		for (int j = 0; j < size; j++)
		{
			if (j == i) // j ≠ i
				continue;

			// Вычисляем (x - x_j) / (x_i - x_j)
			basics_pol *= (x - X(j)) / (X(i) - X(j));
		}
		// Добавляем к общей сумме произведение y_i * l_i(x)
		lagrange_pol += basics_pol * Y(i);
	}
	return lagrange_pol;
}



/*
	L(x) = Сумма от i = 0 до n [ yᵢ * lᵢ(x) ],

	где

	lᵢ(x) = Произведение по j = 0 до n, j ≠ i [ (x - xⱼ) / (xᵢ - xⱼ) ]
*/



// Эта функция вызывается из OnPaint. Она берет исходные точки this->X и this->Y (которые были заданы в OnLagr)
// Цель: Вычислить значения самого интерполяционного полинома Лагранжа L(x) в большом количестве точек, 
// чтобы его можно было плавно нарисовать на графике
void CPlot2D::DrawLagr(CDC& dc)
{
	// Исходные параметры для которых был построен полином
	double dx_orig = pi / 40; // Шаг, с которым были сгенерированы X, Y в OnLagr
	double xL_orig = 0;
	double xH_orig = pi;
	int N_nodes = (xH_orig - xL_orig) / dx_orig; // Количество узлов интерполяции N+1

	// Параметры для расчета точек самого полинома L(x) для отрисовки
	double dx_plot = 0.02; // Новый шаг для точек полинома
	double xL_plot = 0;   // Начало отрезка для L(x) (обычно совпадает с x_orig)
	double xH_plot = pi;    // Конец отрезка для L(x) (обычно совпадает с x_orig)
	int NL = (xH_plot - xL_plot) / dx_plot; // Количество интервалов для точек L(x)
	CMatrix XL(NL + 1); // Вектор X-координат для L(x)
	CMatrix YL(NL + 1); // Вектор Y-координат для L(x)

	for (int i = 0; i <= NL; i++) // Цикл для расчета точек полинома L(x)
	{
		XL(i) = xL_plot + i * dx_plot;
		// X, Y - это члены класса CPlot2D, установленные ранее через SetParams
		// N_nodes + 1 - это количество исходных точек (x_i, y_i)
		YL(i) = Lagr(X, Y, XL(i), N_nodes + 1);
	}

	// Запись точек ПОЛИНОМА Лагранжа в файл
	WriteMatrixToFile(XL, _T("Lagr_Polynomial_Points_X.txt"));
	WriteMatrixToFile(YL, _T("Lagr_Polynomial_Points_Y.txt"));

	// Отрисовка полинома
	double xs, ys;
	int xw, yw;
	xs = XL(0); ys = YL(0);
	GetWindowCoords(xs, ys, xw, yw); // Преобразование мировых координат в оконные
	CPen MyPen(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor); // Создание пера
	CPen* pOldPen = dc.SelectObject(&MyPen); // Выбор пера в контекст устройства
	dc.MoveTo(xw, yw); // Перемещение в начальную точку
	for (int i = 1; i < XL.rows(); i++) // Цикл по точкам полинома для рисования линий
	{
		xs = XL(i); ys = YL(i);
		GetWindowCoords(xs, ys, xw, yw);
		dc.LineTo(xw, yw); // Рисование линии к следующей точке
	}
	dc.SelectObject(pOldPen); // Восстановление старого пера
}

void CPlot2D::GetWindowCoords(double xs, double ys, int &xw, int &yw)
// Пересчитывает координаты точки из МСК в оконную
// xs - x- кордината точки в МСК
// ys - y- кордината точки в МСК
// xw - x- кордината точки в оконной СК
// yw - y- кордината точки в оконной СК

{
	CMatrix V(3), W(3);
	V(2) = 1;
	V(0) = xs;
	V(1) = ys;
	W = K * V;
	xw = (int)W(0);
	yw = (int)W(1);
}

void CPlot2D::SetPenAxis(CMyPen& PAxis)
// Установка параметров пера для линий осей 
{
	PenAxis.PenStyle = PAxis.PenStyle;
	PenAxis.PenWidth = PAxis.PenWidth;
	PenAxis.PenColor = PAxis.PenColor;
}

void CPlot2D::Draw(CDC& dc, int Ind1, int Ind2)
// Рисует график в режиме MM_TEXT - собственный пересчет координат
// dc - ссылка на класс CDC MFC
// Ind1=1/0 - рисовать/не рисовать рамку
// Ind2=1/0 - рисовать/не рисовать оси координат
{
	double xs, ys;  // мировые  координаты точки
	int xw, yw;     // оконные координаты точки
	if (Ind1 == 1)dc.Rectangle(RW);								 // Рамка в окне
	if (Ind2 == 1)	// Если нужны оси...
	{//***
		CPen MyPen(PenAxis.PenStyle, PenAxis.PenWidth, PenAxis.PenColor);
		CPen* pOldPen = dc.SelectObject(&MyPen);
		if (RS.left*RS.right < 0)							// Нужна Ось Y
		{
			xs = 0;  ys = RS.top;           // Точка (0,y_max) в МСК
			GetWindowCoords(xs, ys, xw, yw); // (xw,yw) -точка (0,y_max) в ОСК		
			dc.MoveTo(xw, yw);		      // Перо в точку (0,y_max)

			xs = 0;  ys = RS.bottom;        // Точка (0,y_min) в МСК
			GetWindowCoords(xs, ys, xw, yw); // (xw,yw) -точка (0,y_min) в ОСК
			dc.LineTo(xw, yw);		      // Линия (0,y_max) - (0,y_min) - Ось Y
		}

		if (RS.top*RS.bottom < 0)							// Нужна Ось X
		{
			xs = RS.left;  ys = 0;           // (xs,ys) - точка (x_min,0) в МСК
			GetWindowCoords(xs, ys, xw, yw); // (xw,yw) -точка (x_min,0) в ОСК
			dc.MoveTo(xw, yw);		         // Перо в точку (x_min,0)

			xs = RS.right;  ys = 0;          // (xs,ys) - точка (x_max,0) в МСК
			GetWindowCoords(xs, ys, xw, yw); // (xw,yw) -точка (x_max,0) в ОСК
			dc.LineTo(xw, yw);		         // Линия (x_min,0) - (x_max,0) - Ось X
		}
		dc.SelectObject(pOldPen);

	}

	xs = X(0); ys = Y(0);
	GetWindowCoords(xs, ys, xw, yw); // координаты начальной точки графика в ОСК
	CPen MyPen(PenLine.PenStyle, PenLine.PenWidth, PenLine.PenColor);
	CPen* pOldPen = dc.SelectObject(&MyPen);
	dc.MoveTo(xw, yw);        // Перо в начальную точка для рисования графика
	for (int i = 1; i < X.rows(); i++)
	{
		xs = X(i); ys = Y(i);
		GetWindowCoords(xs, ys, xw, yw); // координаты начальной точки графика с номером i в ОСК
		dc.LineTo(xw, yw);
	}
	dc.SelectObject(pOldPen);
}

void CRectD::SetRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

CSizeD CRectD::SizeD()
{
	CSizeD cz;
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области
	return cz;
}

CMatrix SpaceToWindow(CRectD& RS, CRect& RW)
// Функция обнавлена
// Возвращает матрицу пересчета координат из мировых в оконные
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


	M(0, 0) = kx;  M(0, 1) = 0;    M(0, 2) = (double)RW.left - kx * RS.left;			// Обновлено
	M(1, 0) = 0;   M(1, 1) = -ky;  M(1, 2) = (double)RW.bottom + ky * RS.bottom;		// Обновлено
	M(2, 0) = 0;   M(2, 1) = 0;    M(2, 2) = 1;
	return M;
}

void CPlot2D::SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX)
// XX - вектор данных по X 
// YY - вектор данных по Y 
// RWX - область в окне 
{
	int nRowsX = XX.rows();
	int nRowsY = YY.rows();
	X.RedimMatrix(nRowsX);
	Y.RedimMatrix(nRowsY);
	X = XX;
	Y = YY;
	double x_max = X.MaxElement();
	double x_min = X.MinElement();
	double y_max = Y.MaxElement();
	double y_min = Y.MinElement();
	RS.SetRectD(x_min, y_max, x_max, y_min);				// Область в мировой СК
	RW.SetRect(RWX.left, RWX.top, RWX.right, RWX.bottom);	// Область в окне
	K = SpaceToWindow(RS, RW);								// Матрица пересчета координат
}
