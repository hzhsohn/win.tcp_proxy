/*

����������

*/

#include "data_info.h"


//�����ļ�������---------------------------------------------
TckCfgPorxySocket* srcCfgAddTransSocket(TckTcpSocketCfg* cfg,
									int port,
									char* utf8_source);

TckSrcCfgTcp* srcCfgAddTcp(TckTcpSocketCfg* cfg,
									char* utf8_source,
									char* utf8_host,
									int port);
void srcCfgClearTcp(TckTcpSocketCfg* cfg);


//�����߼�������---------------------------------------------
TckSrcCfgTcp* TcpFindBySource(TzhList* lst_eleList, char*utf8_source);









