
// zlibCompress.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CzlibCompressApp:
// �йش����ʵ�֣������ zlibCompress.cpp
//

class CzlibCompressApp : public CWinAppEx
{
public:
	CzlibCompressApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CzlibCompressApp theApp;