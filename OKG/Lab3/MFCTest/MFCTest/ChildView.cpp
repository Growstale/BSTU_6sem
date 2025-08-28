
// ChildView.cpp: реализация класса CChildView
//

// Этот класс отвечает за отображение графики и обработку сообщений в дочернем окне приложения MFC


#include "pch.h"
#include "framework.h"
#include "MFCTest.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	Index = 0; // переменная используется для определения того, что именно нужно отобразить в окне
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd) // начинает карту сообщений
	ON_WM_PAINT() // связывает сообщение WM_PAINT (перерисовка окна) с методом OnPaint()
	ON_COMMAND(ID_MYMENU_MENU1, &CChildView::OnMymenuMenu1)
	ON_COMMAND(ID_MYMENU_MENU2, &CChildView::OnMymenuMenu2)
	ON_COMMAND(ID_MYMENU_MENU3, &CChildView::OnMymenuMenu3)
	ON_WM_LBUTTONDBLCLK() // связывает двойной клик левой кнопкой мыши с методом OnLButtonDblClk
END_MESSAGE_MAP()



// Обработчики сообщений CChildView

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)  // вызывается перед созданием окна
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE; // добавляет стиль WS_EX_CLIENTEDGE, который создает границу вокруг окна
	cs.style &= ~WS_BORDER; // убирает стиль WS_BORDER (границу окна)
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS,  // регистрирует класс окна с использованием MFC
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);
	// Окно перерисовывается при изменении размера
	// Окно обрабатывает двойные клики
	// Устанавливает стандартный курсор
	// Устанавливает цвет фона окна

	return TRUE;
}

void CChildView::OnPaint() // перерисовка окна
{
	CPaintDC dc(this); // контекст устройства для рисования

	if (Index == 1) // если Index равен 1, рисуется ломаная линия
	{
		Graph.Draw(dc, 1, 1);
	}

	if (Index == 2) // режим отображения MM_TEXT для окружности и восьмиугольника
	{
		GraphCircle.Draw(dc, 1, 1); // Рисуем окружность с рамкой и осями
		GraphOctagon.Draw(dc, 0, 0);
	}
}

double CChildView::MyF1(double x)
{
	double y = sin(x) / x;
	return y;
}

double CChildView::MyF2(double x)
{
	double y = sqrt(fabs(x)) * sin(x);
	return y;
}

void CChildView::OnMymenuMenu1() 
{
	
	double Xl = -3 * pi;		// Координата Х левого угла области
	double Xh = 3 * pi;			// Координата Х правого угла области
	double dX = pi / 36;		// Шаг графика функции
	int N = (Xh - Xl) / dX;		// Количество точек графика
	X.RedimMatrix(N + 1);		// Создаем вектор с N+1 строками для хранения координат точек по Х
	Y.RedimMatrix(N + 1);		// Создаем вектор с N+1 строками для хранения координат точек по Y
	for (int i = 0; i <= N; i++)
	{
		X(i) = Xl + i * dX;		// Заполняем массивы/векторы значениями
		Y(i) = MyF1(X(i));
	}
	PenLine.Set(PS_SOLID, 1, RGB(255, 0, 0));	// Устанавливаем параметры пера для линий (сплошная линия, толщина 1, цвет красный)
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 255));	// Устанавливаем параметры пера для осей (сплошная линия, толщина 2, цвет синий)

	RW.SetRect(100, 100, 500, 500);				// Установка параметров прямоугольной области для отображения графика в окне
	Graph.SetParams(X, Y, RW);					// Передаем векторы с координатами точек и область в окне
	Graph.SetPenLine(PenLine);					// Установка параметров пера для линии графика
	Graph.SetPenAxis(PenAxis);					// Установка параметров пера для линий осей 
	Index = 1;									// Помечаем для режима отображения MM_TEXT
	this->Invalidate();

}

void CChildView::OnMymenuMenu2()
{
	double Xl = -4 * pi;		// Координата Х левого угла области
	double Xh = 4 * pi;			// Координата Х правого угла области
	double dX = pi / 36;		// Шаг графика функции
	int N = (Xh - Xl) / dX;
	X.RedimMatrix(N + 1);
	Y.RedimMatrix(N + 1);
	for (int i = 0; i <= N; i++)
	{
		X(i) = Xl + i * dX;
		Y(i) = MyF2(X(i));
	}
	PenLine.Set(PS_DASHDOT, 3, RGB(255, 0, 0));
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 0));
	RW.SetRect(100, 100, 500, 500);
	Graph.SetParams(X, Y, RW);
	Graph.SetPenLine(PenLine);
	Graph.SetPenAxis(PenAxis);
	Index = 1;
	this->Invalidate();
}


void CChildView::OnMymenuMenu3()
{
	const double radius = 10.0; // Радиус окружности
	const int numSides = 8;     // Количество сторон восьмиугольника
	const double angleIncrement = 2 * pi / numSides; // Угловой шаг

	// Создаем массивы для хранения координат вершин восьмиугольника
	CMatrix octagonX(numSides + 1), octagonY(numSides + 1);

	// Вычисляем координаты вершин восьмиугольника
	for (int i = 0; i < numSides; i++)
	{
		double angle = i * angleIncrement;
		octagonX(i) = radius * cos(angle);
		octagonY(i) = radius * sin(angle);
	}
	octagonX(numSides) = octagonX(0); // Замыкаем фигуру
	octagonY(numSides) = octagonY(0);

	// Окружность рисуется аналогично, только вместо 8 точек используем 100 точек (для плавности)
	const int numPoints = 100; // Количество точек для аппроксимации окружности
	CMatrix circleX(numPoints + 1), circleY(numPoints + 1);

	// Вычисляем координаты окружности
	for (int i = 0; i < numPoints; i++)
	{
		double angle = 2 * pi * i / numPoints;
		circleX(i) = radius * cos(angle);
		circleY(i) = radius * sin(angle);
	}
	circleX(numPoints) = circleX(0); // Замыкаем окружность
	circleY(numPoints) = circleY(0);

	// Устанавливаем прямоугольную область для отображения графика
	RW.SetRect(100, 100, 500, 500);

	// Отрисовка окружности
	PenAxis.Set(PS_SOLID, 2, RGB(0, 0, 255)); // Синий цвет для окружности
	GraphCircle.SetParams(circleX, circleY, RW); // Передаем координаты окружности
	GraphCircle.SetPenLine(PenAxis); // Устанавливаем параметры пера для линии графика (окружности)

	// Отрисовка восьмиугольника
	PenLine.Set(PS_SOLID, 3, RGB(255, 0, 0)); // Красный цвет для восьмиугольника
	GraphOctagon.SetParams(octagonX, octagonY, RW); // Передаем координаты восьмиугольника
	GraphOctagon.SetPenLine(PenLine); // Устанавливаем параметры пера для восьмиугольника

	Index = 2; // Помечаем для режима отображения MM_TEXT
	this->Invalidate(); // Обновляем окно для отображения графика
}





void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	Index = 0; // сбрасывает Index в 0, что приводит к очистке окна
	Invalidate(); // перерисовывает окно, вызывая метод OnPaint
	 
	CWnd::OnLButtonDblClk(nFlags, point); // вызывает базовую реализацию обработчика
}