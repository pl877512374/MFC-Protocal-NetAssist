
// NavDataShowDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "NavDataShow.h"
#include "NavDataShowDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
const int rowCount = 101;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
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

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	// 实现
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


// CNavDataShowDlg 对话框



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


// CNavDataShowDlg 消息处理程序

BOOL CNavDataShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ip = 3232235522;
	m_port = 7070;
	UpdateData(FALSE);
	m_TabControl.InsertItem(0, _T("定位数据显示"));
	m_TabControl.InsertItem(1, _T("数据收发"));
	m_page1.Create(IDD_DIALOG_POINTSHOW, &m_TabControl);
	m_page2.Create(IDD_DIALOG_LOG, &m_TabControl);
	//设定在Tab内显示的范围
	CRect rc;
	m_TabControl.GetClientRect(rc);
	rc.top += 20;
	rc.bottom -= 0;
	rc.left += 0;
	rc.right -= 0;
	m_page1.MoveWindow(&rc);
	m_page2.MoveWindow(&rc);
	m_page1.ShowWindow(TRUE);
	// 将“关于...”菜单项添加到系统菜单中。
	((CButton *)GetDlgItem(IDC_RADIO_ASCALL))->SetCheck(TRUE); //选上
	((CButton *)GetDlgItem(IDC_RADIO_HEX))->SetCheck(FALSE);//不选上
	((CButton *)GetDlgItem(IDC_RADIO_SASCALL))->SetCheck(FALSE); //不选上
	((CButton *)GetDlgItem(IDC_RADIO_SHEX))->SetCheck(TRUE);//选上

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNavDataShowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
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
					UpdateData(true);//不加这一句会导致主界面控件无法操作
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
					sLog = sLog + "解析后为：" + MNChartTip +" "+g_SICKData[13];//g_SICKData[13]数据类型
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
					SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //写入日志
				}
				else
				{
					if (g_SICKData[3] != "6")
					{
						MNInvalidCount++;
					}
					UpdateData(FALSE);
					sLog = SICKTempData + "\r\n";
					sLog = sLog + "       解析为无效数据" + "\r\n";
					SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //写入日志
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
	// TODO:  在此添加控件通知处理程序代码
	if (str == "开始广播")
	{
		Brocast(this->m_hWnd, true, NetBroZhiLing);
		m_BrocastOper.SetWindowText("关闭广播");
	}
	else
	{
		m_BrocastOper.SetWindowText("开始广播");
		Brocast(this->m_hWnd, false, NetBroZhiLing);
	}
	// TODO:  在此添加控件通知处理程序代码

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
	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	Curr_NetConnectPara.IP = m_ip;
	Curr_NetConnectPara.Port = m_port;
	CString str;
	m_Connnect.GetWindowText(str);
	if (str == "连接")
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
			CString temp = "匀速测试";
			OnMNPrepareStartTest(temp);
			f_doWork = true;
			m_Connnect.SetWindowText("断开");
		}
		else
		{
			Curr_NetConnectPara.ConnectState = false;;
			MessageBox("网络连接失败", MB_OK);
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
			m_page2.m_sendMsg.SetWindowText("发送");
			m_cycleControl.EnableWindow(true);
			m_page2.m_richwrite.EnableWindow(true);
			f_doWork = false;
			m_Connnect.SetWindowText("连接");
		}
		else
		{
			m_Connnect.SetWindowText("断开");
		}
	}

}

void CNavDataShowDlg::OnMNPrepareStartTest(CString TestName)
{
	CString FileName = "";
	CString strTemp = "";
	FileName = CTime::GetCurrentTime().Format("测试日志_%Y-%m-%d-%H-%M-%S.txt_AGV");
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
	m_SavePath = path + _T("\\") + "AGV测试结果";
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
	// TODO:  在此添加控件通知处理程序代码
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
	int DataLocation = 0; //取数位置
	CString LoadFilePath = CFileUtils::OpenFileDlg(TRUE, _T("H Files (*.txt_AGV)|*.txt_AGV||"), NULL, NULL, NULL);
	CString l_sNameTmp;
	l_sNameTmp = LoadFilePath.Right(LoadFilePath.GetLength() - LoadFilePath.ReverseFind('.') - 1);
	if (l_sNameTmp != "txt_AGV")
	{
		MessageBox("请选择后缀为txt_AGV的文件！");
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
			IndexStart = str.Find("解析后为：");
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
				IndexStart = str.Find("无效数据");
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
		MessageBox("读取绘制完成，有效和无效数据已显示！");
	}
}


void CNavDataShowDlg::OnBnClickedBtnOk()
{
	// TODO:  在此添加控件通知处理程序代码

}


void CNavDataShowDlg::OnBnClickedBtnStoptest()
{
	// TODO:  在此添加控件通知处理程序代码
	f_doWork = false;
}


void CNavDataShowDlg::onSendAscallClicked()
{
	// TODO:  在此添加控件通知处理程序代码
	b_SendAscall = FALSE;
}


void CNavDataShowDlg::onSendHexClicked()
{
	// TODO:  在此添加控件通知处理程序代码
	b_SendAscall = TRUE;
}




