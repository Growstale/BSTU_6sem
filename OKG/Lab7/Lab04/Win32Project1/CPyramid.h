
#include "afxwin.h"
#include "CMatrix.h"

class CPyramid
{
	CMatrix Vertices;		// Координаты вершин
	void GetRect(CMatrix& Vert, CRectD&RectView);
	// Вычисляет координаты прямоугольника,охватывающего проекцию 
	// пирамиды на плоскость XY в ВИДОВОЙ системе координат
public:
	CPyramid(); 
	void Draw(CDC &dc, CMatrix &P, CRect &RW);	// Метод для отрисовки пирамиды БЕЗ удаления невидимых граней
	void Draw1(CDC &dc, CMatrix&P, CRect &RW);	// Метод для отрисовки пирамиды С УДАЛЕНИЕМ невидимых граней и закраской видимых
};

CMatrix CreateViewCoord(double r, double fi, double q);
// Создает матрицу преобразования из Мировой Системы Координат (МСК) в Видовую Систему Координат (ВСК)

CMatrix VectorMult(CMatrix & V1, CMatrix & V2);
// Вычисляет векторное произведение векторов

double ScalarMult(CMatrix& V1, CMatrix& V2);
// Вычисляет скалярное произведение векторов