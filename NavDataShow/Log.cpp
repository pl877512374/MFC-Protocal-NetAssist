// Log.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NavDataShow.h"
#include "Log.h"
#include "afxdialogex.h"
#include "NavDataShowDlg.h"
// CLog �Ի���
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


// CLog ��Ϣ�������


void CLog::OnBnClickedBtnClearsend()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_richwrite.SetSel(0, -1);
	m_richwrite.Clear();
}


void CLog::OnBnClickedBtnClearrecv()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
			if (b_SendAscall)//��16����
			{
				SendData(&Curr_NetConnectPara);
			}
			else//��ASCALL
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
		/*HWND hWnd = ::FindWindow(NULL, _T("ʵʱ��λ������ʾ"));
		pWnd = (CNavDataShowDlg*)CNavDataShowDlg::FromHandle(hWnd);*/
	}
	if (Curr_NetConnectPara.ConnectState)//
	{
		pWnd->UpdateData(true);
		if (pWnd->m_checkperiod)//��������
		{
			b_conSendOrNot = true;//��ʾ��������
			CString str_btnSend;
			m_sendMsg.GetWindowText(str_btnSend);
			if (str_btnSend == "����")
			{
				m_sendMsg.SetWindowText("ֹͣ����");
				m_richwrite.EnableWindow(false);
				pWnd->GetDlgItem(IDC_STATIC_GroupSend)->EnableWindow(false);
				pWnd->m_cycleControl.EnableWindow(false);
				AfxBeginThread(FuncThreadSend, this);
			}
			else
			{
				b_conSendOrNot = false;
				m_sendMsg.SetWindowText("����");
				m_richwrite.EnableWindow(true);
				pWnd->GetDlgItem(IDC_STATIC_GroupSend)->EnableWindow(true);
				pWnd->m_cycleControl.EnableWindow(true);
			}
		}
		else//��������
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
		AfxMessageBox("����δ����");
	}

}


void CLog::onTextChanged()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
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
