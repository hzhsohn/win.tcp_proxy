#include "proc.h"
#include <windows.h>


int main(int argc,char *argv[])
{
	if(argc<4)
	{
		printf("TCP数据传输监测代理工具启动参数:\r\n\r\n[目标域名或IP] [目标IP] [本地监听端口]\r\n\r\n");
		system("pause");
		exit(0);
	}

	printf("------------------------------------------------------------------\r\n");
	printf("-- TCP数据传输监测代理工具\r\n");
	printf("--\r\n");
	printf("-- 作者: Han Zhi Hong\r\n");
	printf("-- 产品网址: http://www.hx-kong.com\r\n");
	printf("-- 韩讯联控\r\n");
	printf("-- 版本v1.0\r\n");	
	printf("--\r\n");
	printf("------------------------------------------------------------------\r\n");
	printf("\r\n");
	printf("代理目标->%s:%s\r\n",argv[1],argv[2]);
	printf("监听端口->%s\r\n",argv[3]);
	
	InitTransChannel(argv[1],atoi(argv[2]),atoi(argv[3]));
	//开始运行逻辑
	InitProcThread();
	return 0;
}
