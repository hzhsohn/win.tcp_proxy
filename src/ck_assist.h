#ifndef _ZH_LIB_ASSIST2_FUNCTION_
#include <stdio.h>
#include "platform.h"
#include "micro.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _T_LOG_ATTRIB{
	ZH_LOG_ATTRIB_ERROR=0,						//错误
	ZH_LOG_ATTRIB_SYSTEM=1,						//系统
	ZH_LOG_ATTRIB_NORMAL=2						//普通消息
}T_LOG_ATTRIB;

//日志的转换
char* trLogAttribToStr(T_LOG_ATTRIB attrib,char *buf);

//获取当前的工作目录路径
char*getCurrentPath(char*filename,char*dst_buf,int dst_buf_len);
char*getWorkPath(char*work_path,char*directory,char*filename,char*dst_buf,int dst_buf_len);

char trStrToBool(const char *szValue);
char* trBoolToStr(char c);

char* trLongTimeToStrNoSplit(time_t tme,char*dstString);
char* trLongTimeToStrDateNoSplit(time_t tme,char*dstString);
char* trShortTimeToStr(time_t tme,char*dstString);

#ifdef __cplusplus
}
#endif
#define _ZH_LIB_ASSIST2_FUNCTION_
#endif
