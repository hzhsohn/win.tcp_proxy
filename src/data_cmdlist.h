#ifndef _ZH_SERIAL_CMD_LIST_H_

#include "data_info.h"

#ifdef __cplusplus
extern "C"{
#endif

static bool DataCmdAdd(TzhList *pList,char*source,unsigned char*pcmd,short len,int delay)
{
	TzhListDataCmd *pAllowSerivceName=NULL;

	pAllowSerivceName = (TzhListDataCmd *)calloc(1, sizeof(TzhListDataCmd));
	if(NULL==pAllowSerivceName)
    {
        return false;
	}

	strcpy(pAllowSerivceName->source,source);
	memcpy(pAllowSerivceName->btCmd,pcmd,len);
	pAllowSerivceName->rLen=len;
	pAllowSerivceName->dwOperatorTime=zhPlatGetTime();
	pAllowSerivceName->dwOperatorTime+=(DWORD)delay;

	zhListNodeAdd(pList,pAllowSerivceName,sizeof(TzhListDataCmd), eckEleSourceCmd);
	return true;
}

static TzhListElementNode *DataCmdDeleteFrist(TzhList *pList)
{
	TzhListElementNode *pNode;
	pNode=(TzhListElementNode*)zhListFirst(pList);
	free(pNode->pElement);
	pNode=zhListNodeDelete(pList,pNode);
	return pNode;
} 

static TzhListElementNode *DataCmdDeleteElementNode(TzhList *pList, TzhListElementNode *pNode)
{
	TzhListElementNode*p;
	free(pNode->pElement);
	p=zhListNodeDelete(pList,pNode);
	return p;
}

static void DataCmdDeleteAllBySource(TzhList *pList,char*source)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;)
	{
		TzhListDataCmd*pCmd=(TzhListDataCmd*)eleNode->pElement;
		if(0==strcmpi(pCmd->source,source))
		{
			free(eleNode->pElement);
			eleNode->pElement=NULL;
			//删除链表节点
			eleNode=zhListNodeDelete(pList,eleNode);
		}
		else
		{
			 eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode);
		}
	}
}

static void DataCmdDeleteAll(TzhList *pList)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;)
	{
		free(eleNode->pElement);
		eleNode->pElement=NULL;
		//删除链表节点
		eleNode=zhListNodeDelete(pList,eleNode);
	}
}

#ifdef __cplusplus
}
#endif
#define _ZH_SERIAL_CMD_LIST_H_
#endif
