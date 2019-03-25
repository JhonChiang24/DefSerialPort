#include"stdafx.h"
#include"esp8266TcpServer.h"

void CEsp8266TcpServer::GetVersion()
{
	std::string strAt="AT+GMR\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetRst()
{
	std::string strAt="AT+RST\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::_spCBFun(const char *pBuf,size_t szLen,void *lp)
{
	CEsp8266TcpServer *p=reinterpret_cast<CEsp8266TcpServer *>(lp);
	p->m_strRecv.append(pBuf,szLen);
////	p->m_strRecv.append("12356",5);
//	std::string str="jjjjjjjjj,.o[.jjjjjjjjjjjjjj";
//	str.append(p->m_strRecv.c_str(),p->m_strRecv.size());
//	p->m_strRecv=str;
	p->parsingTcpRecv();
//	std::cout<<pBuf<<std::endl/*<<"strlen="<<strlen(pBuf)<<"  szLen=" <<szLen*/<<std::endl;
}

void CEsp8266TcpServer::parsingTcpRecv()
{
	while(m_strRecv.size()>0)
	{
		std::string strHead1="\r\n+IPD";
		int iPos1=m_strRecv.find(strHead1);
		if(iPos1==std::string::npos)
			return ;

		//解析头元素
		int iPos2=iPos1;
		for(int i=0;i<3;i++) //3个“,”隔开解析ID和buf长
		{
			iPos2=m_strRecv.find(",",iPos2+1);
				if(iPos2==std::string::npos)
					return ;
		}

		int iPos3=iPos2,iPos4=0;
		iPos3=m_strRecv.find(",",iPos3+1);
		if(iPos3==std::string::npos)
			return ;
		iPos4=iPos3;
		iPos4=m_strRecv.find(":",iPos4+1);
		if(iPos4==std::string::npos)
			return ;

		size_t szClientID=0,szBufLen=0;
		char cIP[16]={0};size_t szPort=0;
		char cBuf[20]={0};
		memcpy(cBuf,m_strRecv.c_str()+strHead1.size()+iPos1,iPos2-iPos1-strHead1.size());
		sscanf_s(cBuf,",%u,%u",&szClientID,&szBufLen);
	
		memset(cBuf,0,20);
		memcpy(cBuf,m_strRecv.c_str()+iPos2,iPos3-iPos2);
		memset(cBuf,0,20);
		memcpy(cBuf,m_strRecv.c_str()+iPos3,iPos4-iPos3);

		static ULONGLONG ullRecv=0;
		ullRecv+=szBufLen;
		std::cout<<"RecvSize="<<ullRecv<<std::endl;

		const char *pNew=m_strRecv.c_str()+iPos4+szBufLen+1;
		if(m_strRecv.size()-iPos4-1>szBufLen)//一定要注意，长度够长也许是前面的遗留，所以要这样截取计算
		{
			size_t szAll=m_strRecv.size();
			m_strRecv.clear();
			m_strRecv.append(pNew,szAll-(iPos4+szBufLen+1));
		}
		else
			m_strRecv.clear();
	}
}

void CEsp8266TcpServer::QueryMode()
{
	std::string strAt="AT+CWMODE?\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetMode()
{
	std::string strAt="AT+CWMODE_CUR=2\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::QueryDHCP()
{
	std::string strAt="AT+CWDHCP?\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::GetLAPs()
{
	std::string strAt="AT+CWLAP\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetAPInfo()
{
	std::string strAt="AT+CWSAP_DEF=\"ESP8266-jsc022\",\"12345678\",5,3\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::QueryAPMac()
{
	std::string strAt="AT+CIPAPMAC?\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::QueryDHCPs()
{
	std::string strAt="AT+CWDHCPS?\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::QueryAPIP()
{
	std::string strAt="AT+CIPAP?\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::GetStaInfo()
{
	std::string strAt="AT+CWLIF\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::GetLocalIP()
{
	std::string strAt="AT+CIFSR\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetIPMux()
{
	std::string strAt="AT+CIPMUX=1\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetIPTcpServer()
{
	std::string strAt="AT+CIPSERVER=1,6200\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}

void CEsp8266TcpServer::SetRecvIPPort()
{
	std::string strAt="AT+CIPDINFO=1\r\n";
	m_spMain.Send(strAt.c_str(),strAt.size());
}