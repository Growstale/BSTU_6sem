
// MFCTest.h: основной файл заголовка для приложения MFCTest
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CMFCTestApp:
// Сведения о реализации этого класса: MFCTest.cpp
//

class CMFCTestApp : public CWinApp
{
public:
	CMFCTestApp() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFCTestApp theApp;
