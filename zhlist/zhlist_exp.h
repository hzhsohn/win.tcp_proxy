#ifndef _ZH_D_LIST_NODE_MUD_EXP_H__
#define _ZH_D_LIST_NODE_MUD_EXP_H__

#include <memory.h>
#include <stdio.h>
#include "zhlist.h"


#ifdef __cplusplus
extern "C"{
#endif

#ifndef __cplusplus
#ifndef bool
typedef unsigned char bool;
#define true	1
#define false	0
#endif
#endif

//�����Ԫ��ָ��
typedef struct _TzhElementNode{
    TzhListNode node;
    void *pElement;
	int nElementSize;
    int nEleType;
}TzhListElementNode;

void zhListNodeInit(TzhListElementNode* eleNode);
TzhListElementNode * zhListNodeFind(TzhList* pList,void *element);
TzhListElementNode * zhListNodeFindByObject(TzhList* pList,void *pElement);

TzhListElementNode *zhListNodeFindByIndex(TzhList *pList, int index);
TzhListElementNode * zhListNodeAdd(TzhList *pList,void *pElement,int nElementSize,int elementType);

//ɾ���ɹ����ض���Ԫ��
TzhListElementNode * zhListNodePopFirst(TzhList *pListNodeList);

//ɾ���ɹ��������һ��Ԫ��
TzhListElementNode * zhListNodePopBack(TzhList *pListNodeList);

//ɾ��ָ���ڵ�
TzhListElementNode* zhListNodeDelete(TzhList* pListNodeList,TzhListElementNode* node);

//�ͷ���������������
void zhListNodeReleaseAllElement(TzhList* pList);

//ɾ������,�����ع���ӳ���������һ������ڵ������ж���
bool zhListNodeDeleteAll(TzhList *pList);

#ifdef __cplusplus
}
#endif
#endif

