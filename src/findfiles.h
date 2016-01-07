/* ///////////////////////////sample//////////////////////////////////
void findCallBack(char*path,char*filename,EzhFileAttrib attrib,void*p)
{
     printf("path=%s\tname=%s\tattrib=%d\tp=%p\n",path,filename,attrib,p);
}

typedef struct _TagInfo
{
	int a,b,c;
}TagInfo;

int mian()
{
	TagInfo info;
	info.a=info.b=info.c=10;
	zhFindFile(findCallBack,"./",1,&info);
	return 0;
}
*////////////////////////////////////////////////////////////////////

#pragma once
#ifndef _ZH_SEACH_FILES_H__


#ifdef __cplusplus
extern "C"{
#endif
    
typedef enum _EzhFileAttrib{
    ezhFileAttribUnknow,
    ezhFileAttribFile,
    ezhFileAttribDir
}EzhFileAttrib;
typedef void(FINDFILE_CALLBACK)(char*path,char*filename,EzhFileAttrib attrib,void*p);

    
#ifdef _WIN32
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include <windows.h>
    static EzhFileAttrib returnFileAttrib(int attrib)
    {
        switch(attrib)
        {
            case _A_ARCH:            
            case _A_ARCH|_A_RDONLY:            
            case _A_ARCH|_A_HIDDEN:
            case _A_ARCH|_A_SYSTEM:            
            case _A_ARCH|_A_SYSTEM|_A_RDONLY:            
            case _A_ARCH|_A_SYSTEM|_A_HIDDEN:            
            case _A_ARCH|_A_HIDDEN|_A_RDONLY:            
            case _A_ARCH|_A_HIDDEN|_A_RDONLY|_A_SYSTEM:            
                return ezhFileAttribFile;
                
            case _A_SUBDIR:
            case _A_SUBDIR|_A_RDONLY:   
            case _A_SUBDIR|_A_HIDDEN:
            case _A_SUBDIR|_A_SYSTEM:
            case _A_SUBDIR|_A_SYSTEM|_A_RDONLY:
            case _A_SUBDIR|_A_SYSTEM|_A_HIDDEN:
            case _A_SUBDIR|_A_HIDDEN|_A_RDONLY:
            case _A_SUBDIR|_A_HIDDEN|_A_RDONLY|_A_SYSTEM:
                return ezhFileAttribDir;           
        }
        return ezhFileAttribUnknow;
    }
    
    static int zhFindFile(FINDFILE_CALLBACK*pfCallBack,char*path,int recursive,void*p)
    {
        char tmpPath[256];
        struct _finddata_t file; 
        long h_handle; 
        
        //strset(tmpPath,0);
        sprintf(tmpPath,"%s\\*.*",path);
        if ( (h_handle=(long)_findfirst( tmpPath,&file))==-1) 
            return 1;
        
        while ( -1!=_findnext( h_handle, &file )) 
        { 
            if(!strcmp(file.name,"..") || !strcmp(file.name,"."))continue;

            if(returnFileAttrib(file.attrib)==ezhFileAttribDir && recursive)
            {
                strset(tmpPath,0);
                sprintf(tmpPath,"%s\\%s",path,file.name);
                zhFindFile(pfCallBack,tmpPath,recursive,p);
            }
			pfCallBack(path,file.name,returnFileAttrib(file.attrib),p);
        } 
        _findclose(h_handle); 
        return 0;
    }   
    
#else
#include <stdio.h>
#include <stdlib.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string.h>
    static EzhFileAttrib returnFileAttrib(int attrib)
    {
        switch(attrib)
        {
            case  DT_REG:            
                return ezhFileAttribFile;
            case  DT_DIR:
                return ezhFileAttribDir;               
        }
        return ezhFileAttribUnknow;
    }
    static int IS_DIR(const char* path)
    {
        struct stat st;
        lstat(path, &st);
        return S_ISDIR(st.st_mode);
    }
 
    static void List_Files_Core(FINDFILE_CALLBACK*pfCallBack,char *path, int recursive,void*p)
    {
        DIR *pdir;
        struct dirent *pdirent;
        char temp[256];
        pdir = opendir(path);
        if(pdir)
        {
            while((pdirent = readdir(pdir)))
            {
                if(strcmp(pdirent->d_name, ".") == 0
                   || strcmp(pdirent->d_name, "..") == 0)
                    continue;
                sprintf(temp, "%s/%s", path, pdirent->d_name);
                if(IS_DIR(temp) && recursive)
                {
                    List_Files_Core(pfCallBack,temp, recursive,p);
                }
				pfCallBack(path,pdirent->d_name,returnFileAttrib(pdirent->d_type),p);
            }
        }
        /* else
         {
         printf("opendir error:%s\n", path);
         }*/
        closedir(pdir);
        
    }
    static int zhFindFile(FINDFILE_CALLBACK*pfCallBack,const char *path, int recursive,void*p)
    {
        int len;
        char temp[256];
        int i=0;
        
        len = strlen(path);
        strcpy(temp, path);
        for(i=0;i<len;i++)
        {
            if(temp[i]=='\\')
            {temp[i]='/';}
        }
        if(temp[len - 1] == '/'){temp[len -1] = '\0';}
        if(IS_DIR(temp))
        {
            List_Files_Core(pfCallBack,temp, recursive,p);
        }
        else
        {
            //printf("%s\n", path);
            return 1;
        }
        return 0;
    }
#endif

    
#ifdef __cplusplus
}
#endif

#define _ZH_SEACH_FILES_H__
#endif

