
// NavDataShow.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CNavDataShowApp: 
// �йش����ʵ�֣������ NavDataShow.cpp
//

class CNavDataShowApp : public CWinApp
{
public:
	CNavDataShowApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CNavDataShowApp theApp;