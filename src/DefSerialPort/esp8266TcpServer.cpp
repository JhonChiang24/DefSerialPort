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

void CEsp8266TcpServer::_spCBFun(const char *pBuf,size_t szLen)
{
	std::cout<<pBuf<<std::endl;
}

