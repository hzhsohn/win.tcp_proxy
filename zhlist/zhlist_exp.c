#include "zhlist_exp.h"


void zhListNodeInit(TzhListElementNode* eleNode)
{
	memset(eleNode,0,sizeof(TzhListElementNode));
}

TzhListElementNode * zhListNodeFind(TzhList* pList,void *element)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;
        eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		if(eleNode==element)
		{
			return eleNode;
		}
	}
	return NULL;
}

TzhListElementNode * zhListNodeFindByObject(TzhList* pList,void *pElement)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;
        eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		if(eleNode->pElement==pElement)
		{
			return eleNode;
		}
	}
	return NULL;
}

TzhListElementNode *zhListNodeFindByIndex(TzhList *pList, int index)
{
	int i;
    TzhListElementNode *eleNode;
    eleNode=NULL;
	
    if (index>=0 && index<zhListCount(pList)) {
        eleNode=(TzhListElementNode *)zhListFirst(pList);
        for(i=0;i<index;i++)
        {
            eleNode=(TzhListElementNode *)zhListNext((TzhListNode*)eleNode);
        }
    }
    return eleNode;
}

TzhListElementNode * zhListNodeAdd(TzhList *pList,void *pElement,int nElementSize,int elementType)
{
	TzhListElementNode*pListNode;

	pListNode = (TzhListElementNode *)calloc(1, sizeof(TzhListElementNode));

	if(pListNode == NULL) {
		printf("TzhElementNodeList: error to get memory");
		return NULL;
	}

	zhListNodeInit(pListNode);

	pListNode->pElement=pElement;
	pListNode->nElementSize=nElementSize;
	pListNode->nEleType=elementType;

	zhListAdd(pList, (TzhListNode *)pListNode);
	return pListNode;
}

//删除成功返回顶部元素
TzhListElementNode * zhListNodePopFirst(TzhList *pListNodeList)
{
	TzhListElementNode*pFormListNode,*pNew;

	pFormListNode =(TzhListElementNode*)zhListFirst(pListNodeList);

	if(pFormListNode == NULL)
	{
		printf("TzhElementNodeList: no pNode element");
		return NULL;
	}
	pNew=(TzhListElementNode *)zhListNext((TzhListNode *)pFormListNode);
	zhListDelete(pListNodeList, (TzhListNode *)pFormListNode);
	free(pFormListNode);
	return pNew;
}

//删除成功返回最后一个元素
TzhListElementNode * zhListNodePopBack(TzhList *pListNodeList)
{
	TzhListElementNode*pFormListNode,*pNew;

	pFormListNode =(TzhListElementNode*)zhListLast(pListNodeList);

	if(pFormListNode == NULL)
	{
		printf("TzhElementNodeList: no pNode last element");
		return NULL;
	}
	pNew=(TzhListElementNode *)zhListPrev((TzhListNode *)pFormListNode);
	zhListDelete(pListNodeList, (TzhListNode *)pFormListNode);
	free(pFormListNode);
	return pNew;
}

//删除指定节点
TzhListElementNode* zhListNodeDelete(TzhList* pListNodeList,TzhListElementNode* node)
{
	TzhListElementNode *pNew=NULL;
	if(node)
	{
		pNew=(TzhListElementNode *)zhListNext((TzhListNode *)node);
		zhListDelete(pListNodeList, (TzhListNode *)node);
		free(node);	
		return pNew;
	}
	return NULL;
}

//释放链表内所有内容
void zhListNodeReleaseAllElement(TzhList* pList)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
		eleNode != NULL;
		eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		if(eleNode->pElement)
		{
			free(eleNode->pElement);
			eleNode->pElement=NULL;
			eleNode->nElementSize=0;
			eleNode->nEleType=0;
		}
	}
}

//删除链表,并返回工作映射链表的下一个链表节点内所有对象
bool zhListNodeDeleteAll(TzhList *pList)
{
	TzhListElementNode *pListNode;
	if(pList)
	{
		for(pListNode = (TzhListElementNode *)zhListFirst(pList); pListNode != NULL;)
		{
			pListNode=zhListNodeDelete(pList,pListNode);
		}
		return true;
	}
	else
		return false;
}

