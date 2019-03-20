#pragma once
#include"DefSerialPort.h"
#include<iostream>

class CEsp8266TcpServer
{
	BOOL Open()
	{
		return m_spMain.Start("COM1","115200,8,n,1",_spCBFun);
	}
	void Close()
	{
		m_spMain.Stop();
	}

	void GetVersion();

private:
	CDefSerialPort m_spMain;
	static void _spCBFun(const char *pBuf,size_t szLen);

};