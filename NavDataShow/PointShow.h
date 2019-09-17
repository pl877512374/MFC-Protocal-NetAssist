#pragma once
#include "tchart1.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CSeries.h"
#include "EasySize.h"
// CPointShow 对话框

class CPointShow : public CDialogEx
{
	DECLARE_DYNAMIC(CPointShow)

public:
	CPointShow(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CPointShow();
// 对话框数据
	enum { IDD = IDD_DIALOG_POINTSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	DECLARE_MESSAGE_MAP()
public:
	CTchart1 m_MNChartrsTrack;
};
