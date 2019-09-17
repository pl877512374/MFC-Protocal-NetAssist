#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "ProtocolAnalysis.h"
extern bool b_SendAscall;
extern NetConnectPara Curr_NetConnectPara;
// CLog 对话框

class CLog : public CDialogEx
{
	DECLARE_DYNAMIC(CLog)

public:
	CLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CLog();

// 对话框数据
	enum { IDD = IDD_DIALOG_LOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CRichEditCtrl m_richedit;
	afx_msg void OnBnClickedBtnClearsend();
	afx_msg void OnBnClickedBtnClearrecv();
	afx_msg void OnBnClickedBtnSenddata();
	CEdit m_richwrite;
	afx_msg void onTextChanged();
	CButton m_sendMsg;
};
