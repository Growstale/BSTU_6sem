#pragma once

#define LIBGRAPH 1
const double pi = 3.14159;
#include "pch.h"

struct CSizeD // представляет размеры (ширину и высоту) с использованием значений типа double
{
	double cx;
	double cy;
};

struct CRectD // представляет прямоугольник с координатами типа double
	// используется для описания областей в мировых координатах
{
	double left;
	double top;
	double right;
	double bottom;
	CRectD() { left = top = right = bottom = 0; }; // конструктор без параметров
	CRectD(double l, double t, double r, double b); // конструктор с параметрами
	void SetRectD(double l, double t, double r, double b); // метод для установки координат
	CSizeD SizeD();	// Возвращает размеры(ширина, высота) прямоугольника 
};


struct CMyPen // структура CMyPen используется для описания параметров пера (линии)
{
	int PenStyle;		// Стиль пера
	int PenWidth;		// Толщина пера 
	COLORREF PenColor;	// Цвет пера 
	CMyPen() { PenStyle = PS_SOLID; PenWidth = 1; PenColor = RGB(0, 0, 0); }; // сплошная линия, толщина 1, цвет чёрный
	void Set(int PS, int PW, COLORREF PC)
	{
		PenStyle = PS; PenWidth = PW; PenColor = PC;
	};
};

CMatrix SpaceToWindow(CRectD& rs, CRect& rw);


class CPlot2D
	// Этот класс предназначен для создания и отображения двумерных графиков в оконной системе координат. 
	// Он использует матрицы для пересчета координат из мировой системы координат (МСК) в оконную систему координат (ОСК) 
	// и предоставляет методы для настройки параметров графика, осей и пера
{
	CMatrix X;				// Аргумент
	CMatrix Y;				// Функция
	CMatrix K;				// Матрица преобразования координат из мировой системы координат (МСК) в оконную систему координат (ОСК)
	CRect RW;				// Прямоугольник, задающий область в окне, где будет отображаться график
	CRectD RS;				// Прямоугольник, задающий область в мировой системе координат (МСК)
	CMyPen PenLine;			// Перо для рисования графика
	CMyPen PenAxis;			// Перо для осей
public:
	CPlot2D() { K.RedimMatrix(3, 3); };			//Конструктор по умолчанию. Инициализирует матрицу преобразования K размером 3x3
	void SetParams(CMatrix& XX, CMatrix& YY, CRect& RWX);	// Установка параметров графика
	void SetWindowRect(CRect& RWX);				//Установка области в окне для отображения графика
	void GetWindowCoords(double xs, double ys, int& xw, int& yw);	//Пересчет координаты точки из МСК в оконную СК
	void SetPenLine(CMyPen& PLine);				// Перо для рисования графика
	void SetPenAxis(CMyPen& PAxis);				// Перо для осей координат
	void Draw(CDC& dc, int Ind1, int Int2);		// Рисование с самостоятельным пересчетом координат
};
