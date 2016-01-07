#ifndef __MICRO_H_
#ifdef __cplusplus
extern "C"{
#endif

#include "os_define.h"
#include "encoding.h"
#include "platform.h"

#ifdef Q_OS_WIN32
	#include <direct.h>
#else
	#include <sys/stat.h>
    #include <sys/types.h>
#endif

//编译类型语言转换设置
#define TO_UTF8(a,b,c)			Gb2312ToUtf8((const char*)a, (int)b,(char *)c)
#define FROM_UTF8(a,b,c)		Utf8ToGb2312((const char*)a, (int)b,(char *)c)

#ifndef _WIN32
#define strcmpi     strcasecmp
#else
#define strtok_r    strtok_s
#endif

#ifndef Q_OS_WIN32
#define strcmpi strcasecmp
#endif

//忽略错误
#ifdef Q_OS_WIN32
#pragma warning (disable:4819)
#endif

#ifdef Q_OS_WIN32
    #define MKDIR(path)					mkdir(path)
	#define RMDIR(path)					rmdir(path)
#endif

#ifdef Q_OS_MACX
    #define MKDIR(path)    [[NSFileManager defaultManager] createDirectoryAtPath: [NSString stringWithUTF8String:path] withIntermediateDirectories:YES attributes:nil error:nil]
	#define RMDIR(path)    [[NSFileManager defaultManager] removeItemAtPath:path error:nil]
#endif

#ifdef Q_OS_LINUX 
    #define MKDIR(path)					mkdir(path,S_IRWXU)
	#define RMDIR(path)					rmdir(path)
#endif


static void ERR_UTF8_PRINTF(int err_id,char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[1024];
	char buf2[1024];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,1024,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
	FROM_UTF8(buf,sizeof(buf),buf2);
   	printf("Err-%d: %s\r\n",err_id,buf2);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif
	va_end(args);
    //#endif
}

static void ERR_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
   	printf("Err-: %s\r\n",buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif
	va_end(args);
    //#endif
}

static void ALERT_UTF8_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[1024];
	char buf2[1024];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,1024,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
	FROM_UTF8(buf,sizeof(buf),buf2);
   	printf("Alert-: %s\r\n",buf2);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif

	va_end(args);
    //#endif
}

static void ALERT_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
   	printf("Alert-: %s\r\n",buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif

	va_end(args);
    //#endif
}

static void SYS_UTF8_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[1024];
	char buf2[1024];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,1024,format,args);
	FROM_UTF8(buf,sizeof(buf),buf2);
   	printf("System-: %s\r\n",buf2);

	va_end(args);
    //#endif
}

static void SYS_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
   	printf("System-: %s\r\n",buf);
	va_end(args);
    //#endif
}

		
static void DEBUG_PRINTF(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	char tme_buf[30];
	time_t time;		
	struct tm *ptm = NULL;

	//时间
	time=zhPlatGetSec();
	ptm = localtime(&time);
	if(ptm) 

	sprintf(tme_buf, "%02d:%02d:%02d",
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec);

	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
   	printf("[%s]: %s\r\n",tme_buf,buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif
	va_end(args);
    //#endif
}

static void DEBUG_ALERT(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	char tme_buf[30];
	time_t time;		
	struct tm *ptm = NULL;

	//时间
	time=zhPlatGetSec();
	ptm = localtime(&time);
	if(ptm) 

	sprintf(tme_buf, "%02d:%02d:%02d",
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec);

	//数据处理	
	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
   	printf("[%s]: %s\r\n",tme_buf,buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif
	va_end(args);
    //#endif
}

static void DEBUG_ERROR(char*format,...)
{
    //#ifndef Q_OS_MACX
	char buf[512];
	va_list args;
	char tme_buf[30];
	time_t time;		
	struct tm *ptm = NULL;

	//时间
	time=zhPlatGetSec();
	ptm = localtime(&time);
	if(ptm) 

	sprintf(tme_buf, "%02d:%02d:%02d",
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec);

	//数据处理	
	va_start(args, format);
	
	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_RED|FOREGROUND_INTENSITY);
#endif
   	printf("[%s]: %s\r\n",tme_buf,buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif
	va_end(args);
    //#endif
}

static void DEBUG_PRINT16(int len,unsigned char* data,char*format,...)
{
	//#ifndef Q_OS_MACX
	va_list args;
	char buf[512];
	int i;
	char data_buf[1024];
	char buf2[6];
	char tme_buf[30];
	time_t time;		
	struct tm *ptm = NULL;

	//时间
	time=zhPlatGetSec();
	ptm = localtime(&time);
	if(ptm) 

	sprintf(tme_buf, "%02d:%02d:%02d",
			ptm->tm_hour,
			ptm->tm_min,
			ptm->tm_sec);

	//数据处理						
	data_buf[0]=0;
	for(i=0;i<len;i++)
	{
	sprintf(buf2,"0x%02X",(unsigned char)data[i]);
	sprintf(data_buf,"%s %s",data_buf,buf2);
	}

	va_start(args, format);

	VSNPRINTF(buf,512,format,args);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_GREEN|FOREGROUND_INTENSITY);
#endif
   	printf("[%s]: %s%s\r\n",tme_buf,buf,data_buf);
#ifdef Q_OS_WIN32
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_BLUE |FOREGROUND_GREEN|FOREGROUND_RED );
#endif

	va_end(args);
    //#endif
}

#ifdef __cplusplus
}
#endif
#define __MICRO_H_
#endif
