
// NavDataShowDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "CDMLogInfo.h"
#include "PointShow.h"
#include "Log.h"
#include "FileUtils.h"
#define WM_MNWRITELOG WM_USER+178
extern bool b_conSendOrNot;
// CNavDataShowDlg �Ի���
class CNavDataShowDlg : public CDialogEx
{
// ����
public:
	CNavDataShowDlg(CWnd* pParent = NULL);	// ��׼���캯��
	static CNavDataShowDlg* pNavDataShowDlg;
// �Ի�������
	enum { IDD = IDD_NAVDATASHOW_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnBraocastPara(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPoseDataDeal(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabControl;
	afx_msg void OnBnClickedBtnBroadcast();
	afx_msg void OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult);
	DWORD m_ip;
	CButton m_BrocastOper;
	int m_port;
	afx_msg void OnBnClickedBtnConnect();
	CButton m_Connnect;
	int MNPoseGetCount;
	int MNIniPoseNum;
	int MNContinuePoseNum;
	int MNVirtualPoseNum;
	int MNNGPoseNum;
	int MNPoseX;
	int MNPoseY;
	int MNPoseAngle;
	CString MNPoseTimestamp;
	int MNInvalidCount;
protected:
	afx_msg LRESULT OnMnwritelog(WPARAM wParam, LPARAM lParam);
	void OnMNCreateFile(CString filename);
	void OnMNPrepareStartTest(CString TestName);
public:
	afx_msg void OnBnClickedBtnLoaddata();
	afx_msg void OnBnClickedBtnOk();
	afx_msg void OnBnClickedBtnStoptest();
	BOOL m_recAcall;
	BOOL m_sendAscall;
	afx_msg void onSendAscallClicked();
	afx_msg void onSendHexClicked();
	BOOL m_checkperiod;
	int m_cycletime;
	CButton m_cycleControl;
};
