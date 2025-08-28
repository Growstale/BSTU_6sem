#pragma once
#ifndef LIBSURFACE
#define LIBSURFACE 1
using namespace std;


typedef vector<CMatrix> CVecMatrix;
typedef vector<CVecMatrix> CMasMatrix;

typedef vector<CPoint> CVecPoint;
typedef vector<CVecPoint> CMatrPoint;


double Function1(double x, double y);
double Function2(double x, double y);
double Function3(double x, double y);

class CPlot3D
{
	pfunc2 pFunc;		    // Указатель на функцию f(x,y), описывающую поверхность
	CRectD SpaceRect;	    // Область определения функции в МСК
	CMasMatrix MatrF;		// Матрица для хранения координат точек (x,y,z,1) поверхности в МСК
	CMasMatrix MatrView;	// Матрица для хранения координат точек (x,y,1) проекции поверхности на плоскость XY ВСК
	CRectD ViewRect;		// Прямоугольная область, охватывающая проекцию поверхности на плоскость XY ВСК
	CRect WinRect;		    // Прямоугольная область в ОСК для рисования		
	CMatrix ViewPoint;		// Вектор (3x1) - координаты точки наблюдения в МСК
	CMatrPoint MatrWindow;  // Матрица для хранения оконных координат P(xi,yi) точек изображения 

public:
	CPlot3D();
	~CPlot3D() { pFunc = NULL; };
	void SetFunction(pfunc2 pF, CRectD RS, double dx, double dy);	
	// Устанавливает функцию f(x,y)
	// После установки вызывает CreateMatrF, CreateMatrView, CreateMatrWindow, т.е. полностью пересчитывает все данные для новой функции
	void SetViewPoint(double r, double fi, double q);	// Устанавливает положение точки наблюдения в МСК
	CMatrix GetViewPoint();				// Возвращает вектор ViewPoin
	void SetWinRect(CRect Rect);	// Устанавливает область в окне для рисования
	void CreateMatrF(double dx, double dy); // Заполняет матрицу MatrF координатами точек поверхности
	void SetMatrF(CMasMatrix& Matr);	// Задает значение матрицы MatrF извне
	void CreateMatrView(); // Заполняет матрицу MatrView координатами точек проекции поверхности на плоскость XY видовой СК
	void CreateMatrWindow(); // Заполняет матрицу MatrWindow 
	int GetNumberRegion(); // Определяет номер области для рисования
	void Draw(CDC& dc);
};

#endif


