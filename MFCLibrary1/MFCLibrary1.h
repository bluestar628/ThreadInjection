// MFCLibrary1.h : MFCLibrary1 DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFCLibrary1App
// �йش���ʵ�ֵ���Ϣ������� MFCLibrary1.cpp
//

class CMFCLibrary1App : public CWinApp
{
public:
	CMFCLibrary1App();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
