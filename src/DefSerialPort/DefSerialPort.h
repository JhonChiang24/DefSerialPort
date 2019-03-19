#ifndef __DEFSERIALPORT_H__
#define __DEFSERIALPORT_H__

#include"CriticalSection.h"
#include<vector>
#include<process.h>

class CDefSerialPort
{
private:
	typedef void (* __CBFun)(const char *pBuf,size_t szLen);
public:
	CDefSerialPort();
	virtual ~CDefSerialPort();

	BOOL Start(const char *pComNum,const char *pSettings,const __CBFun& fun);//("COM1","9600,n,8,1")
	void Stop();
	BOOL Send(const char *pBuf,size_t szLen);
	void GetLocalComs(std::vector<std::string>& vecComs);

private:
	__CCriticalSection m_mtxOpt;
	__CBFun m_cbFun;
	HANDLE m_hPort;//串口句柄

	COMSTAT m_comRead,m_comWrite;
	OVERLAPPED m_osRead,m_osWrite;

	#define MAX_IO_LEN (1024)

	BOOL SetTimeOut(DWORD dwRinterval,DWORD dwRtotalmultiplier,DWORD dwRtotalconstant,DWORD dwWtotalmultiplier,DWORD dwWtotalconstant);//超时设置
	BOOL PortSetings(int iBaud,char cParity,int iByteSize,int iStopBits);//串口设置

	void _SetEvent();
	void _ReleaseEvent();

	size_t Recv(char *pRecv,size_t szLen);

	bool m_bThreadOut;
	HANDLE m_hThread;
	static unsigned int __stdcall thread_Recv(void *lp);
};

#endif