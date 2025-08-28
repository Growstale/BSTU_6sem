
// MainFrm.h: интерфейс класса CMainFrame
//

// Этот файл содержит описание класса CMainFrame, который наследуется от CFrameWnd — один из основных классов в библиотеке MFC, 
// который используется для создания и управления главным окном приложения
#pragma once
#include "ChildView.h"

class CMainFrame : public CFrameWnd
{
	
public:
	CMainFrame() noexcept; // конструктор класса
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// Атрибуты
public:

// Операции
public:

// Переопределение
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs); // вызывается перед созданием окна
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo); // обрабатывает команды

// Реализация
public:
	virtual ~CMainFrame(); // деструктор класса
#ifdef _DEBUG
	virtual void AssertValid() const; // для проверки валидности объекта в режиме отладки
	virtual void Dump(CDumpContext& dc) const; // для вывода отладочной информации об объекте в режиме отладки
#endif

protected:  // встроенные члены панели элементов управления
	CToolBar          m_wndToolBar; // панель инструментов
	CChildView    m_wndView; // окно представления - это область, где будет отображаться содержимое приложения

// Созданные функции схемы сообщений
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct); // обрабатывает сообщение, которое отправляется при создании окна
	afx_msg void OnSetFocus(CWnd *pOldWnd); // обрабатывает сообщение, которое отправляется, когда окно получает фокус
	DECLARE_MESSAGE_MAP() // объявляет карту сообщений, которая связывает сообщения Windows с функциями-обработчиками в классе

public:
//	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
};


