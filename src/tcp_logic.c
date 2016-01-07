#include "tcp_logic.h"
#include "socket.h"
#include "data_cmdlist.h"
#include "ck_define.h"

extern TckTcpSocketCfg g_websock;
extern TzhList g_userList;

//���ڴ���ĺ���
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
	//TCP������
	zhSockSetSendBufferSize(pTcp->logic.s,2*1024);//���÷��ͻ�����
	zhSockSetRecvBufferSize(pTcp->logic.s,8*1024);//���ý��ջ�����
	ret=zhSockBlockingConnect(pTcp->logic.s,pTcp->obj.host,pTcp->obj.port,0,10);
	if(ret)
	{
		pTcp->logic.isAlertConnectedFail=false;
		zhSockSetNonBlocking(pTcp->logic.s,true);
		LOCK_CS(&pTcp->logic.csStatus);

		pTcp->logic.tStatus=eckProjTcpConnected;
		_sendTcpStatus_cb(pTcp->obj.source,true);

		UNLOCK_CS(&pTcp->logic.csStatus);
		DEBUG_PRINTF("�ɹ����ӵ�����Ŀ��, Դ=%s, Ŀ������ַ=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
		//��������ɾ������������ִ�е�TCP������Ϣ
		DataCmdDeleteAll(&pTcp->logic.lst_waitCmd);
		return;
	}
	else
	{
		if(false==pTcp->logic.isAlertConnectedFail)
		{
			pTcp->logic.isAlertConnectedFail=true;
			DEBUG_ERROR("���ӵ�����Ŀ��ʧ��, Դ=%s, Ŀ������ַ=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
		}
		goto _end;
	}
_end:
	LOCK_CS(&pTcp->logic.csStatus);
	pTcp->logic.tStatus=eckProjTcpDisconnect;
	zhSockClose(pTcp->logic.s);
	UNLOCK_CS(&pTcp->logic.csStatus);
}

//������TCP����
void openTcpCfg()
{
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;

	LOCK_CS(&g_websock.obj.collect.logic.csTcp);
	//��������TCP������Ϣ�ڵ�
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

//�ر�����TCP����
void closeTcpCfg()
{
	TzhListElementNode*eleNode;
	TckSrcCfgTcp* pTcp;

	LOCK_CS(&g_websock.obj.collect.logic.csTcp);
	//��������TCP
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

		//ѭ����ǰ����ִ�еļ�¼,��ΪĳЩ���������¼����delay���Ա���,����һ������ʱ��ִ��ʱ
		//���ж���һ���Ƿ�ʱ��ִ��
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
					//�������16������Ϣ
					//DEBUG_PRINT16(listWaitCmd->rLen,listWaitCmd->btCmd,"send->");

					ret=zhSockSend(pTcp->logic.s,(char*)listWaitCmd->btCmd,listWaitCmd->rLen);
					if(listWaitCmd->rLen==ret)
					{
						//TCP���ͳɹ�,ɾ������
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
							DEBUG_PRINTF("����Ŀ��Ͽ�����, Դ=%s, Ŀ������ַ=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
						}
						LOCK_CS(&pTcp->logic.csStatus);
						pTcp->logic.tStatus=eckProjTcpDisconnect;
						UNLOCK_CS(&pTcp->logic.csStatus);
					}
				}
				else
				{
					//TCP�Ѿ��Ͽ�,ɾ������
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
			//TCP��������,��ӵ��ڲ�����
			LOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
			DataCmdAdd(&g_websock.obj.collect.logic.lst_insideCmd,pTcp->obj.source,szTmpBuf,nBufSize,0);
			UNLOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
		}
		else if(SOCKET_ERROR==nBufSize)
		{
			if(eckProjTcpConnected==pTcp->logic.tStatus)
			{
				_sendTcpStatus_cb(pTcp->obj.source,false);
				DEBUG_PRINTF("����Ŀ��Ͽ�����, Դ=%s, Ŀ������ַ=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
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

	//�����д���
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

				//��������
				DEBUG_PRINTF("�����������Ӵ���Ŀ��, Դ=%s, Ŀ������ַ=%s:%d",pTcp->obj.source,pTcp->obj.host,pTcp->obj.port);
			}
		}
	}
}
