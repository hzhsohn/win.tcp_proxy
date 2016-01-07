#include "proc.h"
#include "data_collect.h"
#include "ck_assist.h"
#include <io.h>
#include "cfg_operator.h"

/*///////////////////////////////////////////////////////

 ERR_UTF8_PRINTF错误值由30000开始  现30048
	
/*///////////////////////////////////////////////////////

TckTcpSocketCfg g_websock;

//建立转换通道
void InitTransChannel(char* target_host,int target_port,int bind_prot)
{	
		//新建转换通道
		srcCfgAddTcp(&g_websock,"tcp1",target_host,target_port);
		srcCfgAddTransSocket(&g_websock,bind_prot,"tcp1");
}

void InitProcThread()
{
		INIT_CS(&g_websock.obj.collect.logic.csTcp);
		INIT_CS(&g_websock.obj.collect.logic.csInsideCmd);
		INIT_CS(&g_websock.obj.collect.logic.csOutsideCmd);

		//当装载了g_websock和互斥锁初始化
		openCollect();

		while(true)
		{
			collectProcess();
		}

		DELETE_CS(&g_websock.obj.collect.logic.csTcp);
		DELETE_CS(&g_websock.obj.collect.logic.csInsideCmd);
		DELETE_CS(&g_websock.obj.collect.logic.csOutsideCmd);
}
