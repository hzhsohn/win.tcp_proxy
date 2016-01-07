#include "cfg_operator.h"

TckCfgPorxySocket* srcCfgAddTransSocket(TckTcpSocketCfg* cfg,
									int port,
									char* utf8_source)
{
	TckCfgPorxySocket *p;

	p = (TckCfgPorxySocket *)calloc(1, sizeof(TckCfgPorxySocket));

	if(p == NULL) {
		ERR_PRINTF("srcCfgAddPorxySocket: error to get memory");
		return NULL;
	}

	p->obj.port=port;
	strcpy(p->obj.source,utf8_source);

	INIT_CS(&p->logic.csClient);

	zhListNodeAdd(&cfg->obj.PorxySocket.obj.lst_porxySocket,p,sizeof(TckCfgPorxySocket),eckEleOpen);
	return p;
}

TckSrcCfgTcp* srcCfgAddTcp(TckTcpSocketCfg* cfg,
									char* utf8_source,
									char* utf8_host,
									int port)
{
	TckSrcCfgTcp *pTcp;

	pTcp = (TckSrcCfgTcp *)calloc(1, sizeof(TckSrcCfgTcp));

	if(pTcp == NULL) {
		ERR_PRINTF("TcpAdd: error to get memory");
		return NULL;
	}

	strcpy(pTcp->obj.source,utf8_source);
	strcpy(pTcp->obj.host,utf8_host);
	pTcp->obj.port=port;

	INIT_CS(&pTcp->logic.csStatus);
	//初始化逻辑参数
	INIT_CS(&pTcp->logic.csWaitCmd);

	zhListNodeAdd(&cfg->obj.collect.obj.lst_eleTCP,pTcp,sizeof(TckSrcCfgTcp),eckEleTcp);
	return pTcp;
}

void srcCfgClearOpen(TckTcpSocketCfg* cfg)
{
	TzhListElementNode *eleNode;
	//
	for(eleNode = (TzhListElementNode *)zhListFirst(&cfg->obj.PorxySocket.obj.lst_porxySocket);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		TckCfgPorxySocket*p=(TckCfgPorxySocket*)eleNode->pElement;
		DELETE_CS(&p->logic.csClient);
	}

	zhListNodeReleaseAllElement(&cfg->obj.PorxySocket.obj.lst_porxySocket);
	zhListNodeDeleteAll(&cfg->obj.PorxySocket.obj.lst_porxySocket);
	
}

void srcCfgClearTcp(TckTcpSocketCfg* cfg)
{
	TzhListElementNode *eleNode;
	//
	for(eleNode = (TzhListElementNode *)zhListFirst(&cfg->obj.collect.obj.lst_eleTCP);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		TckSrcCfgTcp*p=(TckSrcCfgTcp*)eleNode->pElement;
		if(p)
		{
			DELETE_CS(&p->logic.csStatus);
			DELETE_CS(&p->logic.csWaitCmd);
		}
	}
}

TckSrcCfgTcp* TcpFindBySource(TzhList* lst_eleList, char*utf8_source)
{
	TzhListElementNode* eleNode;
	TckSrcCfgTcp *pNode;
	
	for(eleNode = (TzhListElementNode *)zhListFirst(lst_eleList);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		pNode=(TckSrcCfgTcp*)eleNode->pElement;
		if(0==strcmp(pNode->obj.source,utf8_source))
		{
			return pNode;
		}
	}
	return NULL;
}
