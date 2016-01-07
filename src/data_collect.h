#pragma once
#include "tcp_logic.h"
#include "data_info.h"

//数据收集的处理函数
void collectProcess();
void openCollect();
void closeCollect();

void _insideProcess();
void _outsideProcess();
void _collectSrcCfgOpenAccecpt(TckCfgPorxySocket* pOpen);
void _collectNetworkRecv(TckCfgPorxySocket* pOpen);
void _collectNetworkSend(TckCfgPorxySocket* pOpen,unsigned char*buf,int len);
