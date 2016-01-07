#include "data_collect.h"
#include "cfg_operator.h"
#include "data_cmdlist.h"
#include "micro.h"

extern TckTcpSocketCfg g_websock;
extern TzhList g_userList;

//�����ռ��Ĵ�����
void collectProcess()
{
	TzhListElementNode* eleNode;
	TckCfgPorxySocket* pOpen;
	
	tcpProcess();

	//TCP�ͻ�����������ݴ���
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pOpen=(TckCfgPorxySocket*)eleNode->pElement;
		_collectSrcCfgOpenAccecpt(pOpen);//�����ݵĽ��봦��
		_collectNetworkRecv(pOpen);//�����ݵ��ռ�����
	}
	_insideProcess();
	_outsideProcess();
}

void _insideProcess()
{
	TzhListElementNode* eleDataNode;
	TzhListElementNode* eleNode;
	TckCfgPorxySocket* pOpen;
	DWORD dwCurTime;
	TzhListDataCmd* datCmd;

	//��serial��tcpԴ��������������ⲿTCP���ʹ���
	LOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
	for(eleDataNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.logic.lst_insideCmd);
	eleDataNode != NULL;)
	{
		datCmd=(TzhListDataCmd*)eleDataNode->pElement;

		dwCurTime=zhPlatGetTime();
		if(dwCurTime < datCmd->dwOperatorTime)
		{
			/*��������ʱ�仹û����������һ��ѭ��*/
			eleDataNode=(TzhListElementNode*)zhListNext((TzhListNode*)eleDataNode);
			continue;
		}


		//�������ݷ��ʹ���
		for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
		{
			pOpen=(TckCfgPorxySocket*)eleNode->pElement;
			if(0==strcmpi(pOpen->obj.source,datCmd->source))
			{
				_collectNetworkSend(pOpen,datCmd->btCmd,datCmd->rLen);
			}
		}
		
		free(eleDataNode->pElement);
		eleDataNode->pElement=NULL;
		eleDataNode = zhListNodeDelete(&g_websock.obj.collect.logic.lst_insideCmd,eleDataNode);
	}
	UNLOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
}

void _outsideProcess()
{
	TzhListElementNode* eleDataNode;
	DWORD dwCurTime;
	TzhListDataCmd* datCmd;
	TckSrcCfgTcp* tcp;

	//���ⲿ�����tcp�ͻ�����������������ڲ�����
	LOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
	for(eleDataNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.logic.lst_outsideCmd);
	eleDataNode != NULL;)
	{
		datCmd=(TzhListDataCmd*)eleDataNode->pElement;
		dwCurTime=zhPlatGetTime();
		if(dwCurTime < datCmd->dwOperatorTime)
		{
			/*��������ʱ�仹û����������һ��ѭ��*/
			eleDataNode=(TzhListElementNode*)zhListNext((TzhListNode*)eleDataNode);
			continue;
		}

	
		tcp=TcpFindBySource(&g_websock.obj.collect.obj.lst_eleTCP,
									datCmd->source);
		if(tcp)
		{
			LOCK_CS(&tcp->logic.csWaitCmd);
			DataCmdAdd(&tcp->logic.lst_waitCmd,datCmd->source,
					datCmd->btCmd,
					datCmd->rLen,
					datCmd->delay);
			UNLOCK_CS(&tcp->logic.csWaitCmd);
		}
		
		free(eleDataNode->pElement);
		eleDataNode->pElement=NULL;
		eleDataNode = zhListNodeDelete(&g_websock.obj.collect.logic.lst_outsideCmd,eleDataNode);
	}
	UNLOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
}

void openCollect()
{
	TzhListElementNode*eleNode;
	TckCfgPorxySocket* pOpen;
	
	//Ҫ�ȴ�����Դ
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pOpen=(TckCfgPorxySocket*)eleNode->pElement;
		if(!zhSockInit(&pOpen->logic.listen_sock,ZH_SOCK_TCP)){
			ALERT_UTF8_PRINTF("Init Socket Fail..!! data.port=%d",pOpen->obj.port);
			continue;}
		if(!zhSockSetNonBlocking(pOpen->logic.listen_sock,true)){
			ALERT_UTF8_PRINTF("SetNonBlocking Fail..!! data.port=%d",pOpen->obj.port);
			continue;}
		if(!zhSockSetReuseAddr(pOpen->logic.listen_sock,true)){
			ALERT_UTF8_PRINTF("Reuse Addr Fail..!! data.port=%d",pOpen->obj.port);
			continue;}
		if(!zhSockBindAddr(pOpen->logic.listen_sock,NULL,pOpen->obj.port)){
			ALERT_UTF8_PRINTF("Bind Fail..!! data.port=%d",pOpen->obj.port);
			continue;}
		if(!zhSockListen(pOpen->logic.listen_sock)){continue;}
		SYS_UTF8_PRINTF("proxy socket listen. port=%d,source=%s",pOpen->obj.port,pOpen->obj.source);
	}

	/*
		������TCPԴ,
		ע:	�����SOCKET�������̷߳�ʽ�򿪵�,
			��������ǰҪ�ȵ���zhSock_NetStartUp(int VersionHigh,int VersionLow),
			��Ȼ�����SOCKET��ʼ��ʧ�ܵ���ʾ,����Ҫ�ȳ�ʼ��g_websock.obj.open.obj.lst_websocket
			����������еĶ�����,�ٵ���openTcpCfg();
	*/
	openTcpCfg();
}

void closeCollect()
{
	TzhListElementNode* eleNode;
	TckCfgPorxySocket* pOpen;

	TzhListElementNode* eleNode2;
	TckSrcCfgTcpClient* pClient;

	//TCPԴ
	closeTcpCfg();

	//��OPEN����
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pOpen=(TckCfgPorxySocket*)eleNode->pElement;

		//��ռ������ӵĿͻ���
		LOCK_CS(&pOpen->logic.csClient);
		for(eleNode2 = (TzhListElementNode *)zhListFirst(&pOpen->logic.lst_client);
			eleNode2 != NULL;)
		{
			pClient=(TckSrcCfgTcpClient*)eleNode2->pElement;
			zhSockClose(pClient->sockClient);

			free(eleNode->pElement);
			eleNode->pElement=NULL; 
			eleNode2 = zhListNodeDelete(&pOpen->logic.lst_client,eleNode2);
		}
		UNLOCK_CS(&pOpen->logic.csClient);
	}
}

/*
	�������տͻ���SOCKET����
	������collect��TCP��������
*/
void _collectSrcCfgOpenAccecpt(TckCfgPorxySocket* pOpen)
{
	SOCKET tmp;

	tmp=zhSockAccept(pOpen->logic.listen_sock);
	if(tmp)
	{
		char ip[45];
		WORD port;
		TckSrcCfgTcpClient*dsc;

		zhSockGetRemoteAddr(tmp,ip,&port,NULL);

		//�½�һ���ѽṹ���浽��ǰ�ͻ���������
		dsc=(TckSrcCfgTcpClient*)malloc(sizeof(TckSrcCfgTcpClient));
		memset(dsc,0,sizeof(TckSrcCfgTcpClient));
				
		dsc->sockClient=tmp;
		dsc->dwConnectedTime=zhPlatGetTime();
		strcpy(dsc->ip,ip);
		dsc->port=port;

		zhSockSetSendBufferSize(dsc->sockClient,102400);//���÷��ͻ�����
		zhSockSetRecvBufferSize(dsc->sockClient,102400);//���ý��ջ�����

		zhListNodeAdd(	&pOpen->logic.lst_client,
						dsc,
						sizeof(TckSrcCfgTcpClient),
						eckEleSourceTcpClient);

		DEBUG_ALERT("�¿ͻ��˽���! ����˿�=%d Դ=%s ���ӱ�ʶ=%d,�ͻ��˵�ַ=%s:%d",pOpen->obj.port,pOpen->obj.source,dsc->sockClient,ip,port);
	}
}

void _collectNetworkRecv(TckCfgPorxySocket* pOpen)
{
	TzhListElementNode* eleNode;
	TckSrcCfgTcpClient* pTcpClient;

	unsigned char szBuf[1024];
	int nRet;

	nRet=0;
	for(eleNode = (TzhListElementNode *)zhListFirst(&pOpen->logic.lst_client);
		eleNode != NULL;)
	{
		pTcpClient=(TckSrcCfgTcpClient*)eleNode->pElement;
		//����ͻ��˽��մ���
		//�����յ��������Ǹ������ӿڻ�ת��
		nRet=zhSockRecv(pTcpClient->sockClient,(char*)szBuf,sizeof(szBuf));
		if(nRet>0)
		{
				/////////////////////////////////////////////////////
				//��־
				TckSrcCfgTcp* tcp=NULL;				
				tcp=TcpFindBySource(&g_websock.obj.collect.obj.lst_eleTCP, pOpen->obj.source);
				if(tcp)
				{
					int i;
					char buf[10240]={0};
					char buf2[6]={0};
	
					for(i=0;i<nRet;i++)
					{
						sprintf(buf2,"0x%02X",(unsigned char)szBuf[i]);
						sprintf(buf,"%s_%s",buf,buf2);
					}

					if(eckProjTcpConnected==tcp->logic.tStatus)
					{
						DEBUG_ALERT("%s:%d->%s:%d(%s) %d�ֽ� %s",
										pTcpClient->ip,pTcpClient->port,								
										tcp->obj.host,tcp->obj.port,pOpen->obj.source,
										nRet,buf);
					}
					else
					{
						DEBUG_ERROR("%s:%d->%s:%d(%s) ����ʧ��,����Ŀ��δ����",
										pTcpClient->ip,pTcpClient->port,								
										tcp->obj.host,tcp->obj.port,pOpen->obj.source);
					}
					
				
					//�����������
					LOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
					DataCmdAdd(&g_websock.obj.collect.logic.lst_outsideCmd,pOpen->obj.source,szBuf,nRet,0);
					UNLOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
				}
		}
		//�ж��Ƿ�Ͽ�����
		if(nRet==SOCKET_ERROR)
		{
			DEBUG_ALERT("datasource tcp disconnect %s:%d, socket=%d",
						pTcpClient->ip,pTcpClient->port,
						pTcpClient->sockClient);
			free(eleNode->pElement);
			eleNode->pElement=NULL;
			eleNode = zhListNodeDelete(&pOpen->logic.lst_client,eleNode);
		}
		else
		{
			eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode);
		}
	}
}

void _collectNetworkSend(TckCfgPorxySocket* pOpen,unsigned char*buf,int len)
{
	TzhListElementNode* eleNode;
	TckSrcCfgTcpClient* pTcpClient;
	int nRet;

	///////////////////////////////////////
	//��ʾ����
	int i;
	char chr_buf[1024];
	char chr_buf2[6];
							
	chr_buf[0]=0;
	for(i=0;i<len;i++)
	{
		sprintf(chr_buf2,"0x%02X",(unsigned char)buf[i]);
		sprintf(chr_buf,"%s_%s",chr_buf,chr_buf2);
	}

	////////////////////////////
	nRet=0;
	LOCK_CS(&pOpen->logic.csClient);

	if(0==zhListCount(&pOpen->logic.lst_client))
	{
		DEBUG_PRINTF("%s:%d->NONE %d�ֽ� %s",pOpen->obj.source,pOpen->obj.port,
											len,chr_buf);

	}

	for(eleNode = (TzhListElementNode *)zhListFirst(&pOpen->logic.lst_client);
		eleNode != NULL;)
	{		
		TckSrcCfgTcp* tcp=NULL;		

		pTcpClient=(TckSrcCfgTcpClient*)eleNode->pElement;
		zhSockSend(pTcpClient->sockClient,(char*)buf,len);
		
		tcp=TcpFindBySource(&g_websock.obj.collect.obj.lst_eleTCP, pOpen->obj.source);
		if(tcp)
		{
		//��ӡ���з��͵��Ŀͻ���
		DEBUG_PRINTF("%s:%d(%s)->%s:%d %d�ֽ� %s",
											tcp->obj.host,tcp->obj.port,pOpen->obj.source,
											pTcpClient->ip,pTcpClient->port,
											len,chr_buf);
		}

		//�ж��Ƿ�Ͽ�����
		if(nRet==SOCKET_ERROR)
		{
			DEBUG_PRINTF("datasource tcp disconnect port=%d,socket=%d",pOpen->obj.port,pTcpClient->sockClient);
			free(eleNode->pElement); 
			eleNode->pElement=NULL;
			eleNode = zhListNodeDelete(&pOpen->logic.lst_client,eleNode);
		}
		else
		{
			eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode);
		}
	}
	UNLOCK_CS(&pOpen->logic.csClient);
}



