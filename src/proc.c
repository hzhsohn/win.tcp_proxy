#include "proc.h"
#include "data_collect.h"
#include "ck_assist.h"
#include <io.h>
#include "cfg_operator.h"

/*///////////////////////////////////////////////////////

 ERR_UTF8_PRINTF����ֵ��30000��ʼ  ��30048
	
/*///////////////////////////////////////////////////////

TckTcpSocketCfg g_websock;

//����ת��ͨ��
void InitTransChannel(char* target_host,int target_port,int bind_prot)
{	
		//�½�ת��ͨ��
		srcCfgAddTcp(&g_websock,"tcp1",target_host,target_port);
		srcCfgAddTransSocket(&g_websock,bind_prot,"tcp1");
}

void InitProcThread()
{
		INIT_CS(&g_websock.obj.collect.logic.csTcp);
		INIT_CS(&g_websock.obj.collect.logic.csInsideCmd);
		INIT_CS(&g_websock.obj.collect.logic.csOutsideCmd);

		//��װ����g_websock�ͻ�������ʼ��
		openCollect();

		while(true)
		{
			collectProcess();
		}

		DELETE_CS(&g_websock.obj.collect.logic.csTcp);
		DELETE_CS(&g_websock.obj.collect.logic.csInsideCmd);
		DELETE_CS(&g_websock.obj.collect.logic.csOutsideCmd);
}
