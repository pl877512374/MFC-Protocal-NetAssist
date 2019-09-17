// ProtocolAnalysis.cpp : ���� DLL Ӧ�ó���ĵ���������
//DLL_API

// ProtocolAnalysis.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "ProtocolAnalysis.h"
#include "math.h"
#include <winsock2.h> 
#include <string>
#pragma comment(lib,"ws2_32.lib")
using namespace std;
#define PI  3.14159265358979323846
const int SERVER_PORT = 6060;
const int CLIENT_PORT = 7000;
const int MAXLEN = 100;
int xorflag = 0;//У����
const int MAX_BLOCK = 10000;
const int NET_BUF_NUM = 100;
const int NET_BUFCOL_NUM = 5000;
char g_recvother[10000];
char g_cNetRecvBuf[NET_BUF_NUM][NET_BUFCOL_NUM];
typedef struct TagNetDataStruct
{
	char m_pcData[20000];
	int  m_n32Len;
	int m_n32BufCnt;
	int m_n32Channel;
}NetDataStruct;
NetDataStruct g_sNetData;
StruReturnVal ReturnVal;
char ArrayAngle[7200] = { 0 };//�ǶȲ�������
char ArrayEnergy[7200] = { 0 };//����ֵ����
bool f_AddAng = false;//�Ƿ���Ҫ�ǶȲ�����־λ
bool f_AddEnergy = false;
int g_n32MaxDis = 50000;//�����ֵ
int g_n32NetRcvdPkgs = 0;
int g_n32CircleNo = 0;//Ȧ�ű������仯��Ȧ��ֵ��
bool b_readfinish = false;//һ֡������û�ж������һ���ı�־
int g_n32NetRecvID = 0;//ÿһ֡���ݵ�ID
SOCKET sockClient;//TCP
HWND m_hReciWnd;
SOCKET brocastSocket;//udp�㲥
SOCKADDR_IN sin_server;
bool f_BroThreadCon = false;
char *sendbroBuf = NULL;//�㲥ָ��
int nbroLen = 0;//�㲥ָ���
HANDLE handleSecond = NULL;
typedef struct TagStructNetBuf
{
	char m_acbuf[MAX_BLOCK * 10];
	unsigned int m_u32in;
	unsigned int m_u32out;
	unsigned int m_u32size;
}StructNetBuf;
StructNetBuf g_sNetBuf;
CRITICAL_SECTION g_netcs;
char acRecvDataBuf[MAX_BLOCK];

DWORD WINAPI RecvThreadProc(LPVOID lpParam)	// �����̴߳�����
{
	InitializeCriticalSection(&g_netcs);
	int l_n32TcpErr = 0;//0������1ճ֡��2��֡
	memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
	int nRecvSize = 1;
	while (nRecvSize > 0)
	{
		EnterCriticalSection(&g_netcs);
		nRecvSize = recv(sockClient, acRecvDataBuf, MAX_BLOCK, 0);
		if (nRecvSize <= 0)
		{
			continue;
		}
		if ((DWORD)nRecvSize > MAX_BLOCK)
		{
			printf("nRecvSize > MAX_BLOCK");
			continue;
		}
		//-----------------------�½���------------------------------------------
		if ((g_sNetBuf.m_u32in + nRecvSize) >= MAX_BLOCK * 10)
		{
			printf("(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));//������󻺴棬ԭ������ȫ��0
			continue;
		}
		memcpy(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in], acRecvDataBuf, nRecvSize*sizeof(char));
		g_sNetBuf.m_u32in += nRecvSize;
		if (g_sNetBuf.m_u32in >= MAX_BLOCK * 10)
		{
			printf("g_sNetBuf.m_u32in >= MAX_BLOCK\n");
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
			break;
		}
		while (g_sNetBuf.m_u32out < g_sNetBuf.m_u32in)
		{
			if (((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff &&
				(unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 1] == 0xaa))
			{
				//���������
				unsigned int l_u32reallen = 0;
				if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff)
				{
					l_u32reallen = ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 3] << 0) |
						((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 2] << 8);
					l_u32reallen = l_u32reallen + 4;
				}
				else
				{
					g_sNetBuf.m_u32out++;
					continue;
				}
				//�жϰ�������ʵ�ʻ��泤��֮��Ĺ�ϵ
				if (l_u32reallen <= (g_sNetBuf.m_u32in - g_sNetBuf.m_u32out + 1))
				{
					OnNetRecv(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out], l_u32reallen);
					if (xorflag == 1)   //У��ʧ��
					{
						printf("У�� failed!\n");
						int i;
						for (i = 1; i < l_u32reallen; i++)
						{
							if ((g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 1] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 2] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 3] == 0x02)
								|| (g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i] == 0xff && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + i + 1] == 0xff))
							{
								g_sNetBuf.m_u32out += i;
								//memset(&g_sNetBuf,0,sizeof(gti_sNetBuf));
								printf("��������֡ failed!\n");
								xorflag = 0;
								break;
							}
						}
						if (i == l_u32reallen && xorflag == 1)
						{
							g_sNetBuf.m_u32out += l_u32reallen;
						}
						xorflag = 0;
					}
					else
					{
						g_sNetBuf.m_u32out += l_u32reallen;
					}
				}
				else if (l_u32reallen >= MAX_BLOCK)
				{
					printf("l_u32reallen err %d\n", l_u32reallen);
					memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));//������󻺴棬ԭ������ȫ��0
					break;
				}
				else
				{
					break;
				}
			}
			else if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0x02 && (unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out + 1] != 0x02)
			{
				//���������
				unsigned int l_u32reallen = 0;
				if (g_sNetBuf.m_u32in - g_sNetBuf.m_u32out <= nRecvSize)
				{
					for (int i = g_sNetBuf.m_u32out; i < nRecvSize; i++)
					{
						if ((unsigned char)g_sNetBuf.m_acbuf[i] == 0x03)
						{
							l_u32reallen = i - g_sNetBuf.m_u32out + 1;
							OnNetRecv(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out], l_u32reallen);
							g_sNetBuf.m_u32out += l_u32reallen;
							break;
						}
					}
				}
				else
				{
					for (int i = 0; i < nRecvSize; i++)
					{
						if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in - nRecvSize + i] == 0x03)
						{
							l_u32reallen = g_sNetBuf.m_u32in - nRecvSize + i + 1;
							OnNetRecv(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out], l_u32reallen);
							g_sNetBuf.m_u32out += l_u32reallen;
							break;
						}
					}
				}

				//�жϰ����ȴ�������������Ȼû�ҵ�03��������
				if ((g_sNetBuf.m_u32in - g_sNetBuf.m_u32out) >= 1460 && (l_u32reallen == 0))
				{
					printf("l_u32reallen err %d\n", l_u32reallen);
					memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));//������󻺴棬ԭ������ȫ��0
					break;
				}
				else
				{
					continue;
				}
			}
			else if (nRecvSize >= 1)
			{
				if (g_sNetBuf.m_u32in - g_sNetBuf.m_u32out <= nRecvSize)
				{
					OnNetRecv(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out], nRecvSize);
					g_sNetBuf.m_u32out += nRecvSize;
					break;
				}
			}
			else
			{
				g_sNetBuf.m_u32out++;
			}

		}
		if ((g_sNetBuf.m_u32in + nRecvSize) >= MAX_BLOCK * 10)
		{
			printf("������(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			printf("g_sNetBuf.m_u32in is %d,g_sNetBuf.m_u32out is%d\n", g_sNetBuf.m_u32in, g_sNetBuf.m_u32out);
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));//������󻺴棬ԭ������ȫ��0
			continue;
		}
		if (g_sNetBuf.m_u32out >= g_sNetBuf.m_u32in)
		{
			memset(&g_sNetBuf, 0, sizeof(g_sNetBuf));
		}
		LeaveCriticalSection(&g_netcs);
	}
	return 0;
}

bool ConnectServer(HWND hReciveWnd, NetConnectPara*NetStruct)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	int len = sizeof(int);
	struct timeval timeout = { 3, 0 };//3s
	fd_set set;
	unsigned long ul = 1;
	bool ret = false;
	wVersionRequested = MAKEWORD(1, 1);//�������ò�ͬ��Winsock�汾������MAKEWORD(2,2)���ǵ���2.2�棬MAKEWORD(1,1)���ǵ���1.1�档 
	err = WSAStartup(wVersionRequested, &wsaData);//ʹ��Socket�ĳ�����ʹ��Socket֮ǰ�������WSAStartup�������Ժ�Ӧ�ó���Ϳ��Ե����������Socket���е�����Socket������
	if (err != 0)
		return FALSE;

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion != 1))
	{
		WSACleanup();
		return FALSE;
	}
	// �����ͻ����׽���
	sockClient = socket(AF_INET, SOCK_STREAM, 0);//���÷�����ģʽ
	if (sockClient == INVALID_SOCKET)
	{
		return FALSE;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(NetStruct->IP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(NetStruct->Port);
	ioctlsocket(sockClient, FIONBIO, &ul);
	int iConnect = connect(sockClient, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
	//�����ӳɹ�
	if (iConnect == -1)
	{
		FD_ZERO(&set);//��ָ�����ļ�����������գ��ڶ��ļ����������Ͻ�������ǰ�����������г�ʼ�����������գ�������ϵͳ�����ڴ�ռ��
		//ͨ����������մ������Խ���ǲ���֪�ġ�
		FD_SET(sockClient, &set);//�������ļ�����������������һ���µ��ļ��������� 
		if (select(sockClient + 1, NULL, &set, NULL, &timeout) > 0)
		{
			getsockopt(sockClient, SOL_SOCKET, SOCKET_ERROR, (char*)err, &len);
			if (err == 0)
			{
				ret = true;
			}
			else
			{
				ret = false;
			}
		}
		else
			ret = false;

	}
	else
		ret = false;
	ul = 0;
	ioctlsocket(sockClient, FIONBIO, &ul);
	if (!ret)
	{
		return FALSE;
	}
	NetStruct->ConnectState = true;
	m_hReciWnd = hReciveWnd;
	DWORD dwThreadID = 0;
	HANDLE handleFirst = CreateThread(NULL, 0, RecvThreadProc, 0, 0, &dwThreadID);
	return TRUE;
}

bool DisconnectServer(NetConnectPara*NetStruct)
{
	if (NetStruct->ConnectState = true)
	{
		Sleep(100);
		closesocket(sockClient);
		WSACleanup();
		NetStruct->ConnectState = false;
		return true;
	}
	else
	{
		return false;
	}
}

bool checkXor(char* recvbuf, int recvlen)
{
	int i = 0;
	char check = 0;
	char * p = recvbuf;
	int len;//ĩβ��λ����У��
	if (*p == (char)0xff)
	{
		len = recvlen - 2;
		p++;
		p++;
		for (i = 0; i < len; i++)
		{
			check ^= *p;
			p++;
		}
		p++;
	}
	else if (*p == (char)0x02)
	{
		p = p + 8;
		len = recvlen - 9;
		for (i = 0; i < len; i++)
		{
			check ^= *p++;
		}
	}
	else
	{
		return false;
	}

	if (check == *p)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void OnNetRecv(char *pDataBuf, int nDataBufSize)
{
	memcpy(g_cNetRecvBuf[g_n32NetRecvID], pDataBuf, nDataBufSize);
	if (g_cNetRecvBuf[g_n32NetRecvID][0] == (char)0xFF &&
		g_cNetRecvBuf[g_n32NetRecvID][1] == (char)0xAA && g_cNetRecvBuf[g_n32NetRecvID][12] == (char)0x00 &&
		g_cNetRecvBuf[g_n32NetRecvID][13] == (char)0x07 && g_cNetRecvBuf[g_n32NetRecvID][11] == 0x02)   //����������
	{
		static int temp_Packnum = 0;
		static int PackSum = 0;
		if (nDataBufSize > 0)
		{
			if (checkXor(pDataBuf, nDataBufSize))
			{
				memcpy(g_cNetRecvBuf[g_n32NetRecvID], pDataBuf, nDataBufSize);
				switch ((unsigned char)g_cNetRecvBuf[g_n32NetRecvID][22])
				{
#pragma region 0x02
				case 0x02:
				{
							 g_n32NetRcvdPkgs++;
							 int CirclNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][81];   //Ȧ��
							 if (g_n32NetRcvdPkgs == 1)//����ǵ�һ������
							 {
								 g_n32CircleNo = CirclNo;               //��¼��һ�����ݵ�Ȧ��
							 }
							 int PackNo = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][82];  //��Ȧ����
							 int MarkScanData = (BYTE)g_cNetRecvBuf[g_n32NetRecvID][79];//ɨ����������
							 int RatioMark = (unsigned char)g_cNetRecvBuf[g_n32NetRecvID][80];//�ֱ��ʱ�־
							 int RightPackNum;//һ֡��ȷ�İ�����
							 switch (MarkScanData)
							 {
							 case 0://����
								 if (RatioMark == 0)
								 {
									 RightPackNum = 78;
								 }
								 else if (RatioMark == 1)
								 {
									 RightPackNum = 21;
								 }
								 f_AddAng = false;
								 f_AddEnergy = false;
								 break;
							 case 3://����+�Ƕ�
								 if (RatioMark == 0)
								 {
									 RightPackNum = 171;
								 }
								 else if (RatioMark == 1)
								 {
									 RightPackNum = 45;
								 }
								 f_AddAng = true;
								 f_AddEnergy = false;
								 break;
							 case 4://����+����
								 if (RatioMark == 0)
								 {
									 RightPackNum = 171;
								 }
								 else if (RatioMark == 1)
								 {
									 RightPackNum = 45;
								 }
								 f_AddAng = false;
								 f_AddEnergy = true;
								 break;
							 case 6://����+�Ƕ�+����
								 if (RatioMark == 0)
								 {
									 RightPackNum = 300;
								 }
								 else if (RatioMark == 1)
								 {
									 RightPackNum = 78;
								 }
								 f_AddAng = true;
								 f_AddEnergy = true;
								 break;
							 }
							 if (RatioMark == 0) //12��
							 {
								 switch (PackNo)
								 {
								 case 1:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 2:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 3:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 4:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 5:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 6:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 7:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[7200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 8:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[8400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 9:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[9600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 10:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[10800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 11:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[12000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 12:
									 PackSum += PackNo;
									 if (MarkScanData == 0)//ɨ����������Ϊ����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 memcpy(&g_sNetData.m_pcData[13200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 13:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)//����+�Ƕ� ���߾���+�Ƕ�+����
									 {
										 memcpy(&ArrayAngle[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)//����+����
									 {
										 memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 14:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 15:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 16:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 17:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 18:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 4)//ɨ����������Ϊ����+�ǶȻ��߾���+����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 19:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 20:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 21:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 22:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 23:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 24:
									 PackSum += PackNo;
									 if (MarkScanData == 6)//ɨ����������Ϊ����+�Ƕ�+����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 memcpy(&ArrayEnergy[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 default:
									 break;
								 }
							 }
#pragma region �Ƕȷֱ���0.1
							 else if (RatioMark == 1)
							 {
								 switch (PackNo)
								 {
								 case 1:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 2:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 3:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 4:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 5:
									 PackSum += PackNo;
									 memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 6:
									 PackSum += PackNo;
									 if (MarkScanData == 0)//ɨ����������Ϊ����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 7:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)//����+�Ƕ� ���߾���+�Ƕ�+����
									 {
										 memcpy(&ArrayAngle[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)//����+����
									 {
										 memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 8:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 9:
									 PackSum += PackNo;
									 if (MarkScanData == 3 || MarkScanData == 4)//ɨ����������Ϊ����+�ǶȻ��߾���+����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 if (MarkScanData == 3 || MarkScanData == 6)
									 {
										 memcpy(&ArrayAngle[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 else if (MarkScanData == 4)
									 {
										 memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 }
									 break;
								 case 10:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[0], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 11:
									 PackSum += PackNo;
									 memcpy(&ArrayEnergy[1200], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 case 12:
									 PackSum += PackNo;
									 if (MarkScanData == 6)//ɨ����������Ϊ����+�Ƕ�+����
									 {
										 temp_Packnum = PackSum;
										 b_readfinish = true;
									 }
									 memcpy(&ArrayEnergy[2400], &g_cNetRecvBuf[g_n32NetRecvID][86], 1200);
									 break;
								 default:
									 break;
								 }
							 }
#pragma endregion
							 if (g_n32CircleNo != CirclNo)
							 {
								 PackSum = 0;
								 int BufLength = 0;
								 double AngRatio = 0;
								 if (RatioMark == 0)
								 {
									 BufLength = 14400;
									 AngRatio = 0.05;
								 }
								 else if (RatioMark == 1)
								 {
									 BufLength = 7200;
									 AngRatio = 0.1;
								 }
								 if (b_readfinish&&temp_Packnum == RightPackNum)
								 {
									 ReturnVal.Ratio = RatioMark;
									 b_readfinish = false;
									 int nDataZhiCount = 0;
									 int nDataJi = 0;
									 float l_64nDataRegionZhiX = 0;
									 float l_64nDataRegionZhiY = 0;
									 float l_n64actualangle = 0;
									 if (f_AddAng)
									 {
										 for (int i = 0; i < BufLength / 2; i++)//�����ǶȲ���ֵ
										 {
											 ReturnVal.g_n64ArrayCompenAngle[i] = (unsigned char)(ArrayAngle[i]);
										 }
									 }
									 if (f_AddEnergy)
									 {
										 for (int k = 0; k < BufLength / 2; k++)//��������ֵ
										 {
											 ReturnVal.g_n64ArrayEnergy[k] = (unsigned char)(ArrayEnergy[k]);
										 }
									 }
									 for (int m = 0; m < BufLength; m += 2)
									 {
										 nDataJi = ((g_sNetData.m_pcData[m] & 0xFF) << 8) + (BYTE)g_sNetData.m_pcData[m + 1];
										 if (nDataJi > g_n32MaxDis)//�����ֵ
											 nDataJi = g_n32MaxDis;
										 l_n64actualangle = nDataZhiCount * AngRatio + ReturnVal.g_n64ArrayCompenAngle[nDataZhiCount] / 100.0;
										 l_64nDataRegionZhiX = nDataJi *cos(l_n64actualangle / 180 * 3.14) / 1000.0;
										 l_64nDataRegionZhiY = nDataJi *sin(l_n64actualangle / 180 * 3.14) / 1000.0;
										 ReturnVal.g_n64ArrayJiX[nDataZhiCount] = m / 2;
										 ReturnVal.g_n64ArrayJiY[nDataZhiCount] = nDataJi;
										 ReturnVal.g_n64ArrayZhiX[nDataZhiCount] = l_64nDataRegionZhiX;
										 ReturnVal.g_n64ArrayZhiY[nDataZhiCount] = l_64nDataRegionZhiY;
										 nDataZhiCount++;
									 }
									 SendMessage(m_hReciWnd, WM_SERVER_SendData, (WPARAM)&ReturnVal, 0);
								 }
								 temp_Packnum = 0;
								 memset(&g_sNetData, 0, sizeof(g_sNetData));
								 g_n32CircleNo = CirclNo;
							 }
				}
					break;
#pragma endregion
				default:
					break;
				}
			}
		}
	}
	else if (g_cNetRecvBuf[g_n32NetRecvID][0] == (char)0x02 && g_cNetRecvBuf[g_n32NetRecvID][1] != (char)0x02)   //���������� sickЭ�� �жϵ���02��֡ͷ
	{
		memcpy(g_recvother, g_cNetRecvBuf[g_n32NetRecvID], nDataBufSize);
		if (g_recvother[12] == 84 && g_recvother[13] == 105)//Ti ���㼤����ʱ���
		{
			CString str_timp = g_recvother;
			int t_Space = 0, s_Timestap_index = 0;
			for (int i = 0; i < str_timp.GetLength(); i++)
			{
				if (str_timp[i] == ' ')
				{
					t_Space++;
					if (t_Space == 3)
					{
						s_Timestap_index = i + 1;
					}
				}
			}
			CString t_str = str_timp.Mid(s_Timestap_index, (str_timp.GetLength() - s_Timestap_index - 1));
			//LaserTimestap = strtoul(t_str, NULL, 16);
		}
		else
		{

			SendMessage(m_hReciWnd, WM_SICK_POSEREAD, (WPARAM)g_recvother, nDataBufSize);
		}
	}
	else
	{
		memset(g_recvother, 0, sizeof(g_recvother));
		memcpy(g_recvother, g_cNetRecvBuf[g_n32NetRecvID], nDataBufSize);
		SendMessage(m_hReciWnd, WM_SICK_POSEREAD, (WPARAM)g_recvother, nDataBufSize);
	}
	g_n32NetRecvID = (g_n32NetRecvID + 1) % NET_BUF_NUM;
}

void SendData(NetConnectPara*NetStruct)
{
	if (NetStruct->Len_Senddata <= 0)
	{
		return;
	}
	//�ѷ����ֽ���
	int iSentBytes = 0;
	//δ�����ֽ���;
	int iRemain = NetStruct->Len_Senddata;
	//������ʼλ��
	int iPos = 0;
	while (iRemain > 0)
	{
		iSentBytes = send(sockClient, &NetStruct->Senddata[iPos], iRemain, 0);
		if (iSentBytes == iRemain)
		{
			//ȫ���������
			break;
		}
		if (iSentBytes == SOCKET_ERROR)
		{
			return;
		}
		iRemain -= iSentBytes;
		iPos += iSentBytes;
	}
}

bool InitialUDPSocket()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	// ����socket api   
	wVersionRequested = MAKEWORD(2, 2);
	int err = WSAStartup(wVersionRequested, &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		WSACleanup();
	}
	brocastSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == brocastSocket)
	{
		err = WSAGetLastError();
		return false;
	}
	// �������׽���   
	SOCKADDR_IN sin_client;
	sin_client.sin_family = AF_INET;
	sin_client.sin_port = htons(CLIENT_PORT);
	sin_client.sin_addr.s_addr = 0;
	// �����������ϵĹ㲥��ַ��������   
	sin_server.sin_family = AF_INET;
	sin_server.sin_port = htons(SERVER_PORT);
	sin_server.sin_addr.s_addr = INADDR_BROADCAST;
	//���ø��׽���Ϊ�㲥���ͣ�   
	bool bOpt = true;//��ʾ����㲥
	setsockopt(brocastSocket, SOL_SOCKET, SO_BROADCAST, (char*)&bOpt, sizeof(bOpt));
	// ���׽���   
	err = bind(brocastSocket, (SOCKADDR*)&sin_client, sizeof(SOCKADDR));
	if (SOCKET_ERROR == err)
	{
		err = WSAGetLastError();
		return false;
		printf("\"bind\" error! error code is %d\n", err);
	}
	return true;
}

DWORD WINAPI FuncBrocast(LPVOID lparam)
{
	int nBroadtime = 0;
	char recvBuf[MAXLEN] = { 0 };
	int nAddrLen = sizeof(SOCKADDR);
	while (f_BroThreadCon)
	{
		if (nBroadtime < 10)
		{
			nBroadtime++;
			int nRevSize = recvfrom(brocastSocket, recvBuf, MAXLEN, 0, (SOCKADDR*)&sin_server, &nAddrLen);
			if (SOCKET_ERROR == nRevSize)
			{
				break;
			}
			if ((unsigned char)recvBuf[0] == 0xFF && (unsigned char)recvBuf[1] == 0xAA)
			{
				string IPAdress;
				IPAdress = to_string((unsigned char)recvBuf[26]) + "." + to_string((unsigned char)recvBuf[27]) + "." + to_string((unsigned char)recvBuf[28]) + "." + to_string((unsigned char)recvBuf[29]);
				string Port;
				Port = to_string(((recvBuf[38] & 0xFF) << 8) + (unsigned char)(recvBuf[39]));
				string str = IPAdress + " " + Port;
				if (IPAdress.length() > 10 && Port.length() > 1)
				{
					KillTimer(NULL, 0);
					StruBroPara strBro;
					strBro.ip = IPAdress.c_str();
					strBro.port = Port.c_str();
					SendMessage(m_hReciWnd, WM_BROCAST_PARA, (WPARAM)&strBro, 1);
					nBroadtime = 0;
					f_BroThreadCon = false;
					break;
				}
			}
		}
		else
		{
			SendMessage(m_hReciWnd, WM_BROCAST_PARA, 0, 0);
			break;
		}
	}
	return 0;
}

void CALLBACK onTimer(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime)
{
	if (brocastSocket)
	{
		sendto(brocastSocket, sendbroBuf, nbroLen, 0, (sockaddr*)&sin_server, sizeof(sin_server));
	}
}

void Brocast(HWND hReciveWnd, bool isOpenOrClose, StruBroZhiL struBroZL)
{
	if (isOpenOrClose)
	{
		sendbroBuf = struBroZL.broZL;
		nbroLen = struBroZL.lenZL;
		if (InitialUDPSocket())
		{
			m_hReciWnd = hReciveWnd;
			DWORD broThreadID = 0;
			if (!handleSecond)
			{
				CloseHandle(handleSecond);
				handleSecond = NULL;
			}
			f_BroThreadCon = true;
			handleSecond = CreateThread(NULL, 0, FuncBrocast, 0, 0, &broThreadID);
			SetTimer(m_hReciWnd, 0, 1000, onTimer);
		}
	}
	else
	{
		DWORD dwexitcode;
		if (closesocket(brocastSocket) == 0)
		{
			f_BroThreadCon = false;
			KillTimer(m_hReciWnd, 0);
			GetExitCodeThread(handleSecond, &dwexitcode);
			if (handleSecond != INVALID_HANDLE_VALUE&&handleSecond != 0)
			{
				TerminateThread(handleSecond, dwexitcode);
				WaitForSingleObject(handleSecond, 0);
				Sleep(200);
			}
		}
	}
}

