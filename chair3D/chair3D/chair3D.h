
// chair3D.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//



#if !defined(AFX_GxSingleCamColor_H__42E4C164_A272_45E1_945D_981DE8DEE59C__INCLUDED_)
#define AFX_GxSingleCamColor_H__42E4C164_A272_45E1_945D_981DE8DEE59C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cchair3DApp: 
// �йش����ʵ�֣������ chair3D.cpp
//

class Cchair3DApp : public CWinApp
{
public:
	Cchair3DApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cchair3DApp theApp;
#endif