#ifndef _ZH_PROCESS_TCP_H_
#include <stdio.h>


#ifdef __cplusplus
extern "C"{
#endif

//����ת��ͨ��
void InitTransChannel(char* target_host,int target_port,int bind_prot);

//�½�һ��ת��ͨ��
void InitProcThread();

#ifdef __cplusplus
}
#endif
#define _ZH_PROCESS_TCP_H_
#endif
