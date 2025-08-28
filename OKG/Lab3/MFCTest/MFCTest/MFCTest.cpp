
// MFCTest.cpp: определяет поведение классов для приложения.
//

// Этот файл содержит реализацию класса CMFCTestApp, который является основным классом приложения MFC

#include "pch.h"
#include "framework.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "MFCTest.h"
#include "MainFrm.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCTestApp

BEGIN_MESSAGE_MAP(CMFCTestApp, CWinApp) // карта сообщений
	ON_COMMAND(ID_APP_ABOUT, &CMFCTestApp::OnAppAbout)
END_MESSAGE_MAP()


// Создание CMFCTestApp

CMFCTestApp::CMFCTestApp() noexcept
{
	SetAppID(_T("Vodchyts.Lab3.NoVersion")); // идентификатор приложения
}

// Единственный объект CMFCTestApp

CMFCTestApp theApp; // глобальный объект, который представляет приложение


// Инициализация CMFCTestApp

BOOL CMFCTestApp::InitInstance()
{
	CWinApp::InitInstance(); // вызывает базовую реализацию инициализации приложения


	EnableTaskbarInteraction(FALSE); // отключает взаимодействие с панелью задач Windows

	SetRegistryKey(_T("OKG_Lab3")); // устанавливает ключ реестра для хранения настроек приложения


	CFrameWnd* pFrame = new CMainFrame; // объект главного окна
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame; // устанавливает созданное окно как главное окно приложения.
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr,
		nullptr); // загружает ресурсы главного окна (меню, панели инструментов и т.д.) и создает окно с указанными стилями


	// Разрешить использование расширенных символов в горячих клавишах меню
	CMFCToolBar::m_bExtCharTranslation = TRUE;

	pFrame->ShowWindow(SW_SHOW); // отображает главное окно
	pFrame->UpdateWindow();  // обновляет окно, чтобы оно отобразилось корректно
	return TRUE;
}

int CMFCTestApp::ExitInstance() // вызывается при завершении работы приложения
{
	return CWinApp::ExitInstance(); // базовая реализация
}

// Обработчики сообщений CMFCTestApp


// Диалоговое окно CAboutDlg используется для описания сведений о приложении
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg() noexcept;

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // метод для обмена данными между элементами управления и переменными

// Реализация
protected:
	DECLARE_MESSAGE_MAP() // объявляет карту сообщений для обработки событий
public:
	afx_msg void OnMymenuMenu2();
};

CAboutDlg::CAboutDlg() noexcept : CDialogEx(IDD_ABOUTBOX) // инициализирует диалоговое окно с идентификатором IDD_ABOUTBOX
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX) // метод для обмена данными между элементами управления и переменными
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx) // начинает карту сообщений для диалогового окна
	ON_COMMAND(ID_MYMENU_MENU2, &CAboutDlg::OnMymenuMenu2)
END_MESSAGE_MAP()

// Команда приложения для запуска диалога
void CMFCTestApp::OnAppAbout() // отображает диалоговое окно О программе
{
	CAboutDlg aboutDlg; // создает объект диалогового окна
	aboutDlg.DoModal(); // отображает диалоговое окно в модальном режиме (блокирует основное окно до закрытия диалога)
}

// Обработчики сообщений CMFCTestApp





void CAboutDlg::OnMymenuMenu2()
{



	// TODO: добавьте свой код обработчика команд
}
