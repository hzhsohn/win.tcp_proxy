/*
  platform.h - The difference between the platform and the 
				corresponding processing platform system function
  2009/7/20
*/

/* 
  Copyright (C) 2008-2, Han.zhihong, Developer. Created 2008. 
  All rights reserved.

  License to copy and use this software is granted provided that it
  is identified as the "Han.zhihong. Message-Digest
  Algorithm" in all material mentioning or referencing this software
  or this function.
  
  Han.zhihong. makes no representations concerning either
  the merchantability of this software or the suitability of this
  software for any particular purpose. It is provided "as is"
  without express or implied warranty of any kind.
	  
  These notices must be retained in any copies of any part of this
  documentation and/or software.
*/

#include "platform.h"

#ifndef _WIN32
pthread_t _pthreadid;
#endif

unsigned long zhPlatGetTime()
{
#ifndef _WIN32
	/* linux */
	struct timeval tv;
    gettimeofday(&tv, NULL);
    return (tv.tv_sec*1000+tv.tv_usec/1000);
#else
	return GetTickCount();
#endif
}

unsigned long zhPlatGetSec()
{
	return (unsigned long)time(NULL);
}

void zhPlatSleep(int ms)
{
#ifdef _WIN32
	Sleep(ms);
#else 
	/* linux */
	usleep(ms*1000);
#endif
}

//////////////////////////////////////////////////////////
//debug function
void zhPlatCreateConsole(char*title)
{
#ifdef _WIN32
    HANDLE hStdOut;
	COORD co;
    AllocConsole();
    SetConsoleTitleA(title);
    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE); 
  	co.X=0;
	co.Y=0;
    SetConsoleScreenBufferSize(hStdOut, co); 
    freopen("CONOUT$","w+t",stdout);
    freopen("CONIN$","r+t",stdin);
#endif
}

void zhPlatFreeConsole()
{
#ifdef _WIN32
	FreeConsole();
#endif
}
//输出信息 
void zhPlatPrintf(char*format,...)
{
	char buf[256];
	va_list args;
	va_start(args, format);

	VSNPRINTF(buf,256,format,args);
   	printf("%s\r\n",buf);

	// write to file
	va_end(args);
}

void zhPlatPrint16(int len,char*buf)
{
	int i;
	for(i =0;i<len;i++)
	printf("%02X ",(BYTE)buf[i]);
	printf("\r\n");
}

char* zhPlatPrintf16ToBuf(int len,char *buf,char*dstString)
{
    char szbuf[10];
    short i=0;
    
    dstString[0]=0;
    for(i=0;i<len;i++)
    {
        sprintf(szbuf,"%02X",(unsigned char)buf[i]);
        if(strlen(dstString))
        sprintf(dstString,"%s %s",dstString,szbuf);
        else
        sprintf(dstString,"%s",szbuf);
    }
    return dstString;
}

///////////////////////////// 不区分大小写查找字符 ///////
char *strstri(const char *phaystack, const char *pneedle)
{
    register const unsigned char *haystack, *needle;
    register unsigned bl, bu, cl, cu;
    
    haystack = (const unsigned char *) phaystack;
    needle = (const unsigned char *) pneedle;
    
    bl = tolower(*needle);
    if (bl != '\0')
    {
        // Scan haystack until the first character of needle is found:
        bu = toupper(bl);
        haystack--;                             /* possible ANSI violation */
        do
        {
            cl = *++haystack;
            if (cl == '\0')
                goto ret0;
        }
        while ((cl != bl) && (cl != bu));
        
        // See if the rest of needle is a one-for-one match with this part of haystack:
        cl = tolower(*++needle);
        if (cl == '\0')  // Since needle consists of only one character, it is already a match as found above.
            goto foundneedle;
        cu = toupper(cl);
        ++needle;
        goto jin;
        
        for (;;)
        {
            register unsigned a;
            register const unsigned char *rhaystack, *rneedle;
            do
            {
                a = *++haystack;
                if (a == '\0')
                    goto ret0;
                if ((a == bl) || (a == bu))
                    break;
                a = *++haystack;
                if (a == '\0')
                    goto ret0;
            shloop:
                ;
            }
            while ((a != bl) && (a != bu));
            
        jin:
            a = *++haystack;
            if (a == '\0')  // Remaining part of haystack is shorter than needle.  No match.
                goto ret0;
            
            if ((a != cl) && (a != cu)) // This promising candidate is not a complete match.
                goto shloop;            // Start looking for another match on the first char of needle.
            
            rhaystack = haystack-- + 1;
            rneedle = needle;
            a = tolower(*rneedle);
            
            if (tolower(*rhaystack) == (int) a)
                do
                {
                    if (a == '\0')
                        goto foundneedle;
                    ++rhaystack;
                    a = tolower(*++needle);
                    if (tolower(*rhaystack) != (int) a)
                        break;
                    if (a == '\0')
                        goto foundneedle;
                    ++rhaystack;
                    a = tolower(*++needle);
                }
            while (tolower(*rhaystack) == (int) a);
            
            needle = rneedle;               /* took the register-poor approach */
            
            if (a == '\0')
                break;
        } // for(;;)
   
} // if (bl != '\0')
foundneedle:
    return (char*) haystack;
ret0:
    return 0;
}
