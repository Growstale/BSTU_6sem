#include "CMatrix.h"
#include "CPyramid.h"
#include <afxwin.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <vector>

#define ID_Dr1 2002
#define ID_Dr 2003
#define ID_AngF 2004
#define ID_AngD 2005


class CMainWin : public CFrameWnd
{
public:
	// Целочисленные переменные для хранения текущих параметров камеры
	int ang1 = 10;
	int ang2 = 315;
	int ang3 = 45;

	CMainWin()
	{
		Create(NULL, L"Lab07");
		Index = 0;
		PView.RedimMatrix(3);
		// Устанавливаем начальные значения PView при создании окна

		PView(0) = ang1;
		PView(1) = ang2;
		PView(2) = ang3;

	}
	CMenu *menu;
	CRect WinRect;
	CPyramid PIR;
	CMatrix PView;
	int Index;
	afx_msg int OnCreate(LPCREATESTRUCT);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	void Dr1();
	void Dr();
	void AngF();
	void AngD();
	DECLARE_MESSAGE_MAP();
};
class CMyApp : public CWinApp
{
public:
	CMyApp() {};
	virtual BOOL InitInstance()
	{
		m_pMainWnd = new CMainWin();
		int windowWidth = 800;  // 800 пикселей в ширину
		int windowHeight = 900; // 600 пикселей в высоту

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);
		int xPos = (screenWidth - windowWidth) / 2;
		int yPos = (screenHeight - windowHeight) / 2;

		m_pMainWnd->SetWindowPos(NULL, xPos, yPos, windowWidth, windowHeight, SWP_SHOWWINDOW);

		return TRUE;
	}
};
BEGIN_MESSAGE_MAP(CMainWin, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_COMMAND(ID_Dr1, Dr1)
	ON_COMMAND(ID_Dr, Dr)
	ON_COMMAND(ID_AngF, AngF)
	ON_COMMAND(ID_AngD, AngD)
	ON_WM_SIZE()
END_MESSAGE_MAP()

// Ключевой обработчик для отрисовки
// Вызывается, когда окно нуждается в перерисовке
afx_msg void CMainWin::OnPaint()
{
	CPaintDC dc(this);
	if (Index == 1) PIR.Draw(dc, PView, WinRect);
	if (Index == 2) PIR.Draw1(dc, PView, WinRect); // только видимые

}

afx_msg int CMainWin::OnCreate(LPCREATESTRUCT)
{
	menu = new CMenu();
	menu->CreateMenu();
	this->menu->AppendMenu(MF_STRING, ID_Dr, _T("Только видимые грани"));
	this->menu->AppendMenu(MF_STRING, ID_Dr1, _T("Все грани"));
	this->menu->AppendMenu(MF_STRING, ID_AngF, _T("Текущее положение камеры"));
	this->menu->AppendMenu(MF_STRING, ID_AngD, _T("Положение камеры по умолчанию"));

	SetMenu(menu);
	return 0;
}

void CMainWin::Dr1() // Все грани
{
	PView(0) = ang1;
	PView(1) = ang2;
	PView(2) = ang3;
	Index = 1;
	Invalidate();
}

void CMainWin::Dr() // Только видимые грани
{
	PView(0) = ang1;
	PView(1) = ang2;
	PView(2) = ang3;
	Index = 2;
	Invalidate();
}

void CMainWin::AngF() // Текущее положение камеры из файла
{
	std::ifstream file("angles.txt");
	std::string line;
	std::vector<std::string> lines;

	if (file.is_open()) {
		while (std::getline(file, line)) {

			lines.push_back(line);
		}
		file.close();
	}
	
	ang2 = stoi(lines[0]);
	ang3 = stoi(lines[1]);
	PView(1) = ang2;
	PView(2) = ang3;

	Invalidate();
}

void CMainWin::AngD()
{
	ang1 = 10;   
	ang2 = 315; 
	ang3 = 45; 

	PView(0) = ang1;
	PView(1) = ang2;
	PView(2) = ang3;
	Invalidate();
}
afx_msg void CMainWin::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy); // Вызываем базовый класс

	// Определяем прямоугольник для рисования (RW)
	int margin = 10;
	if (cx > 2 * margin && cy > 2 * margin)
	{
		WinRect.SetRect(margin, margin, cx - margin, cy - margin);
	}
	else // Если окно очень маленькое, занимаем его целиком
	{
		WinRect.SetRect(0, 0, cx, cy);
	}

}


CMyApp theApp;