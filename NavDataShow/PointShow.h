#pragma once
#include "tchart1.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CSeries.h"
#include "EasySize.h"
// CPointShow �Ի���

class CPointShow : public CDialogEx
{
	DECLARE_DYNAMIC(CPointShow)

public:
	CPointShow(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CPointShow();
// �Ի�������
	enum { IDD = IDD_DIALOG_POINTSHOW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	DECLARE_MESSAGE_MAP()
public:
	CTchart1 m_MNChartrsTrack;
};
