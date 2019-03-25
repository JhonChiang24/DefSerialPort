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

	void GetVersion();//��ȡ�汾
	void SetRst(); //����
	void QueryMode();//sta /APģʽ��
	void SetMode();//sta /APģʽ
	void QueryDHCP(); //�Ƿ���DHCP
	void GetLAPs();//��ǰ����APs
	void SetAPInfo();//wifi�ȵ�
	void QueryAPMac();//AP��mac��ַ
	void QueryDHCPs();//��ѯ�����ip��Χ (��ѯ����)
	void QueryAPIP();//��ѯAP��ǰIP
	void GetStaInfo();//��ȡ���ӵ��ȵ��sta��Ϣ
	void GetLocalIP();//��ȡ��ǰIP
	void SetIPMux();//���ö����� server����
	void SetIPTcpServer();
	void SetRecvIPPort();//������������ʱ��ʾip�Ͷ˿�

	std::string m_strRecv;
private:
	CDefSerialPort m_spMain;
	static void _spCBFun(const char *pBuf,size_t szLen,void *lp);

	void parsingTcpRecv();//����Э��
};