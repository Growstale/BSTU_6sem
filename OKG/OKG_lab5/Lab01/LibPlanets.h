#ifndef LIBPLANETS
#define LIBPLANETS 1
const double pi = 3.14159;


struct CSizeD
{
	double cx;
	double cy;
};
//-------------------------------------------------------------------------------
struct CRectD
{
	double left;
	double top;
	double right;
	double bottom;
	CRectD() { left = top = right = bottom = 0; };
	CRectD(double l, double t, double r, double b);
	void SetRectD(double l, double t, double r, double b);
	CSizeD SizeD();		// Возвращает размеры(ширина, высота) прямоугольника 
};
//-------------------------------------------------------------------------------

CMatrix CreateTranslate2D(double dx, double dy);
CMatrix CreateRotate2D(double fi);
CMatrix SpaceToWindow(CRectD& RS, const CRect& RW);
void SetMyMode(CDC& dc, CRectD& RS, CRect& RW);




class CBlade
{
	CRect MainPoint;
	CRect FirstTop;
	CRect SecondTop;
	CRect FirstBootom;
	CRect SecondBootom;
	CRect EarthOrbit;
	CMatrix FTCoords;
	CMatrix STCoords;
	CMatrix FBCoords;
	CMatrix SBCoords;
	CRect RW;		   // Прямоугольник в окне
	CRectD RS;		   // Прямоугольник области в МСК
	double wPoint;		//угловая скорость
	double fiSB;
	double fiFB;
	double fiST;
	double fiFT;
	// Для второй лопасти
	CMatrix FTCoords2, STCoords2, FBCoords2, SBCoords2;
	double fiSB2, fiFB2, fiST2, fiFT2;



	double dt;		   // Интервал дискретизации, сек.
public:
	CBlade();
	void DrawTriangle(CMatrix FCoords, CMatrix SCoords, CDC& dc, COLORREF color);
	void DrawTransformedTriangle(CMatrix FCoords, CMatrix SCoords, CMatrix& SW, CDC& dc, COLORREF color);
	void SetDT(double dtx) { dt = dtx; };	// Установка интервала дискретизации
	void SetNewCoords();					// Вычисляет новые координаты планет
	void GetRS(CRectD& RSX);				// Возвращает область графика в мировой СК
	CRect GetRW() { return RW; };			// Возвращает область графика в окне	
	void Draw(CDC& dc, const CRect& actualWindowRect);					// Рисование без самостоятельного пересчета координат
};


#endif

