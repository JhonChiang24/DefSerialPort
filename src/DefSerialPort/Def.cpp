// Def.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"DefSerialPort.h"
#include"esp8266TcpServer.h"
#include<iostream>

static CDefSerialPort *def;

void __CBFun(const char *pBuf,size_t szLen,void *lp)
{
	std::cout<<pBuf<<"  szLen="<<szLen<<std::endl<<std::endl;
//	def->Send(pBuf,szLen);
}

int _tmain(int argc, _TCHAR* argv[])
{
	char *p=",0,1024,192.168.0.1,8989,";
	int i1=0,i2=0;
	char cBuf[16]={0};
	int iPort=0;
	sscanf_s(p,",%d,%d,%s,%d,",&i1,&i2,cBuf,16,&iPort);

#if 0

	def=new CDefSerialPort();

	std::vector<std::string> vecComs;
	def->GetLocalComs(vecComs);

	if(!def->Start(vecComs[2].c_str(),"115200,n,8,1",__CBFun))
		return 0;

	while(1)
	{
		//std::string strAt="AT+RST\r\n";
		//std::string strAt="AT+UART?\r\n";
		//std::string strAt="AT+CIFSR?\r\n";
		std::string strAt="AT+CWMODE?\r\n";
		char c=getchar();
		if(c=='q')
			break;
		else if(c=='s')
			//def->Send("AT+UART_CUR=115200,8,1,0,1\r\n",strlen("AT+UART_CUR=115200,8,1,0,1\r\n"));
			//def->Send("AT+UART?\r\n",strlen("AT+UART?\r\n"));
			def->Send(strAt.c_str(),strAt.size());
	}
	def->Stop();
	delete def;
#else

	CEsp8266TcpServer logical;
	if(!logical.Open())
		return 0;

	while(1)
	{
		char c=getchar();

		if(c=='q')
			break;
		else if(c=='1')
			logical.GetVersion();
		else if(c=='2')
			logical.SetRst();
		else if(c=='3')
			logical.QueryMode();
		else if(c=='4')
			logical.QueryDHCP();
		else if(c=='5')
			logical.GetLAPs();
		else if(c=='6')
			logical.SetAPInfo();
		else if(c=='7')
			logical.SetMode();
		else if(c=='8')
			logical.QueryAPMac();
		else if(c=='9')
			logical.QueryDHCPs();
		else if(c=='0')
			logical.QueryAPIP();
		else if(c=='w')
			logical.GetStaInfo();

/*开启TCPServer*/
		else if(c=='e')
			logical.SetIPMux();
		else if(c=='r')
			logical.SetIPTcpServer();
		else if(c=='t')
			logical.SetRecvIPPort();
		/*******************************/
		else
			;
	}

#endif

	return 0;
}

