
#include "afxwin.h"
#include "CMatrix.h"

class CPyramid
{
	CMatrix Vertices;		// ���������� ������
	CMatrix Nabcd;          // ������ ������� �������
	void GetRect(CMatrix& Vert, CRectD&RectView);
	// ��������� ���������� ��������������,������������� �������� 
	// �������� �� ��������� XY � ������� ������� ���������
public:
	CPyramid(); 
	void Draw(CDC &dc, CMatrix &P, CRect &RW);	// ������ �������� � ��������� ��������� �����

	void Draw1(CDC &dc, CMatrix&P, CRect &RW);	// ������ �������� ��� �������� ��������� �����
};

CMatrix CreateViewCoord(double r, double fi, double q);
// ������� ������� ��������� ����� �� ������� ������� ��������� � �������





CMatrix VectorMult(CMatrix & V1, CMatrix & V2);
// ��������� ��������� ������������ �������� V1 (3x1) � V2(3x1)

double ScalarMult(CMatrix& V1, CMatrix& V2);
// ��������� ��������� ������������ �������� V1(3x1) � V2(3x1)