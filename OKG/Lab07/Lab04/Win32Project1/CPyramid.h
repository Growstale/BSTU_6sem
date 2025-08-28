
#include "afxwin.h"
#include "CMatrix.h"

class CPyramid
{
	CMatrix Vertices;		// Координаты вершин
	CMatrix Nabcd;          // Вектор внешней нормали
	void GetRect(CMatrix& Vert, CRectD&RectView);
	// Вычисляет координаты прямоугольника,охватывающего проекцию 
	// пирамиды на плоскость XY в ВИДОВОЙ системе координат
public:
	CPyramid(); 
	void Draw(CDC &dc, CMatrix &P, CRect &RW);	// Рисует пирамиду С УДАЛЕНИЕМ невидимых ребер

	void Draw1(CDC &dc, CMatrix&P, CRect &RW);	// Рисует пирамиду БЕЗ удаления невидимых ребер
};

CMatrix CreateViewCoord(double r, double fi, double q);
// Создает матрицу пересчета точки из мировой системы координат в видовую





CMatrix VectorMult(CMatrix & V1, CMatrix & V2);
// Вычисляет векторное произведение векторов V1 (3x1) и V2(3x1)

double ScalarMult(CMatrix& V1, CMatrix& V2);
// Вычисляет скалярное произведение векторов V1(3x1) и V2(3x1)