// PointShow.cpp : 实现文件
//

#include "stdafx.h"
#include "NavDataShow.h"
#include "PointShow.h"
#include "afxdialogex.h"
// CPointShow 对话框
CSplitterWnd m_wndSplitter;
IMPLEMENT_DYNAMIC(CPointShow, CDialogEx)

CPointShow::CPointShow(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPointShow::IDD, pParent)
{
	
}


CPointShow::~CPointShow()
{
}

void CPointShow::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART1, m_MNChartrsTrack);
}


BEGIN_MESSAGE_MAP(CPointShow, CDialogEx)
END_MESSAGE_MAP()



// CPointShow 消息处理程序
