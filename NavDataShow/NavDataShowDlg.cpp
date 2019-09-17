
// NavDataShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "NavDataShow.h"
#include "NavDataShowDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
const int rowCount = 101;
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
char netBroZL[] = { 0xFF, 0xAA, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01,
0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x1B, 0xEE, 0xEE };
CPointShow m_page1;
CLog m_page2;
CString MNChartTip = "";
int MNLastTimestamp = 0;
CCDMLogInfo *cLogInfo;
CString m_SavePath;
bool b_SendAscall = TRUE;
NetConnectPara Curr_NetConnectPara;
CNavDataShowDlg* CNavDataShowDlg::pNavDataShowDlg = NULL;
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CNavDataShowDlg �Ի���



CNavDataShowDlg::CNavDataShowDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CNavDataShowDlg::IDD, pParent)
, m_ip(0)
, m_port(0)
, MNPoseGetCount(0)
, MNIniPoseNum(0)
, MNContinuePoseNum(0)
, MNVirtualPoseNum(0)
, MNNGPoseNum(0)
, MNPoseX(0)
, MNPoseY(0)
, MNPoseAngle(0)
, MNPoseTimestamp(_T(""))
, MNInvalidCount(0)
, m_recAcall(FALSE)
, m_sendAscall(TRUE)
, m_checkperiod(FALSE)
, m_cycletime(120)
{
	pNavDataShowDlg = this;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CNavDataShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
	DDX_IPAddress(pDX, IDC_IPADDRESS1, m_ip);
	DDX_Control(pDX, IDC_BTN_BROADCAST, m_BrocastOper);
	DDX_Text(pDX, IDC_EDIT_PORT, m_port);
	DDX_Control(pDX, IDC_BTN_CONNECT, m_Connnect);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT, MNPoseGetCount);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT2, MNIniPoseNum);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT3, MNContinuePoseNum);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT4, MNVirtualPoseNum);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT5, MNNGPoseNum);
	DDX_Text(pDX, IDC_EDIT_MNPOSEX, MNPoseX);
	DDX_Text(pDX, IDC_EDIT_MNPOSEY, MNPoseY);
	DDX_Text(pDX, IDC_EDIT_MNPOSEANGLE, MNPoseAngle);
	DDX_Text(pDX, IDC_EDIT_MNPOSETIMESTAMP, MNPoseTimestamp);
	DDX_Text(pDX, IDC_EDIT_MNINVALIDCOUNT, MNInvalidCount);
	DDX_Radio(pDX, IDC_RADIO_ASCALL, m_recAcall);
	DDX_Radio(pDX, IDC_RADIO_SASCALL, m_sendAscall);
	DDX_Check(pDX, IDC_CHECK_XUNHUAN, m_checkperiod);
	DDX_Text(pDX, IDC_EDIT1, m_cycletime);
	DDX_Control(pDX, IDC_CHECK_XUNHUAN, m_cycleControl);
}

BEGIN_MESSAGE_MAP(CNavDataShowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_BROADCAST, &CNavDataShowDlg::OnBnClickedBtnBroadcast)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CNavDataShowDlg::OnTcnSelchangeTab)
	ON_MESSAGE(WM_BROCAST_PARA, &CNavDataShowDlg::OnBraocastPara)
	ON_MESSAGE(WM_SICK_POSEREAD, &CNavDataShowDlg::OnPoseDataDeal)
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CNavDataShowDlg::OnBnClickedBtnConnect)
	ON_MESSAGE(WM_MNWRITELOG, &CNavDataShowDlg::OnMnwritelog)
	ON_BN_CLICKED(IDC_BTN_LOADDATA, &CNavDataShowDlg::OnBnClickedBtnLoaddata)
	ON_BN_CLICKED(ID_BTN_OK, &CNavDataShowDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_STOPTEST, &CNavDataShowDlg::OnBnClickedBtnStoptest)
	ON_BN_CLICKED(IDC_RADIO_SASCALL, &CNavDataShowDlg::onSendAscallClicked)
	ON_BN_CLICKED(IDC_RADIO_SHEX, &CNavDataShowDlg::onSendHexClicked)
END_MESSAGE_MAP()


// CNavDataShowDlg ��Ϣ�������

BOOL CNavDataShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ip = 3232235522;
	m_port = 7070;
	UpdateData(FALSE);
	m_TabControl.InsertItem(0, _T("��λ������ʾ"));
	m_TabControl.InsertItem(1, _T("�����շ�"));
	m_page1.Create(IDD_DIALOG_POINTSHOW, &m_TabControl);
	m_page2.Create(IDD_DIALOG_LOG, &m_TabControl);
	//�趨��Tab����ʾ�ķ�Χ
	CRect rc;
	m_TabControl.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_page1.MoveWindow(&rc);
	m_page2.MoveWindow(&rc);
	m_page1.ShowWindow(TRUE);
	// ��������...���˵�����ӵ�ϵͳ�˵��С�
	((CButton *)GetDlgItem(IDC_RADIO_ASCALL))->SetCheck(TRUE); //ѡ��
	((CButton *)GetDlgItem(IDC_RADIO_HEX))->SetCheck(FALSE);//��ѡ��
	((CButton *)GetDlgItem(IDC_RADIO_SASCALL))->SetCheck(FALSE); //��ѡ��
	((CButton *)GetDlgItem(IDC_RADIO_SHEX))->SetCheck(TRUE);//ѡ��

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CNavDataShowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CNavDataShowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CNavDataShowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
bool f_doWork = false;
afx_msg LRESULT CNavDataShowDlg::OnPoseDataDeal(WPARAM wParam, LPARAM lParam)
{
	if (f_doWork)
	{
		char *pcBuf = (char *)wParam;
		int nBufSize = (int)lParam;
		CString str_ouput;
		if (pcBuf[0] == 2 && pcBuf[nBufSize - 1] == 3)
		{
			CString SICKTempData = "";
			CString g_SICKData[50];
			int pos = 0;
			int pre_pos = 0;
			int index = 0;
			CString sLog = "";
			int CurrentTimestamp = 0;

			SICKTempData = pcBuf;
			SICKTempData = SICKTempData.Left(nBufSize);
			while (-1 != pos)
			{
				pre_pos = pos;
				pos = SICKTempData.Find(" ", (pos + 1));
				if (-1 == pos)
				{
					g_SICKData[index] = (SICKTempData.Mid(pre_pos + 1, nBufSize - pre_pos - 2));
				}
				else
				{
					g_SICKData[index++] = (SICKTempData.Mid(pre_pos + 1, (pos - pre_pos) - 1));
				}
			}

			if (g_SICKData[0] == "sAN" && g_SICKData[1] == "mNPOSGetPose")
			{
				if (g_SICKData[5] == "1")//pose data follow
				{
					UpdateData(true);//������һ��ᵼ��������ؼ��޷�����
					MNPoseGetCount++;
					switch (_ttoi(g_SICKData[13]))
					{
					case 0:
						MNIniPoseNum++;
						break;
					case 1:
						MNContinuePoseNum++;
						break;
					case 2:
						MNVirtualPoseNum++;
						break;
					default:
						MNNGPoseNum++;
						break;
					}
					MNPoseX = strtoul(g_SICKData[6], NULL, 16);
					MNPoseY = strtoul(g_SICKData[7], NULL, 16);
					MNPoseAngle = strtol(g_SICKData[8], NULL, 16);
					MNPoseTimestamp = g_SICKData[11];
					CurrentTimestamp = strtol(g_SICKData[11], NULL, 16);
					UpdateData(FALSE);
					MNChartTip.Format("%d, %d, %d,%d, %d ", MNPoseGetCount, MNPoseX, MNPoseY, MNPoseAngle, (CurrentTimestamp - MNLastTimestamp));
					MNLastTimestamp = CurrentTimestamp;
					MNChartTip += MNPoseTimestamp;
					((CSeries)m_page1.m_MNChartrsTrack.Series(0)).AddXY(MNPoseX, MNPoseY, MNChartTip, 0);
					sLog = SICKTempData + "\r\n";
					sLog = sLog + "������Ϊ��" + MNChartTip +" "+g_SICKData[13];//g_SICKData[13]��������
					m_page2.m_richedit.SetSel(-1, -1);
					if (!m_recAcall)
					{
						CString   strLog_Time(" ");
						SYSTEMTIME   tSysTime;
						GetLocalTime(&tSysTime);
						strLog_Time.Format("%02ld:%02ld:%02ld:%03ld",
							tSysTime.wHour, tSysTime.wMinute, tSysTime.wSecond,
							tSysTime.wMilliseconds);
						m_page2.m_richedit.ReplaceSel(strLog_Time + ":\r\n");
						for (size_t i = 0; i < SICKTempData.GetLength(); i += rowCount)
						{
							m_page2.m_richedit.ReplaceSel(SICKTempData.Mid(i, rowCount) + "\r\n");
						}
					}
					else
					{
						CString str_temp;
						for (size_t i = 0; i < nBufSize; i++)
						{
							str_temp.Format("%02X", pcBuf[i]);
							str_temp += " ";
							str_ouput += str_temp;
						}
						CString   strLog_Time(" ");
						SYSTEMTIME   tSysTime;
						GetLocalTime(&tSysTime);
						strLog_Time.Format("%02ld:%02ld:%02ld:%03ld",
							tSysTime.wHour, tSysTime.wMinute, tSysTime.wSecond,
							tSysTime.wMilliseconds);
						m_page2.m_richedit.ReplaceSel(strLog_Time + ":\r\n");
						for (size_t i = 0; i < str_ouput.GetLength(); i += rowCount)
						{
							m_page2.m_richedit.ReplaceSel(str_ouput.Mid(i, rowCount) + "\r\n");
						}
					}
					SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //д����־
				}
				else
				{
					if (g_SICKData[3] != "6")
					{
						MNInvalidCount++;
					}
					UpdateData(FALSE);
					sLog = SICKTempData + "\r\n";
					sLog = sLog + "       ����Ϊ��Ч����" + "\r\n";
					SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //д����־
				}
			}
		}
		else
		{
			if (!m_recAcall)
			{
				CString   strLog_Time(" ");
				SYSTEMTIME   tSysTime;
				GetLocalTime(&tSysTime);
				strLog_Time.Format("%02ld:%02ld:%02ld:%03ld",
					tSysTime.wHour, tSysTime.wMinute, tSysTime.wSecond,
					tSysTime.wMilliseconds);
				m_page2.m_richedit.ReplaceSel(strLog_Time + ":\r\n");
				CString TempData = pcBuf;
				m_page2.m_richedit.ReplaceSel(TempData + "\r\n");
			}
			else
			{
				CString str_temp;
				for (size_t i = 0; i < nBufSize; i++)
				{
					str_temp.Format("%02X", pcBuf[i]);
					str_temp += " ";
					str_ouput += str_temp;
				}
				CString   strLog_Time(" ");
				SYSTEMTIME   tSysTime;
				GetLocalTime(&tSysTime);
				strLog_Time.Format("%02ld:%02ld:%02ld:%03ld",
					tSysTime.wHour, tSysTime.wMinute, tSysTime.wSecond,
					tSysTime.wMilliseconds);
				m_page2.m_richedit.ReplaceSel(strLog_Time + ":\r\n");
				m_page2.m_richedit.ReplaceSel(str_ouput + "\r\n");

			}
		}
		m_page2.m_richedit.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	return 0;
}


void CNavDataShowDlg::OnBnClickedBtnBroadcast()
{
	CString str;
	m_BrocastOper.GetWindowText(str);
	StruBroZhiL NetBroZhiLing;
	NetBroZhiLing.broZL = netBroZL;
	NetBroZhiLing.lenZL = 34;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (str == "��ʼ�㲥")
	{
		Brocast(this->m_hWnd, true, NetBroZhiLing);
		m_BrocastOper.SetWindowText("�رչ㲥");
	}
	else
	{
		m_BrocastOper.SetWindowText("��ʼ�㲥");
		Brocast(this->m_hWnd, false, NetBroZhiLing);
	}
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}

afx_msg LRESULT CNavDataShowDlg::OnBraocastPara(WPARAM wparam, LPARAM lparam)
{
	BroPara *retVal = (BroPara*)wparam;
	DWORD dwIP = ntohl(inet_addr(retVal->ip));
	m_ip = dwIP;
	m_port =_ttoi(retVal->port);
	UpdateData(FALSE);
	return 0;
}

void CNavDataShowDlg::OnTcnSelchangeTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	int CurSel = m_TabControl.GetCurSel();
	switch (CurSel)
	{
	case 0:
	{
			  m_page1.ShowWindow(TRUE);
			  m_page2.ShowWindow(FALSE);
			  break;
	}
	case 1:
	{
			  m_page1.ShowWindow(FALSE);
			  m_page2.ShowWindow(TRUE);
			  break;
	}
	}
	*pResult = 0;
}


void CNavDataShowDlg::OnBnClickedBtnConnect()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	Curr_NetConnectPara.IP = m_ip;
	Curr_NetConnectPara.Port = m_port;
	CString str;
	m_Connnect.GetWindowText(str);
	if (str == "����")
	{
		bool b_Connect = ConnectServer(this->m_hWnd, &Curr_NetConnectPara);
		if (b_Connect)
		{
			Curr_NetConnectPara.ConnectState = true;
			MNPoseGetCount = 0;
			MNInvalidCount = 0;
			MNContinuePoseNum = 0;
			MNVirtualPoseNum = 0;
			MNNGPoseNum = 0;
			MNPoseAngle = 0;
			MNPoseX = 0;
			MNPoseY = 0;
			MNPoseTimestamp = "";
			((CSeries)(m_page1.m_MNChartrsTrack.Series(0))).Clear();
			CString temp = "���ٲ���";
			OnMNPrepareStartTest(temp);
			f_doWork = true;
			m_Connnect.SetWindowText("�Ͽ�");
		}
		else
		{
			Curr_NetConnectPara.ConnectState = false;;
			MessageBox("��������ʧ��", MB_OK);
		}
		UpdateData(FALSE);
	}
	else
	{
		bool b_DisConnect = DisconnectServer(&Curr_NetConnectPara);
		if (b_DisConnect)
		{
			b_conSendOrNot = false;
			if (cLogInfo)
			{
				cLogInfo->Release();
				cLogInfo = NULL;
			}
			Curr_NetConnectPara.ConnectState = false;
			m_page2.m_sendMsg.SetWindowText("����");
			m_cycleControl.EnableWindow(true);
			m_page2.m_richwrite.EnableWindow(true);
			f_doWork = false;
			m_Connnect.SetWindowText("����");
		}
		else
		{
			m_Connnect.SetWindowText("�Ͽ�");
		}
	}

}

void CNavDataShowDlg::OnMNPrepareStartTest(CString TestName)
{
	CString FileName = "";
	CString strTemp = "";
	FileName = CTime::GetCurrentTime().Format("������־_%Y-%m-%d-%H-%M-%S.txt_AGV");
	OnMNCreateFile(FileName);
	cLogInfo = CCDMLogInfo::GetInstance(m_SavePath.GetBuffer(m_SavePath.GetLength()));
	strTemp = "\r\n========" + CTime::GetCurrentTime().Format("%H:%M:%S ") + TestName + "========\r\n";
	if (cLogInfo)
		cLogInfo->SetNotify(strTemp.GetBuffer(strTemp.GetLength()));
}

void CNavDataShowDlg::OnMNCreateFile(CString filename)
{
	CFile file;
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_SavePath = path + _T("\\") + "AGV���Խ��";
	if (!PathIsDirectory(m_SavePath))
	{
		BOOL bRet = CreateDirectory(m_SavePath, NULL);
	}

	m_SavePath = m_SavePath + "\\" + filename;

	file.Open(m_SavePath, CFile::modeCreate | CFile::modeWrite);
	file.Close();
}

afx_msg LRESULT CNavDataShowDlg::OnMnwritelog(WPARAM wParam, LPARAM lParam)
{
	CString   str = (char*)lParam;
	CString getData = "";
	CTime t = CTime::GetCurrentTime();
	getData = t.Format("%H:%M:%S ") + str;
	if (cLogInfo)
		cLogInfo->SetNotify(getData.GetBuffer(getData.GetLength()));
	getData.ReleaseBuffer();
	return 0;
}


void CNavDataShowDlg::OnBnClickedBtnLoaddata()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CStdioFile hufile;
	CString str = "";
	int IndexStart = 0;
	int len = 0;
	CString strTip = "";
	CString temp = "";
	int IndexDotStart = 0;
	int IndexDotEnd = 0;
	int DataX = 0;
	int DataY = 0;
	int DataLocation = 0; //ȡ��λ��
	CString LoadFilePath = CFileUtils::OpenFileDlg(TRUE, _T("H Files (*.txt_AGV)|*.txt_AGV||"), NULL, NULL, NULL);
	CString l_sNameTmp;
	l_sNameTmp = LoadFilePath.Right(LoadFilePath.GetLength() - LoadFilePath.ReverseFind('.') - 1);
	if (l_sNameTmp != "txt_AGV")
	{
		MessageBox("��ѡ���׺Ϊtxt_AGV���ļ���");
		return;
	}

	MNPoseGetCount = 0;
	MNInvalidCount = 0;
	MNIniPoseNum = 0;
	MNContinuePoseNum = 0;
	MNVirtualPoseNum = 0;
	MNNGPoseNum = 0;;
	((CSeries)(m_page1.m_MNChartrsTrack.Series(0))).Clear();
	if (hufile.Open(LoadFilePath, CFile::modeRead))
	{
		while (hufile.ReadString(str))
		{
			IndexStart = str.Find("������Ϊ��");
			len = str.GetLength();
			if (IndexStart != -1)
			{
				strTip = str.Mid(IndexStart + 10, len - 5);
				while (IndexDotEnd != -1)
				{
					IndexDotStart = IndexDotEnd;
					IndexDotEnd = strTip.Find(", ", IndexDotStart + 1);
					DataLocation++;
					if (DataLocation == 2)
					{
						temp = strTip.Mid(IndexDotStart + 2, IndexDotEnd - IndexDotStart - 2);
						DataX = atoi(temp);
					}
					else if (DataLocation == 3)
					{
						temp = strTip.Mid(IndexDotStart + 2, IndexDotEnd - IndexDotStart - 2);
						DataY = atoi(temp);
						MNPoseGetCount++;
						((CSeries)(m_page1.m_MNChartrsTrack.Series(0))).AddXY(DataX, DataY, strTip, 0);
					}
					else if (DataLocation > 3)
					{
						CString str_temp = strTip.Mid(strTip.GetLength() - 1, 1);
						switch (_ttoi(str_temp))
						{
						case 0:
						MNIniPoseNum++;
						break;
						case 1:
						MNContinuePoseNum++;
						break;
						case 2:
						MNVirtualPoseNum++;
						break;
						default:
						MNNGPoseNum++;
						break;
						}
						DataLocation = 0;
						IndexDotStart = 0;
						IndexDotEnd = 0;
						break;
					}
				}
			}
			else
			{
				IndexStart = str.Find("��Ч����");
				if (IndexStart != -1)
				{
					MNInvalidCount++;
				}
			}
		}
		hufile.Close();
		MNPoseX = 0;
		MNPoseY = 0;
		MNPoseAngle = 0;
		MNPoseTimestamp = "";
		UpdateData(false);
		MessageBox("��ȡ������ɣ���Ч����Ч��������ʾ��");
	}
}


void CNavDataShowDlg::OnBnClickedBtnOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

}


void CNavDataShowDlg::OnBnClickedBtnStoptest()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	f_doWork = false;
}


void CNavDataShowDlg::onSendAscallClicked()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	b_SendAscall = FALSE;
}


void CNavDataShowDlg::onSendHexClicked()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	b_SendAscall = TRUE;
}




