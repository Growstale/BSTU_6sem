#pragma once
#include <wingdi.h>
#include <wingdi.h>

double cosv1v2(CMatrix& one, CMatrix& two);

class CPyramid
{
	private:
		CMatrix Vertices; // ���������� ������
		void GetRect(CMatrix& Vert, CRectD&  RectView);

	public:
		CPyramid();
		void ColorDraw(CDC& dc, CMatrix& PView, CMatrix& PSourceLight, CRect RW, COLORREF Color);
};

CPyramid::CPyramid()
{
	Vertices.RedimMatrix(4, 6);

	/*       A                   B                    C                   A'                  B'                  C'      */
	Vertices(0, 0) = 0;	Vertices(0, 1) = 0;  Vertices(0, 2) = 3; Vertices(0, 3) = 0; Vertices(0, 4) = 0; Vertices(0, 5) = 1; 
	Vertices(1, 0) = 3; Vertices(1, 1) = 0;  Vertices(1, 2) = 0; Vertices(1, 3) = 1; Vertices(1, 4) = 0; Vertices(1, 5) = 0;  
	Vertices(2, 0) = 0; Vertices(2, 1) = 0;  Vertices(2, 2) = 0; Vertices(2, 3) = 3; Vertices(2, 4) = 3; Vertices(2, 5) = 3;  
	Vertices(3, 0) = 1; Vertices(3, 1) = 1;  Vertices(3, 2) = 1; Vertices(3, 3) = 1; Vertices(3, 4) = 1; Vertices(3, 5) = 1;  
}

void CPyramid::GetRect(CMatrix& Vert, CRectD& RectView) 
/*
������� ����������� � ������������ X, Y ���������� ����� ���������� ������ Vert
��������� ������������ � RectView. ��� ����� ��� SpaceToWindow, ����� ��������� �������������� �����������
*/
{
	CMatrix V = Vert.GetRow(0);               
	double xMin = V.MinElement();
	double xMax = V.MaxElement();
	V = Vert.GetRow(1);                    
	double yMin = V.MinElement();
	double yMax = V.MaxElement();
	RectView.SetRectD(xMin, yMax, xMax, yMin);
}

void CPyramid::ColorDraw(CDC& dc, CMatrix& PView, CMatrix& PSourceLight, CRect RW, COLORREF Color)
{
	BYTE red = GetRValue(Color);
	BYTE green = GetGValue(Color);
	BYTE blue = GetBValue(Color);
	double kLight;
	CRectD RectView;
	
	CMatrix VR = SphereToCart(PView); // ��������� ���������� ����������� � ���
	CMatrix VS = SphereToCart(PSourceLight);// ��������� ���������� ��������� ����� � ���
	CMatrix MV = CreateViewCoord(PView(0), PView(1), PView(2));// ������� �������� �� ��� � ���
	CMatrix ViewVert = MV * Vertices;
	GetRect(ViewVert, RectView); // ����������� ����������� �������������� �������� �������� � ���
	CMatrix MW = SpaceToWindow(RectView, RW); // ���->���
	
	CPoint MasVert[6];
	CMatrix V(3);
	V(2) = 1;
	for (int i = 0; i < 6; i++)	// �������������� � ���
	{
		V(0) = ViewVert(0, i); // x
		V(1) = ViewVert(1, i); // y
		V = MW * V;
		MasVert[i].x = (int)V(0);
		MasVert[i].y = (int)V(1);
	}

	CMatrix R1(3),	
		R2(3),	
		VN(3);	
	// ��������� ������� ��� �������� ��������� ������ (���������� �����) ����� � ������� ������� � �����
	double sm; // ��� �������� �������� ���������
	for (int i = 0; i < 3; i++) // ���� ����������� ��� ����, �� ������ ���� ��� ������ �� ���� ������� ������ ����������� ������
	{
		CMatrix VE = Vertices.GetCol(i + 3, 0, 2);	// ������� � �������� ���������
		int k;

		// ������������ ����������� ������� �� �������� ���������. ���� i=0 (A), �� k=1 (B). ���� i=1 (B), �� k=2 (C). ���� i=2 (C), �� k=0 (A)
		if (i == 2) k = 0;
		else k = i + 1;
		R1 = Vertices.GetCol(i, 0, 2);
		R2 = Vertices.GetCol(k, 0, 2);

		CMatrix V1 = R2 - R1;          // ������ � ����� � ���������
		CMatrix V2 = VE - R1;          // ������ � ����� � ������� � �������� ���������
		VN = VectorMult(V2, V1);
		sm = cosv1v2(VR, VN);

		if (sm >= 0) // ����� ������
		{
			CMatrix VP = VS - VR; // ������ �� ������ � ��������� �����
			sm = cosv1v2(VP, VN); // ������� ���� ����� (VS-VR) � �������� ����� VN
			double result = 0;
			if(sm > 0) // ���� ����� ��������
			{
					kLight = cosv1v2(VR, VN);
					result = pow(kLight, 1);
			}
			CPen Pen(PS_SOLID, 2, RGB(0,7*result*red, 0,7*result*green, 0,7*result*blue));	//������
			CPen* pOldPen = dc.SelectObject(&Pen);
			CBrush Brus(RGB(result * red, result * green, result * blue));
			CBrush* pOldBrush = dc.SelectObject(&Brus);
			CPoint MasVertr[4]{ MasVert[i], MasVert[k], MasVert[k + 3],MasVert[i + 3] };
			dc.Polygon(MasVertr, 4);	///���������
			dc.SelectObject(pOldPen);
			dc.SelectObject(pOldBrush);
		}
	}
	// ��������� ��������� ������ (������ � �������)
	VN = VectorMult(R1, R2);
	sm = cosv1v2(VN, VR);

	if (sm >= 0) // ������� ���������
	{
		CMatrix VP = VS - VR;
		sm = cosv1v2(VP, VN);
		double result = 0;
		if (sm > 0)
		{
			kLight = cosv1v2(VN, VR);;
			result = pow(kLight, 2);
		}
		if (result < 0) result = 0;
		CBrush* topBrush = new CBrush(RGB(result *red, result * green, result * blue));
		dc.SelectObject(topBrush);
		dc.Polygon(MasVert, 3);
	}
	else // ������ ���������
	{
		CMatrix VP = VS - VR;
		sm = cosv1v2(VP, VN);
		double result = 0;
		if (sm > 0)
		{
			kLight = cosv1v2(VN, VR);
			result = pow(kLight, 5);
		}
		CBrush* topBrush = new CBrush(RGB(result * result*red, result * result * green, result * result * blue));
		dc.SelectObject(topBrush);
		dc.Polygon(MasVert + 3, 3);	// ������� ���������
	}
}


double cosv1v2(CMatrix& one, CMatrix& two)
{
	double sc = ScalarMult(one, two);
	return sc / ((sqrt((pow(one(0, 0), 2) + pow(one(1, 0), 2) + pow(one(2, 0), 2))))*sqrt(pow(two(0, 0), 2) + pow(two(1, 0), 2) + pow(one(2, 0), 2)));
}
