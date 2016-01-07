#ifndef _ZH_LIB_ASSIST2_FUNCTION_
#include <stdio.h>
#include "platform.h"
#include "micro.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef enum _T_LOG_ATTRIB{
	ZH_LOG_ATTRIB_ERROR=0,						//����
	ZH_LOG_ATTRIB_SYSTEM=1,						//ϵͳ
	ZH_LOG_ATTRIB_NORMAL=2						//��ͨ��Ϣ
}T_LOG_ATTRIB;

//��־��ת��
char* trLogAttribToStr(T_LOG_ATTRIB attrib,char *buf);

//��ȡ��ǰ�Ĺ���Ŀ¼·��
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
