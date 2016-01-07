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

//ɾ���ɹ����ض���Ԫ��
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

//ɾ���ɹ��������һ��Ԫ��
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

//ɾ��ָ���ڵ�
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

//�ͷ���������������
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

//ɾ������,�����ع���ӳ���������һ������ڵ������ж���
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

