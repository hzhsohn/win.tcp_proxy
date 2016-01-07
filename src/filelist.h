#ifndef _ZH_DATA_LIST_H_

#include "data_info.h"

#ifdef __cplusplus
extern "C"{
#endif

static TckSrcCfgFiles *FileListAdd(TzhList *pList,char*filename,char*md5,int file_size)
{
	TckSrcCfgFiles *pfile=NULL;

	pfile = (TckSrcCfgFiles *)calloc(1, sizeof(TckSrcCfgFiles));
	if(NULL==pfile)
    {
		//ERR_PRINTF("TckSrcCfgFiles: error to get memory\n");
        goto end;
	}
	if(!filename)
	{goto end;}
	
	strcpy(pfile->filename,filename);
	strcpy(pfile->md5,md5);
	pfile->nFileSize=file_size;

	zhListNodeAdd(pList,pfile,sizeof(TckSrcCfgFiles),eckEleFiles);
end:
	return pfile;
}

static TckSrcCfgFiles* FileListFindByFilename(TzhList *pList,char*filename)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;
        eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		TckSrcCfgFiles *pfile=(TckSrcCfgFiles *)eleNode->pElement;
		if(0==strcmpi(pfile->filename,filename))
		{			
			return pfile;
		}
	}
	return NULL;
}

static void FileListDeleteByFilename(TzhList *pList, char*filename)
{
	TckSrcCfgFiles *pFiles;
	pFiles=FileListFindByFilename(pList,filename);
	if(pFiles)
	{
		TzhListElementNode *pLstNode=zhListNodeFindByObject(pList,pFiles);
		free(pLstNode->pElement);
		zhListNodeDelete(pList,pLstNode);
	}
}

static void FileListDeleteAll(TzhList *pList)
{
	TzhListElementNode *eleNode;
	for(eleNode = (TzhListElementNode *)zhListFirst(pList);
        eleNode != NULL;
        eleNode = (TzhListElementNode *)zhListNext((TzhListNode *)eleNode))
	{
		free(eleNode->pElement);
		eleNode->pElement=NULL;
	}
	zhListNodeDeleteAll(pList);
}

#ifdef __cplusplus
}
#endif
#define _ZH_DATA_LIST_H_
#endif
