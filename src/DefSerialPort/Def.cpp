// Def.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include"DefSerialPort.h"
#include<iostream>

static CDefSerialPort *def;

void __CBFun(const char *pBuf,size_t szLen)
{
	std::cout<<pBuf<<"  szLen="<<szLen<<std::endl;
	def->Send(pBuf,szLen);
}

int _tmain(int argc, _TCHAR* argv[])
{
	def=new CDefSerialPort();

	std::vector<std::string> vecComs;
	def->GetLocalComs(vecComs);

	if(!def->Start(vecComs[0].c_str(),"9600,n,8,1",__CBFun))
		return 0;

	while(1)
	{
		if(getchar()=='q')
			break;
	}
	def->Stop();
	delete def;
	return 0;
}

