#pragma once
#include"DefSerialPort.h"
#include<iostream>

class CEsp8266TcpServer
{
public:
	BOOL Open()
	{
		return m_spMain.Start("COM4","115200,n,8,1",_spCBFun,this);
	}
	void Close()
	{
		m_spMain.Stop();
	}

	void GetVersion();//获取版本
	void SetRst(); //重启
	void QueryMode();//sta /AP模式？
	void SetMode();//sta /AP模式
	void QueryDHCP(); //是否开启DHCP
	void GetLAPs();//当前可用APs
	void SetAPInfo();//wifi热点
	void QueryAPMac();//AP的mac地址
	void QueryDHCPs();//查询分配的ip范围 (查询错误)
	void QueryAPIP();//查询AP当前IP
	void GetStaInfo();//获取连接到热点的sta信息
	void GetLocalIP();//获取当前IP
	void SetIPMux();//设置多连接 server必需
	void SetIPTcpServer();
	void SetRecvIPPort();//接收网络数据时显示ip和端口

	std::string m_strRecv;
private:
	CDefSerialPort m_spMain;
	static void _spCBFun(const char *pBuf,size_t szLen,void *lp);

	void parsingTcpRecv();//解析协议
};