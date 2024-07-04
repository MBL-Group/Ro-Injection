// Picturecontrol.h : main header file for the PROJECT_NAME application
//
#pragma once
#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif
#include "resource.h"		// main symbols
// CPicturecontrolApp:
// See Picturecontrol.cpp for the implementation of this class
//
class CPicturecontrolApp : public CWinApp
{
public:
	CPicturecontrolApp();
	TCHAR		m_Path[_MAX_FNAME];
// Overrides
public:
	virtual BOOL InitInstance();
// Implementation
	DECLARE_MESSAGE_MAP()
};
extern CPicturecontrolApp theApp;