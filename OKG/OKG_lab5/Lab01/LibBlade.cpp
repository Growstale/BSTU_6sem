#include "stdafx.h"

CRectD::CRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}
//------------------------------------------------------------------------------
void CRectD::SetRectD(double l, double t, double r, double b)
{
	left = l;
	top = t;
	right = r;
	bottom = b;
}

//------------------------------------------------------------------------------
CSizeD CRectD::SizeD()
{
	CSizeD cz;
	cz.cx = fabs(right - left);	// Ширина прямоугольной области
	cz.cy = fabs(top - bottom);	// Высота прямоугольной области
	return cz;
}

//----------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy)
// Формирует матрицу для преобразования координат объекта при его смещении 
// на dx по оси X и на dy по оси Y в фиксированной системе координат
// Матрица CreateTranslate2D(dx, dy) переносит объект на (dx, dy).
// Если нужно перенести СК, применяется обратная матрица CreateTranslate2D(-dx, -dy).
{
	CMatrix TM(3, 3);
	TM(0, 0) = 1; TM(0, 2) = dx;
	TM(1, 1) = 1;  TM(1, 2) = dy;
	TM(2, 2) = 1;
	return TM;
}

//------------------------------------------------------------------------------------
CMatrix CreateRotate2D(double fi)
// Формирует матрицу для преобразования координат объекта при его повороте
// на угол fi (при fi>0 против часовой стрелки)в фиксированной системе координат
// Матрица CreateRotate2D(fi) поворачивает объект на угол fi против часовой стрелки.
// Если нужно повернуть СК, применяется обратная матрица CreateRotate2D(-fi).
{
	double fg = fmod(fi, 360.0);
	double ff = (fg / 180.0) * pi; // Перевод в радианы
	CMatrix RM(3, 3);
	RM(0, 0) = cos(ff); RM(0, 1) = -sin(ff);
	RM(1, 0) = sin(ff);  RM(1, 1) = cos(ff);
	RM(2, 2) = 1;
	return RM;
}


//------------------------------------------------------------------------------

CMatrix SpaceToWindow(CRectD& RS, const CRect& RW) // Новая версия
{
	CMatrix M(3, 3);
	// CRect не имеет метода Size(), используем Width() и Height()
	int dwx = RW.Width(); // Ширина
	int dwy = RW.Height(); // Высота
	CSizeD szd = RS.SizeD(); // Размер области в МИРОВЫХ координатах

	double dsx = szd.cx; // Ширина в мировых координатах
	double dsy = szd.cy; // Высота в мировых координатах

	// Защита от деления на ноль, если размеры нулевые
	if (dsx == 0 || dsy == 0 || dwx == 0 || dwy == 0) {
			M(0, 0) = 1; M(1, 1) = 1; M(2, 2) = 1; // Например, единичная
		return M;
	}

	double kx = (double)dwx / dsx; // Масштаб по x
	double ky = (double)dwy / dsy; // Масштаб по y

	// Вычисляем центр мировой системы координат
	double rs_center_x = RS.left + dsx / 2.0;
	double rs_center_y = RS.bottom + dsy / 2.0; // У CRectD Y растет снизу вверх

	// Вычисляем центр оконной системы координат (у CRect Y растет сверху вниз)
	double rw_center_x = (double)RW.left + dwx / 2.0;
	double rw_center_y = (double)RW.top + dwy / 2.0; // Используем RW.top

	// Вычисляем необходимое смещение
	// Мировая точка (rs_center_x, rs_center_y) должна отобразиться в (rw_center_x, rw_center_y)
	// Уравнения:
	// rw_center_x = kx * rs_center_x + dx  => dx = rw_center_x - kx * rs_center_x
	// rw_center_y = -ky * rs_center_y + dy => dy = rw_center_y + ky * rs_center_y
	double dx = rw_center_x - kx * rs_center_x;
	double dy = rw_center_y + ky * rs_center_y; // Учитываем инверсию Y (-ky)

	M(0, 0) = kx;	 M(0, 1) = 0;  M(0, 2) = dx;
	M(1, 0) = 0;  M(1, 1) = -ky;  M(1, 2) = dy; // Инверсия Y
	M(2, 0) = 0;  M(2, 1) = 0;	M(2, 2) = 1;
	return M;
}

//------------------------------------------------------------------------------

void SetMyMode(CDC& dc, CRectD& RS, CRect& RW)
// Устанавливает режим отображения MM_ANISOTROPIC и его параметры
// dc - ссылка на класс CDC MFC
// RS -  область в мировых координатах - int
// RW -	 Область в оконных координатах - int  
{
	double dsx = RS.right - RS.left;
	double dsy = RS.top - RS.bottom;
	double xsL = RS.left;
	double ysL = RS.bottom;

	int dwx = RW.right - RW.left;
	int dwy = RW.bottom - RW.top;
	int xwL = RW.left;
	int ywH = RW.bottom;

	dc.SetMapMode(MM_ANISOTROPIC);
	dc.SetWindowExt((int)dsx, (int)dsy);
	dc.SetViewportExt(dwx, -dwy);
	dc.SetWindowOrg((int)xsL, (int)ysL);
	dc.SetViewportOrg(xwL, ywH);
}



CBlade::CBlade()
{
	double rS = 30;
	double RoE = 10 * rS;
	RS.SetRectD(-RoE, RoE, RoE, -RoE);		    // Область системы в мировых координатах
	// Изменяем RW на основе размера RS
	RW.SetRect(0, 0, static_cast<int>(RS.right - RS.left), static_cast<int>(RS.top - RS.bottom));
	MainPoint.SetRect(-rS, rS, rS, -rS);
	FirstTop.SetRect(-5, 5, 5, -5);
	SecondTop.SetRect(-5, 5, 5, -5);
	FirstBootom.SetRect(-5, 5, 5, -5);
	SecondBootom.SetRect(-5, 5, 5, -5);
	EarthOrbit.SetRect(-RoE, RoE, RoE, -RoE);
	fiSB = 215;
	fiFB = 205;
	fiST = 25;
	fiFT = 35;
	wPoint = -8;
	dt = 1;
	FTCoords.RedimMatrix(3);
	STCoords.RedimMatrix(3);
	FBCoords.RedimMatrix(3);
	SBCoords.RedimMatrix(3);

	// Для второй лопасти
	FTCoords2.RedimMatrix(3);
	STCoords2.RedimMatrix(3);
	FBCoords2.RedimMatrix(3);
	SBCoords2.RedimMatrix(3);
}

void CBlade::SetNewCoords()
{
	double RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	double ff = (fiFT / 90.0) * pi;
	double x = RoV * cos(ff);
	double y = RoV * sin(ff);
	FTCoords(0) = x;
	FTCoords(1) = y;
	FTCoords(2) = 1;

	fiFT += wPoint * dt;
	CMatrix P = CreateRotate2D(fiFT);
	FTCoords = P * FTCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiST / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	STCoords(0) = x;
	STCoords(1) = y;
	STCoords(2) = 1;

	fiST += wPoint * dt;
	P = CreateRotate2D(fiST);
	STCoords = P * STCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiFB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	FBCoords(0) = x;
	FBCoords(1) = y;
	FBCoords(2) = 1;

	fiFB += wPoint * dt;
	P = CreateRotate2D(fiFB);
	FBCoords = P * FBCoords;

	RoV = (EarthOrbit.right - EarthOrbit.left) / 2;
	ff = (fiSB / 90.0) * pi;
	x = RoV * cos(ff);
	y = RoV * sin(ff);
	SBCoords(0) = x;
	SBCoords(1) = y;
	SBCoords(2) = 1;

	// Для второй лопасти
	fiSB += wPoint * dt;
	P = CreateRotate2D(fiSB);
	SBCoords = P * SBCoords;

	fiFT2 += wPoint * dt;
	CMatrix P2 = CreateRotate2D(fiFT2);
	FTCoords2 = P2 * FTCoords;

	fiST2 += wPoint * dt;
	P2 = CreateRotate2D(fiST2);
	STCoords2 = P2 * STCoords;

	fiFB2 += wPoint * dt;
	P2 = CreateRotate2D(fiFB2);
	FBCoords2 = P2 * FBCoords;

	fiSB2 += wPoint * dt;
	P2 = CreateRotate2D(fiSB2);
	SBCoords2 = P2 * SBCoords;

}


void CBlade::Draw(CDC& dc, const CRect& actualWindowRect) // Новая версия
{
	CBrush SBrush(RGB(0, 255, 0));
	CBrush EBrush(RGB(255, 0, 0));
	CBrush MBrush(RGB(0, 0, 255));

	// Используем переданный actualWindowRect вместо члена RW
	CMatrix SW = SpaceToWindow(RS, actualWindowRect);

	dc.SelectStockObject(NULL_BRUSH);

	// Преобразуем и рисуем орбиту
	POINT orbitPoints[2];
	orbitPoints[0].x = (int)(SW(0, 0) * EarthOrbit.left + SW(0, 2));
	orbitPoints[0].y = (int)(SW(1, 1) * EarthOrbit.top + SW(1, 2)); // Используем top для Y
	orbitPoints[1].x = (int)(SW(0, 0) * EarthOrbit.right + SW(0, 2));
	orbitPoints[1].y = (int)(SW(1, 1) * EarthOrbit.bottom + SW(1, 2)); // Используем bottom для Y

	// Преобразуем координаты эллипса правильно (left, top, right, bottom)
	CRect transformedEarthOrbit;
	transformedEarthOrbit.left = min(orbitPoints[0].x, orbitPoints[1].x);
	transformedEarthOrbit.top = min(orbitPoints[0].y, orbitPoints[1].y); // min Y -> top
	transformedEarthOrbit.right = max(orbitPoints[0].x, orbitPoints[1].x);
	transformedEarthOrbit.bottom = max(orbitPoints[0].y, orbitPoints[1].y); // max Y -> bottom

	dc.Ellipse(transformedEarthOrbit);

	// Первая лопасть (красная)
	DrawTransformedTriangle(FTCoords, STCoords, SW, dc, RGB(255, 0, 0));
	DrawTransformedTriangle(FBCoords, SBCoords, SW, dc, RGB(255, 0, 0));

	// Вторая лопасть (синяя)
	DrawTransformedTriangle(FTCoords2, STCoords2, SW, dc, RGB(0, 0, 255));
	DrawTransformedTriangle(FBCoords2, SBCoords2, SW, dc, RGB(0, 0, 255));

	// Центральная точка (зелёная)
	CBrush* pOldBrush = dc.SelectObject(&SBrush);
	POINT centerPoint[2];
	centerPoint[0].x = (int)(SW(0, 0) * MainPoint.left + SW(0, 2));
	centerPoint[0].y = (int)(SW(1, 1) * MainPoint.top + SW(1, 2));
	centerPoint[1].x = (int)(SW(0, 0) * MainPoint.right + SW(0, 2));
	centerPoint[1].y = (int)(SW(1, 1) * MainPoint.bottom + SW(1, 2));

	// Преобразуем координаты эллипса правильно (left, top, right, bottom)
	CRect transformedMainPoint;
	transformedMainPoint.left = min(centerPoint[0].x, centerPoint[1].x);
	transformedMainPoint.top = min(centerPoint[0].y, centerPoint[1].y);
	transformedMainPoint.right = max(centerPoint[0].x, centerPoint[1].x);
	transformedMainPoint.bottom = max(centerPoint[0].y, centerPoint[1].y);

	dc.Ellipse(transformedMainPoint);
	dc.SelectObject(pOldBrush);
}
void CBlade::DrawTransformedTriangle(CMatrix FCoords, CMatrix SCoords, CMatrix& SW, CDC& dc, COLORREF color) {
	CBrush TBrush(color);
	CBrush* pOldBrush = dc.SelectObject(&TBrush);

	POINT points[3];
	points[0].x = (int)(SW(0, 2)); // Начало координат после преобразования
	points[0].y = (int)(SW(1, 2));
	points[1].x = (int)(SW(0, 0) * FCoords(0) + SW(0, 1) * FCoords(1) + SW(0, 2));
	points[1].y = (int)(SW(1, 0) * FCoords(0) + SW(1, 1) * FCoords(1) + SW(1, 2));
	points[2].x = (int)(SW(0, 0) * SCoords(0) + SW(0, 1) * SCoords(1) + SW(0, 2));
	points[2].y = (int)(SW(1, 0) * SCoords(0) + SW(1, 1) * SCoords(1) + SW(1, 2));

	dc.Polygon(points, 3);

	dc.SelectObject(pOldBrush); // Восстанавливаем кисть
}


void CBlade::GetRS(CRectD& RSX)
// RS - структура, куда записываются параметры области графика
{
	RSX.left = RS.left;
	RSX.top = RS.top;
	RSX.right = RS.right;
	RSX.bottom = RS.bottom;
}