#ifndef _ZH_PROCESS_TCP_H_
#include <stdio.h>


#ifdef __cplusplus
extern "C"{
#endif

//建立转换通道
void InitTransChannel(char* target_host,int target_port,int bind_prot);

//新建一个转换通道
void InitProcThread();

#ifdef __cplusplus
}
#endif
#define _ZH_PROCESS_TCP_H_
#endif
