#include "data_collect.h"
#include "cfg_operator.h"
#include "data_cmdlist.h"
#include "micro.h"

extern TckTcpSocketCfg g_websock;
extern TzhList g_userList;

//数据收集的处理函数
void collectProcess()
{
	TzhListElementNode* eleNode;
	TckCfgPorxySocket* pOpen;
	
	tcpProcess();

	//TCP客户端链表的数据处理
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pOpen=(TckCfgPorxySocket*)eleNode->pElement;
		_collectSrcCfgOpenAccecpt(pOpen);//对数据的接入处理
		_collectNetworkRecv(pOpen);//对数据的收集处理
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

	//对serial和tcp源内数据链表进行外部TCP发送处理
	LOCK_CS(&g_websock.obj.collect.logic.csInsideCmd);
	for(eleDataNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.logic.lst_insideCmd);
	eleDataNode != NULL;)
	{
		datCmd=(TzhListDataCmd*)eleDataNode->pElement;

		dwCurTime=zhPlatGetTime();
		if(dwCurTime < datCmd->dwOperatorTime)
		{
			/*如果命令的时间还没到就跳进另一个循环*/
			eleDataNode=(TzhListElementNode*)zhListNext((TzhListNode*)eleDataNode);
			continue;
		}


		//网络数据发送处理
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

	//对外部服务和tcp客户端外数据链表进行内部处理
	LOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
	for(eleDataNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.collect.logic.lst_outsideCmd);
	eleDataNode != NULL;)
	{
		datCmd=(TzhListDataCmd*)eleDataNode->pElement;
		dwCurTime=zhPlatGetTime();
		if(dwCurTime < datCmd->dwOperatorTime)
		{
			/*如果命令的时间还没到就跳进另一个循环*/
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
	
	//要先打开数据源
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
		打开所有TCP源,
		注:	这里的SOCKET连接是线程方式打开的,
			所以启用前要先调用zhSock_NetStartUp(int VersionHigh,int VersionLow),
			不然会出现SOCKET初始化失败的提示,所以要先初始化g_websock.obj.open.obj.lst_websocket
			这变量里所有的对象先,再调用openTcpCfg();
	*/
	openTcpCfg();
}

void closeCollect()
{
	TzhListElementNode* eleNode;
	TckCfgPorxySocket* pOpen;

	TzhListElementNode* eleNode2;
	TckSrcCfgTcpClient* pClient;

	//TCP源
	closeTcpCfg();

	//打开OPEN服务
	for(eleNode = (TzhListElementNode *)zhListFirst(&g_websock.obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pOpen=(TckCfgPorxySocket*)eleNode->pElement;

		//清空监听连接的客户端
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
	监听接收客户端SOCKET连接
	这里是collect的TCP监听功能
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

		//新建一个堆结构保存到当前客户端链表里
		dsc=(TckSrcCfgTcpClient*)malloc(sizeof(TckSrcCfgTcpClient));
		memset(dsc,0,sizeof(TckSrcCfgTcpClient));
				
		dsc->sockClient=tmp;
		dsc->dwConnectedTime=zhPlatGetTime();
		strcpy(dsc->ip,ip);
		dsc->port=port;

		zhSockSetSendBufferSize(dsc->sockClient,102400);//设置发送缓冲区
		zhSockSetRecvBufferSize(dsc->sockClient,102400);//设置接收缓冲区

		zhListNodeAdd(	&pOpen->logic.lst_client,
						dsc,
						sizeof(TckSrcCfgTcpClient),
						eckEleSourceTcpClient);

		DEBUG_ALERT("新客户端接入! 代理端口=%d 源=%s 连接标识=%d,客户端地址=%s:%d",pOpen->obj.port,pOpen->obj.source,dsc->sockClient,ip,port);
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
		//处理客户端接收处理
		//将接收到的数据是跟其它接口互转的
		nRet=zhSockRecv(pTcpClient->sockClient,(char*)szBuf,sizeof(szBuf));
		if(nRet>0)
		{
				/////////////////////////////////////////////////////
				//日志
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
						DEBUG_ALERT("%s:%d->%s:%d(%s) %d字节 %s",
										pTcpClient->ip,pTcpClient->port,								
										tcp->obj.host,tcp->obj.port,pOpen->obj.source,
										nRet,buf);
					}
					else
					{
						DEBUG_ERROR("%s:%d->%s:%d(%s) 发送失败,代理目标未连接",
										pTcpClient->ip,pTcpClient->port,								
										tcp->obj.host,tcp->obj.port,pOpen->obj.source);
					}
					
				
					//添入命令到链表
					LOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
					DataCmdAdd(&g_websock.obj.collect.logic.lst_outsideCmd,pOpen->obj.source,szBuf,nRet,0);
					UNLOCK_CS(&g_websock.obj.collect.logic.csOutsideCmd);
				}
		}
		//判断是否断开连接
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
	//显示数据
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
		DEBUG_PRINTF("%s:%d->NONE %d字节 %s",pOpen->obj.source,pOpen->obj.port,
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
		//打印所有发送到的客户端
		DEBUG_PRINTF("%s:%d(%s)->%s:%d %d字节 %s",
											tcp->obj.host,tcp->obj.port,pOpen->obj.source,
											pTcpClient->ip,pTcpClient->port,
											len,chr_buf);
		}

		//判断是否断开连接
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



