#include"stdafx.h"
#include"DefSerialPort.h"

CDefSerialPort::CDefSerialPort()
	:m_cbFun(NULL)
	,m_hPort(INVALID_HANDLE_VALUE)
	,m_bThreadOut(true)
	,m_hThread(NULL)
	,m_UserPtr(NULL)
{
	m_osRead.hEvent=NULL;
	m_osWrite.hEvent=NULL;
	m_osWait.hEvent=NULL;
}

CDefSerialPort::~CDefSerialPort()
{
	Stop();
}

BOOL CDefSerialPort::Start(const char *pComNum,const char *pSettings,const __CBFun& fun,void *pUserPtr)
{
	CMakeCritical cri(m_mtxOpt);

	if(m_hPort!=INVALID_HANDLE_VALUE)
		return FALSE;

	m_cbFun=fun;
	m_UserPtr=pUserPtr;
	int iBaud=0,iByteSize=0,iStopBits=0;
	char cParity='\0';
	sscanf_s(pSettings,"%d,%c,%d,%d",&iBaud,&cParity,1,&iByteSize,&iStopBits);

	m_hPort=CreateFile(pComNum,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL); // 重叠方式
	if(m_hPort==INVALID_HANDLE_VALUE)
		return FALSE;

	SetupComm(m_hPort,MAX_IO_LEN,MAX_IO_LEN);
	SetCommMask(m_hPort,EV_TXEMPTY|EV_RXCHAR);//设置等待事件

	if(!SetTimeOut(100,100,500,100,500)||!PortSetings(iBaud,cParity,iByteSize,iStopBits))//超时设置(内部设置)
	{
		Stop();
		return FALSE;
	}
	_SetEvent();

	m_bThreadOut=false;
	m_hThread=(HANDLE)_beginthreadex(NULL,0,thread_Recv,this,0,NULL);
	return TRUE;
}

void CDefSerialPort::Stop()
{
	CMakeCritical cri(m_mtxOpt);

	m_bThreadOut=true;
	WaitForSingleObject(m_hThread,INFINITE);
	CloseHandle(m_hThread);
	m_hThread=NULL;

	_ReleaseEvent();

	if(m_hPort!=INVALID_HANDLE_VALUE)
	{
		PurgeComm(m_hPort,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
		CloseHandle(m_hPort);
	}
	m_hPort=INVALID_HANDLE_VALUE;
}

BOOL CDefSerialPort::SetTimeOut(DWORD dwRinterval,DWORD dwRtotalmultiplier,DWORD dwRtotalconstant,DWORD dwWtotalmultiplier,DWORD dwWtotalconstant)
{
	  COMMTIMEOUTS timeouts;
	  if(!GetCommTimeouts(m_hPort, &timeouts))
		  return FALSE;

      timeouts.ReadIntervalTimeout=dwRinterval;
	  timeouts.ReadTotalTimeoutMultiplier=dwRtotalmultiplier;
      timeouts.ReadTotalTimeoutConstant=dwRtotalconstant;
	  timeouts.WriteTotalTimeoutMultiplier=dwWtotalmultiplier;
      timeouts.WriteTotalTimeoutConstant=dwWtotalconstant;
     
      if(!SetCommTimeouts(m_hPort,&timeouts))
		 return FALSE;
      else
		 return TRUE;
}

BOOL CDefSerialPort::PortSetings(int iBaud,char cParity,int iByteSize,int iStopBits)
{
	DCB dcb;
	if(!GetCommState(m_hPort,&dcb))
		return FALSE;

	dcb.fBinary=1;
	dcb.BaudRate=iBaud;
	switch(cParity)
	{
	case 'n':
		dcb.Parity=NOPARITY;
		break;
	case 'o':
		dcb.fParity=1;
		dcb.Parity=ODDPARITY;
		break;
	case 'e':
		dcb.fParity=1;
		dcb.Parity=EVENPARITY;
		break;
	case 'm':
		dcb.Parity=MARKPARITY;
		break;
	case  's':
		dcb.Parity=SPACEPARITY;
		break;
	default://AfxMessageBox(_T("串口校验参数设置错误..."));
		return FALSE;
	}
	dcb.ByteSize=iByteSize;
	switch(iStopBits)
	{
	case 1:
		dcb.StopBits=ONESTOPBIT;
		break;
	case 3://1.5停止位
		dcb.StopBits=ONE5STOPBITS;
		break;
	case 2:
		dcb.StopBits=TWOSTOPBITS;
		break;
	default://AfxMessageBox(_T("串口停止位参数设置错误..."));
		return FALSE;
	}
	//硬件流控制
	/*dcb.fOutxCtsFlow=TRUE;
	dcb.fRtsControl=TRUE;

    dcb.fInX=TRUE;
    dcb.fOutX=TRUE;

	dcb.XonChar=(char)0;
	dcb.XoffChar=(char)0;
	dcb.XonLim=50;
	dcb.XoffLim=50;*/
    dcb.fDtrControl=DTR_CONTROL_ENABLE;//数据终端准备好
    dcb.fRtsControl=RTS_CONTROL_ENABLE;//请求发送使能
    dcb.fOutxCtsFlow=FALSE;//允许发送
	dcb.fOutxDsrFlow=FALSE;

	if(!SetCommState(m_hPort,&dcb))
		return FALSE;
	
	PurgeComm(m_hPort,PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);
	return TRUE;
}

void CDefSerialPort::_SetEvent()
{
	memset(&m_osRead,0,sizeof(OVERLAPPED));
	memset(&m_osWrite,0,sizeof(OVERLAPPED));
	memset(&m_osWait,0,sizeof(OVERLAPPED));
	m_osRead.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);//手动置位,初始非触发(释放)
	m_osWrite.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_osWait.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

/*	DWORD dwEvtMask=0;
	GetCommMask(m_hPort,&dwEvtMask);
 	dwEvtMask|=EV_RXCHAR;
 	SetCommMask(m_hPort,dwEvtMask); */
}

void CDefSerialPort::_ReleaseEvent()
{
	if(m_osRead.hEvent!=NULL&&m_osWrite.hEvent!=NULL&&m_osWait.hEvent!=NULL)
	{
		CloseHandle(m_osRead.hEvent);
		CloseHandle(m_osWrite.hEvent);
		CloseHandle(m_osWait.hEvent);
	}
	m_osRead.hEvent=NULL;
	m_osWrite.hEvent=NULL;
	m_osWait.hEvent=NULL;
}

BOOL CDefSerialPort::Send(const char *pBuf,size_t szLen)
{
	CMakeCritical cri(m_mtxOpt);
	if(m_hPort==INVALID_HANDLE_VALUE)
		return FALSE;

	DWORD dwBytesWritten=0,dwErrorFlags=0;
	ClearCommError(m_hPort,&dwErrorFlags,&m_comWrite);

	ResetEvent(m_osWrite.hEvent);
	BOOL bWriteStatus=WriteFile(m_hPort,pBuf,szLen,&dwBytesWritten,&m_osWrite);
	if(!bWriteStatus)
	{
		if(GetLastError()==ERROR_IO_PENDING)
		{
			while(!GetOverlappedResult(m_hPort,&m_osWrite,&dwBytesWritten,TRUE))
			{
				if(GetLastError() == ERROR_IO_INCOMPLETE) 
 					continue;
			}
			if(dwBytesWritten!=szLen)
				return FALSE;
		}
	}

	if(dwBytesWritten!=szLen)
		return FALSE;
	return TRUE;
}

size_t CDefSerialPort::Recv(char *pRecv,size_t szLen)
{
	DWORD dwBytesRead=0,dwErrorFlags=0,dwEvtMask=0;

/*	ResetEvent(m_osWait.hEvent);
	BOOL BWait=WaitCommEvent(m_hPort,&dwEvtMask,&m_osWait);
	if(!BWait)
	{
		if(GetLastError()==ERROR_IO_PENDING)
			WaitForSingleObject(m_osWait.hEvent, INFINITE);
		else
			return 0;
	}

 	if((dwEvtMask & EV_RXCHAR) != EV_RXCHAR)
		return 0;  */

	ClearCommError(m_hPort,&dwErrorFlags,&m_comRead);//清除通信错误(会清空上次已接收的缓冲区),并获取通信缓冲区状态
	if(m_comRead.cbInQue<=0)
		return dwBytesRead;

	ResetEvent(m_osRead.hEvent);//手动置位(无信号)
	BOOL bReadStatus=ReadFile(m_hPort,pRecv,szLen,&dwBytesRead,&m_osRead);//(接收够dwLen或超时)执行完这条,m_osRead.hEvent有信号
	if(!bReadStatus)
	{
		if(GetLastError()==ERROR_IO_PENDING)
			while(!GetOverlappedResult(m_hPort,&m_osRead,&dwBytesRead,TRUE))
			{
				if(GetLastError() == ERROR_IO_INCOMPLETE) 
 					continue;
			}
		else
			return (dwBytesRead=0);
	}
	return dwBytesRead;
}

unsigned int __stdcall CDefSerialPort::thread_Recv(void *lp)
{
	CDefSerialPort *p=reinterpret_cast<CDefSerialPort *>(lp);

	char cRecv[MAX_IO_LEN]={0};
	size_t szLen=0;

	while(1)
	{
		Sleep(1);
		if(p->m_bThreadOut)
			break;

		memset(cRecv,0,MAX_IO_LEN);
		szLen=p->Recv(cRecv,MAX_IO_LEN);
		if(szLen>0)
			p->m_cbFun(cRecv,szLen,p->m_UserPtr);
	}

	return 0;
}

void CDefSerialPort::GetLocalComs(std::vector<std::string>& vecComs)
{
	HKEY hKey;
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("Hardware\\DeviceMap\\SerialComm"),NULL, KEY_READ,&hKey)!=ERROR_SUCCESS)
		return ;
    DWORD dwPortName=MAX_PATH;
    DWORD dwComName=MAX_PATH;
    TCHAR cPortName[MAX_PATH];
    TCHAR cComName[MAX_PATH];
    DWORD dwIndex=0;
    while(1)
    {  
        ZeroMemory(cPortName,MAX_PATH);
        ZeroMemory(cComName,MAX_PATH);
        dwPortName=MAX_PATH;
        dwComName=MAX_PATH;
        if(RegEnumValue(hKey,dwIndex,cPortName,&dwPortName,NULL, NULL,(LPBYTE)cComName, &dwComName)==ERROR_NO_MORE_ITEMS)  
			break;
		vecComs.push_back(cComName);
        dwIndex++;
    }
    RegCloseKey(hKey);
}