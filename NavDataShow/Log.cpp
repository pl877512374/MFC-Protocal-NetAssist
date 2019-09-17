// Log.cpp : 实现文件
//

#include "stdafx.h"
#include "NavDataShow.h"
#include "Log.h"
#include "afxdialogex.h"
#include "NavDataShowDlg.h"
// CLog 对话框
bool b_conSendOrNot = false;
IMPLEMENT_DYNAMIC(CLog, CDialogEx)
CNavDataShowDlg *pWnd;
CLog::CLog(CWnd* pParent /*=NULL*/)
: CDialogEx(CLog::IDD, pParent)
{

}

CLog::~CLog()
{
}

void CLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT21, m_richedit);
	DDX_Control(pDX, IDC_EDIT1, m_richwrite);
	DDX_Control(pDX, IDC_BTN_SENDDATA, m_sendMsg);
}


BEGIN_MESSAGE_MAP(CLog, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_CLEARSEND, &CLog::OnBnClickedBtnClearsend)
	ON_BN_CLICKED(IDC_BTN_CLEARRECV, &CLog::OnBnClickedBtnClearrecv)
	ON_BN_CLICKED(IDC_BTN_SENDDATA, &CLog::OnBnClickedBtnSenddata)
	ON_EN_CHANGE(IDC_EDIT1, &CLog::onTextChanged)
END_MESSAGE_MAP()


// CLog 消息处理程序


void CLog::OnBnClickedBtnClearsend()
{
	// TODO:  在此添加控件通知处理程序代码
	m_richwrite.SetSel(0, -1);
	m_richwrite.Clear();
}


void CLog::OnBnClickedBtnClearrecv()
{
	// TODO:  在此添加控件通知处理程序代码
	m_richedit.SetSel(0, -1);
	m_richedit.Clear();
}

char ConvertHexChar(char ch)
{
	if ((ch >= '0') && (ch <= '9'))
		return ch - 0x30;
	else if ((ch >= 'A') && (ch <= 'F'))
		return ch - 'A' + 10;
	else if ((ch >= 'a') && (ch <= 'f'))
		return ch - 'a' + 10;
	else return (-1);
}
typedef  struct u_Str2HexRetVal
{
	int len;
	char* senfbuf;
}Str2HexRetVal;
Str2HexRetVal String2Hex(CString str)
{
	Str2HexRetVal retVal;
	char* senddata;
	int hexdata, lowhexdata;
	int hexdatalen = 0;
	int len = str.GetLength();
	char lstr, hstr;
	for (int i = 0; i < len;)
	{
		while (str[i] == ' ')
		{
			i++;
			continue;
		}
		hstr = str[i];
		i++;
		if (i > len)
			break;
		else if (i == len)
		{
			hexdata = ConvertHexChar(hstr);
			if (hexdata == 16)
			{
				break;
			}
		}
		else
		{
			lstr = str[i];
			while (lstr == ' '&&i < len)
			{
				i++;
				lstr = str[i];
				continue;
			}
			lstr = str[i];
			lowhexdata = ConvertHexChar(lstr);
			hexdata = ConvertHexChar(hstr);
			if ((hexdata == 16) || (lowhexdata == 16))
				break;
			else
				hexdata = hexdata * 16 + lowhexdata;
			i++;
		}
		senddata[hexdatalen] = hexdata;
		hexdatalen++;
	}
	retVal.senfbuf = senddata;
	retVal.len = hexdatalen;
	return retVal;

}

UINT FuncThreadSend(LPVOID lpParam)
{
	CLog* pclog = (CLog*)lpParam;
	CString str_send;
	pclog->m_richwrite.GetWindowText(str_send);
	if (b_SendAscall)
	{
		Str2HexRetVal sendVal = String2Hex(str_send);
		Curr_NetConnectPara.Senddata = sendVal.senfbuf;
		Curr_NetConnectPara.Len_Senddata = sendVal.len;
	}
	else
	{
		Curr_NetConnectPara.Senddata = new char[1000];
		Curr_NetConnectPara.Len_Senddata = str_send.GetLength();
		memcpy(Curr_NetConnectPara.Senddata, str_send, str_send.GetLength());
	}
	if (pWnd)
	{
		pWnd->UpdateData(true);
		while (pWnd->m_checkperiod&&b_conSendOrNot)
		{
			pWnd->UpdateData(true);
			if (b_SendAscall)//发16进制
			{
				SendData(&Curr_NetConnectPara);
			}
			else//发ASCALL
			{
				SendData(&Curr_NetConnectPara);
			}
			Sleep(pWnd->m_cycletime);
		}
	}
	return 0;
}

void CLog::OnBnClickedBtnSenddata()
{
	if (!pWnd)
	{
		pWnd = CNavDataShowDlg::pNavDataShowDlg;
		/*HWND hWnd = ::FindWindow(NULL, _T("实时定位数据显示"));
		pWnd = (CNavDataShowDlg*)CNavDataShowDlg::FromHandle(hWnd);*/
	}
	if (Curr_NetConnectPara.ConnectState)//
	{
		pWnd->UpdateData(true);
		if (pWnd->m_checkperiod)//持续发送
		{
			b_conSendOrNot = true;//表示持续发送
			CString str_btnSend;
			m_sendMsg.GetWindowText(str_btnSend);
			if (str_btnSend == "发送")
			{
				m_sendMsg.SetWindowText("停止发送");
				m_richwrite.EnableWindow(false);
				pWnd->GetDlgItem(IDC_STATIC_GroupSend)->EnableWindow(false);
				pWnd->m_cycleControl.EnableWindow(false);
				AfxBeginThread(FuncThreadSend, this);
			}
			else
			{
				b_conSendOrNot = false;
				m_sendMsg.SetWindowText("发送");
				m_richwrite.EnableWindow(true);
				pWnd->GetDlgItem(IDC_STATIC_GroupSend)->EnableWindow(true);
				pWnd->m_cycleControl.EnableWindow(true);
			}
		}
		else//单个发送
		{
			b_conSendOrNot = false;
			CString str_send;
			m_richwrite.GetWindowText(str_send);
			if (b_SendAscall)
			{
				Str2HexRetVal sendVal = String2Hex(str_send);
				Curr_NetConnectPara.Senddata = sendVal.senfbuf;
				Curr_NetConnectPara.Len_Senddata = sendVal.len;
			}
			else
			{
				Curr_NetConnectPara.Senddata = new char[1000];
				Curr_NetConnectPara.Len_Senddata = str_send.GetLength();
				memcpy(Curr_NetConnectPara.Senddata, str_send, str_send.GetLength());
			}
			SendData(&Curr_NetConnectPara);
		}
	}
	else
	{
		AfxMessageBox("网络未连接");
	}

}


void CLog::onTextChanged()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (b_SendAscall)
	{
		CString str_input;
		m_richwrite.GetWindowText(str_input);
		if (str_input.GetLength() > 0)
		{
			int len_input = str_input.GetLength();
			if (str_input[len_input - 1] > 102 || (str_input[len_input - 1] > 70 && str_input[len_input - 1] <= 90))
			{
				str_input = str_input.Left(len_input - 1);
				m_richwrite.SetWindowText(str_input);
			}
			int len = str_input.GetLength();
			m_richwrite.SetSel(len, len, FALSE);
			m_richwrite.SetFocus();
		}
	}
}
