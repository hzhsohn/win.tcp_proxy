#include "tcp_logic.h"
#include "socket.h"
#include "data_cmdlist.h"
#include "ck_define.h"

extern TckTcpSocketCfg g_websock;
extern TzhList g_userList;

//串口处理的函数
void tcpProcess()
{
	LOCK_CS(&g_websock.obj.collect.logic.csTcp);

	_checkTcpLogic();
	_writeTcpLogic();
	_readTcpLogic();

	UNLOCK_CS(&g_websock.obj.collect.logic.csTcp);
}

void _sendTcpStatus_cb(char*source,bool enable)
{

}

void _tcpCfgConnecting(TckSrcCfgTcp* pTcp)
{
	bool ret;

	LOCK_CS(&pTcp->logic.csStatus);
	pTcp->logic.tStatus=eckProjTcpConnecting;
	UNLOCK_CS(&pTcp->logic.csStatus);

	if(!zhSockInit(&pTcp->logic.s,ZH_SOCK_TCP))
	{
		ERR_PRINTF("Init Socket Fail..Tcp Source=%s",pTcp->obj.source);
		goto _end;
	}
	if(!zhSockSetNonBlocking(pTcp->logic.s,false))
	{
		ERR_PRINTF("SetNonBlocking Fail..Tcp Source=%s",pTcp->obj.source);
		goto _end;
	}
	if(!zhSockSetReuseAddr(pTcp->logic.s,true))
	{
		ERR_PRINTF("Reuse Addr Fail..Tcp Source=%s",pTcp->obj.source);
		goto _end;
	}
	//TCP缓冲区
	zhSockSetSendBufferSize(pTcp->logic.s,2*1024);//设置发送缓冲区
	zhSockSetRecvBufferSize(pTcp->logic.s,8*1024);//设置接收缓冲区
	ret=zhSockBlockingConnect(pTcp->logic.s,pTcp->obj.host,pTcp->obj.port,0,10);
	if(ret)
	{
		pTcp->logic.isAlertConnectedFail=false;
		zhSockSetNonBlocking(pTcp->logic.s,true);
		LOCK_CS(&pTcp->logic.csStatus);

		pTcp->logic.tStatus=eckProjTcpConnected;
		_sendTcpStatus_cb(pTcp->obj.source,true);

		UNLOCK_CS(&pTcp->logic.csStatus);
		DEBUG_PRINTF("成功连接到代理目标, 源=%s, 目标服务地址=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
		//重新连接删除其里面所有执行的TCP数据信息
		DataCmdDeleteAll(&pTcp->logic.lst_waitCmd);
		return;
	}
	else
	{
		if(false==pTcp->logic.isAlertConnectedFail)
		{
			pTcp->logic.isAlertConnectedFail=true;
			DEBUG_ERROR("连接到代理目标失败, 源=%s, 目标服务地址=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
		}
		goto _end;
	}
_end:
	LOCK_CS(&pTcp->logic.csStatus);
	pTcp->logic.tStatus=eckProjTcpDisconnect;
	zhSockClose(pTcp->logic.s);
	UNLOCK_CS(&pTcp->logic.csStatus);
}

//打开所有TCP连接
void openTcpCfg()
{
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;

	LOCK_CS(&g_websock.obj.collect.logic.csTcp);
	//遍历所有TCP连接信息节点
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.obj.lst_eleTCP);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pTcp=(TckSrcCfgTcp*)eleNode->pElement;
	
		pTcp->logic.tmeLastReopen=zhPlatGetTime();
		CREATE_THREAD(_tcpCfgConnecting,pTcp);
	}
	UNLOCK_CS(&g_websock.obj.collect.logic.csTcp);
}

//关闭所有TCP连接
void closeTcpCfg()
{
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;

	LOCK_CS(&g_websock.obj.collect.logic.csTcp);
	//遍历所有TCP
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.obj.lst_eleTCP);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pTcp=(TckSrcCfgTcp*)eleNode->pElement;
		zhSockClose(pTcp->logic.s);
		
		pTcp->logic.tStatus=eckProjTcpDisconnect;
		_sendTcpStatus_cb(pTcp->obj.source,false);
	}
	UNLOCK_CS(&g_websock.obj.collect.logic.csTcp);
}

void _writeTcpLogic()
{
	TzhListElementNode *eleNode;
	TzhListElementNode *eleNode2;
	TzhListDataCmd *listWaitCmd;
	int ret;
	DWORD dwCurTime;
	TckSrcCfgTcp *pTcp;

	ret=0;

	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.obj.lst_eleTCP);
        eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pTcp=(TckSrcCfgTcp*)eleNode->pElement;

		//循环当前可以执行的记录,因为某些串口命令记录具有delay所以遍历,当第一个不够时间执行时
		//会判断下一条是否够时间执行
		LOCK_CS(&pTcp->logic.csWaitCmd);
		for(eleNode2 = (TzhListElementNode *)zhListFirst(&pTcp->logic.lst_waitCmd);
        eleNode2 != NULL;)
		{
			listWaitCmd=(TzhListDataCmd *)eleNode2->pElement;
			dwCurTime=zhPlatGetTime();
			if(dwCurTime >= listWaitCmd->dwOperatorTime)
			{
				if(eckProjTcpConnected==pTcp->logic.tStatus)
				{
					//调试输出16进制信息
					//DEBUG_PRINT16(listWaitCmd->rLen,listWaitCmd->btCmd,"send->");

					ret=zhSockSend(pTcp->logic.s,(char*)listWaitCmd->btCmd,listWaitCmd->rLen);
					if(listWaitCmd->rLen==ret)
					{
						//TCP发送成功,删除数据
						eleNode2=DataCmdDeleteElementNode(&pTcp->logic.lst_waitCmd,eleNode2);
					}
					else if(ret>0)
					{
						listWaitCmd->rLen-=ret;
						memmove(&listWaitCmd->btCmd[0],&listWaitCmd->btCmd[ret],listWaitCmd->rLen);
					}
					else if(SOCKET_ERROR==ret)
					{
						if(eckProjTcpConnected==pTcp->logic.tStatus)
						{
							_sendTcpStatus_cb(pTcp->obj.source,false);
							DEBUG_PRINTF("代理目标断开连接, 源=%s, 目标服务地址=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
						}
						LOCK_CS(&pTcp->logic.csStatus);
						pTcp->logic.tStatus=eckProjTcpDisconnect;
						UNLOCK_CS(&pTcp->logic.csStatus);
					}
				}
				else
				{
					//TCP已经断开,删除数据
					eleNode2=DataCmdDeleteElementNode(&pTcp->logic.lst_waitCmd,eleNode2);
				}
			}
			else
			{
				eleNode2 = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode2);
			}
		}
		UNLOCK_CS(&pTcp->logic.csWaitCmd);
	}
}

void _readTcpLogic()
{
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;
	unsigned char szTmpBuf[ZH_DATA_SOURCE_RECV_SIZE+1];
	int nBufSize;

	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.obj.lst_eleTCP);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pTcp=(TckSrcCfgTcp*)eleNode->pElement;
		nBufSize=zhSockRecv(pTcp->logic.s,(char*)szTmpBuf,sizeof(szTmpBuf));
		if(nBufSize>0)
		{
			//TCP读入数据,添加到内部链表
			LOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
			DataCmdAdd(&g_websock.obj.collect.logic.lst_insideCmd,pTcp->obj.source,szTmpBuf,nBufSize,0);
			UNLOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
		}
		else if(SOCKET_ERROR==nBufSize)
		{
			if(eckProjTcpConnected==pTcp->logic.tStatus)
			{
				_sendTcpStatus_cb(pTcp->obj.source,false);
				DEBUG_PRINTF("代理目标断开连接, 源=%s, 目标服务地址=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
			}
			LOCK_CS(&pTcp->logic.csStatus);
			pTcp->logic.tStatus=eckProjTcpDisconnect;
			UNLOCK_CS(&pTcp->logic.csStatus);
		}
	}
}

void _checkTcpLogic()
{
	DWORD dwCurTime;
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;

	//打开所有串口
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.obj.lst_eleTCP);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pTcp=(TckSrcCfgTcp*)eleNode->pElement;
		dwCurTime=zhPlatGetTime();
		if(dwCurTime - pTcp->logic.tmeLastReopen>ZH_CHECK_TCP_DATA_INTERVAL)
		{
			pTcp->logic.tmeLastReopen=dwCurTime;

			if(eckProjTcpDisconnect==pTcp->logic.tStatus)
			{
				CREATE_THREAD(_tcpCfgConnecting,pTcp);

				//重新连接
				DEBUG_PRINTF("尝试重新连接代理目标, 源=%s, 目标服务地址=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
			}
		}
	}
}
