#pragma once

#include "main_wnd.h"

class CMyApp : public CWinApp
{
public:
	CMyApp() {}
	virtual BOOL InitInstance() {
		m_pMainWnd = new CMainWnd();
		ASSERT(m_pMainWnd);
		m_pMainWnd->ShowWindow(SW_SHOW);
		return TRUE;
	}
};