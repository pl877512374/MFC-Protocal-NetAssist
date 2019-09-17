#define WM_SERVER_SendData WM_USER + 162
#define WM_BROCAST_PARA WM_USER+163
#define WM_SICK_POSEREAD WM_USER + 164
typedef struct NetConnectPara
{
	char *Senddata;
	int Len_Senddata;
	DWORD IP;
	int Port;
	bool ConnectState;
};
typedef struct ReturnVal
{
	int g_n64ArrayJiX[7200];//极坐标X
	int g_n64ArrayJiY[7200];//极坐标Y
	int g_n64ArrayCompenAngle[7200];//角度补偿数组
	int g_n64ArrayEnergy[7200];//能量值数组
	double g_n64ArrayZhiX[7200];
	double g_n64ArrayZhiY[7200];
	double g_n64ArrayZhiZ[7200];
	int Ratio;//分辨率 0:0.05 1:0.1
} StruReturnVal;
typedef struct BroPara
{
	const char * ip;
	const char * port;
}StruBroPara;
typedef struct BroZhiL
{
	char * broZL;
	int lenZL;
}StruBroZhiL;
void OnNetRecv(char *pDataBuf, int nDataBufSize);
bool InitialUDPSocket();
void CALLBACK onTimer(HWND hwnd, UINT message, UINT idTimer, DWORD dwTime);
bool checkXor(char* recvbuf, int recvlen);
bool ConnectServer(HWND hReciveWnd, NetConnectPara*NetStruct);
bool DisconnectServer(NetConnectPara*NetStruct);
void SendData(NetConnectPara*NetStruct);
void Brocast(HWND hReciveWnd, bool isOpenOrClose, StruBroZhiL broZL);
