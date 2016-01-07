#include "ck_assist.h"

#ifdef _WIN32
#include "windows.h"
#endif


char* trLogAttribToStr(T_LOG_ATTRIB attrib,char *buf)
{
	switch(attrib)
	{
		case ZH_LOG_ATTRIB_ERROR:
			strcpy(buf,"Error");
		break;
		case ZH_LOG_ATTRIB_SYSTEM:
			strcpy(buf,"System");
		break;
		case ZH_LOG_ATTRIB_NORMAL:
			strcpy(buf,"Normal");
		break;
		default:
			strcpy(buf,"Unknow");
		break;
	}
	return buf;
}


char*getCurrentPath(char*filename,char*dst_buf,int dst_buf_len)
{
#ifdef _WIN32
	GetCurrentDirectory(dst_buf_len,dst_buf); 
	strcat(dst_buf,"\\");
#else
	//头文件：direct.h（TC2.0下为dir.h）
	getcwd(dst_buf, dst_buf_len);
	strcat(dst_buf,"/");
#endif
	if(filename)
	{ strcat(dst_buf,filename); }
	return dst_buf;
}

char*getWorkPath(char*work_path,char*directory,char*filename,char*dst_buf,int dst_buf_len)
{
	char buf[512];

	if(0==dst_buf_len){return NULL;}
	memset(dst_buf,0,dst_buf_len);

	if(NULL==work_path)
	{work_path="";}

	buf[0]=0;
#ifdef _WIN32
	if(strlen(work_path)>0)
	{
		sprintf(buf,"%s\\",work_path);
	}
	if(NULL!=directory && 0!=strlen(directory))
	{
		sprintf(buf,"%s%s\\",buf,directory);
	}
#else
	if(strlen(work_path)>0)
	{
		sprintf(buf,"%s/",work_path);
	}
	if(NULL!=directory && 0!=strlen(directory))
	{
		sprintf(buf,"%s%s/",buf,directory);
	}	
#endif
	if(NULL!=filename && 0!=strlen(filename))
	{
		sprintf(buf,"%s%s",buf,filename);
	}
	if(0==strcmp(".",work_path) || 0==strncmp("./",work_path,2) || 0==strncmp(".\\",work_path,2) ||
	0==strcmp("..",work_path) || 0==strncmp("../",work_path,3) || 0==strncmp("..\\",work_path,3) ||
	0==strlen(work_path))
	{
		getCurrentPath(buf,dst_buf,dst_buf_len);
	}
	else
	{
		strcpy(dst_buf,buf);
	}

	return dst_buf;
}


char trStrToBool(const char *szValue)
{
	if(!strcmpi(szValue,"0"))
	{	return 0;}
	else if(!strcmpi(szValue,"1"))
	{	return 1;}
	else if(!strcmpi(szValue,"false"))
	{	return 0;}
	else if(!strcmpi(szValue,"true"))
	{	return 1;}
	else
	{	return 0;}
}

char* trBoolToStr(char c)
{
	 return c?"true":"false";
}

//time_t 转 "年月日时分秒"
char* trLongTimeToStrNoSplit(time_t tme,char*dstString)
 {
	struct tm *ptm = NULL;
	ptm = localtime(&tme);
	if(ptm) 
	sprintf(dstString, "%4d%02d%02d%02d%02d%02d", 
			(ptm->tm_year + 1900),
			ptm->tm_mon+1, 
			ptm->tm_mday, 
			ptm->tm_hour, 
			ptm->tm_min,ptm->tm_sec);
	return dstString;
 }

//time_t 转 "年月日"
char* trLongTimeToStrDateNoSplit(time_t tme,char*dstString)
{
	struct tm *ptm = NULL;
	ptm = localtime(&tme);
	if(ptm) 
	sprintf(dstString, "%4d%02d%02d", 
			(ptm->tm_year + 1900),
			ptm->tm_mon+1, 
			ptm->tm_mday);
	return dstString;
}

//time_t 转 "时:分:秒"
char* trShortTimeToStr(time_t tme,char*dstString)
 {
	struct tm *ptm = NULL;
	ptm = localtime(&tme);
	if(ptm) 

	sprintf(dstString, "%02d:%02d:%02d",
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec);
	return dstString;
 }
