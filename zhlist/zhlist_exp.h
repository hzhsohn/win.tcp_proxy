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

//链表的元素指针
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

//删除成功返回顶部元素
TzhListElementNode * zhListNodePopFirst(TzhList *pListNodeList);

//删除成功返回最后一个元素
TzhListElementNode * zhListNodePopBack(TzhList *pListNodeList);

//删除指定节点
TzhListElementNode* zhListNodeDelete(TzhList* pListNodeList,TzhListElementNode* node);

//释放链表内所有内容
void zhListNodeReleaseAllElement(TzhList* pList);

//删除链表,并返回工作映射链表的下一个链表节点内所有对象
bool zhListNodeDeleteAll(TzhList *pList);

#ifdef __cplusplus
}
#endif
#endif

