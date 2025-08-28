
// ChildView.h: интерфейс класса CChildView
//


#pragma once


// Окно CChildView

class CChildView : public CWnd
{
// Создание
public:
	CString ss1,ss2; // хранения текстовых строк
	CChildView();

// Атрибуты
public:
	int Index; // используется для управления состоянием отрисовки
	CMatrix X, Y; // матрицы, которые используются для хранения данных графиков
	CRect RW; // прямоугольник в оконной системе координат
	CRectD RS; // прямоугольник в мировой системе координат
	CPlot2D Graph; // для управления и отрисовки двумерных графиков
	CPlot2D GraphCircle;    // График для окружности
	CPlot2D GraphOctagon;   // График для восьмиугольника
	CMyPen PenLine, PenAxis; // перья для рисования
// Операции
public:
	double MyF1(double x);
	double MyF2(double x);
	double MyF3(double x);
// Переопределение
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Реализация
public:
	virtual ~CChildView();

	// Созданные функции схемы сообщений
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMymenuMenu1();
	afx_msg void OnMymenuMenu2();
	afx_msg void OnMymenuMenu3();

	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};

