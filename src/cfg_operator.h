/*

辅助处理函数

*/

#include "data_info.h"


//配置文件处理函数---------------------------------------------
TckCfgPorxySocket* srcCfgAddTransSocket(TckTcpSocketCfg* cfg,
									int port,
									char* utf8_source);

TckSrcCfgTcp* srcCfgAddTcp(TckTcpSocketCfg* cfg,
									char* utf8_source,
									char* utf8_host,
									int port);
void srcCfgClearTcp(TckTcpSocketCfg* cfg);


//常用逻辑处理函数---------------------------------------------
TckSrcCfgTcp* TcpFindBySource(TzhList* lst_eleList, char*utf8_source);









