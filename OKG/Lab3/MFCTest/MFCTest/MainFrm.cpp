
// MainFrm.cpp: реализация класса CMainFrame
//

#include "pch.h"
#include "framework.h"
#include "MFCTest.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd) // Начинает карту сообщений
	ON_WM_CREATE() // создание окна будет обрабатываться функцией OnCreate
	ON_WM_SETFOCUS() // получение фокуса окном будет обрабатываться функцией OnSetFocus
//	ON_WM_LBUTTONDBLCLK() // двойной клик левой кнопкой мыши будет обрабатываться функцией OnLButtonDblClk
END_MESSAGE_MAP()


CMainFrame::CMainFrame() noexcept // конструктор класса CMainFrame
{

}

CMainFrame::~CMainFrame() // деструктор класса CMainFrame
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) // вызывается при создании окна
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1) // окно не удалось создать
		return -1;

	// создает область представления (CChildView)
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("Не удалось создать окно представлений\n");
		return -1;
	}

	// создать панель инструментов
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Не удалось создать панель инструментов\n");
		return -1;      // не удалось создать
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY); // включает возможность закрепления панели инструментов
	EnableDocking(CBRS_ALIGN_ANY); // включает возможность закрепления для главного окна
	DockControlBar(&m_wndToolBar); // закрепляет панель инструментов в главном окне


	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) // вызывается перед созданием окна
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE; // убирает стиль WS_EX_CLIENTEDGE, который добавляет границу вокруг окна
	cs.lpszClass = AfxRegisterWndClass(0); // регистрирует класс окна с использованием MFC
	return TRUE;
}

// Диагностика CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const // проверяет, что объект валиден (используется в режиме отладки)
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const // выводит отладочную информацию об объекте (используется в режиме отладки)
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// Обработчики сообщений CMainFrame

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/) // вызывается, когда окно получает фокус
{
	// передача фокуса окну представления
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) // обрабатывает команды (например, нажатие кнопки)
{
	// разрешить ошибки в представлении при выполнении команды
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo)) 
		return TRUE;

	// в противном случае выполняется обработка по умолчанию
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}
