#pragma once

#ifndef _TCP_DATA_SOURCE_LOGIC_H_

#include "data_info.h"

#ifdef __cplusplus
extern "C"{
#endif

//串口处理的函数
void tcpProcess();
//打开所有TCP连接
void openTcpCfg();
//关闭所有TCP连接
void closeTcpCfg();

void _sendTcpStatus_cb(char*source,bool enable);
void _tcpCfgConnecting(TckSrcCfgTcp* pTcp);
void _writeTcpLogic();
void _readTcpLogic();
void _checkTcpLogic();



#ifdef __cplusplus
}
#endif
#define _TCP_DATA_SOURCE_LOGIC_H_
#endif 
