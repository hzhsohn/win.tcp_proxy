#ifndef CK_WEBSOCKET_CK_DEFINE_H__
#define CK_WEBSOCKET_CK_DEFINE_H__

//源的配置文件名
#define ZH_PROXY_NET_XML			"tcp_data_proxy.config"

//扩展名
#define ZH_PROXY_NET_LOG_EXT			"zh_proxy_log"		//日志文件

//检测连网状态的时间间隔
#define ZH_CHECK_TCP_DATA_INTERVAL			3000

//变量的数据长度
#define	ZH_DATA_SOURCE_SIZE					26
#define ZH_DATA_FILENAME_SIZE				64
#define ZH_DATA_SOURCE_CMD_SIZE				512
#define ZH_DATA_MD5_SIZE					33
#define ZH_DATA_PORT_SIZE					26
#define ZH_DATA_NAME_SIZE					26
#define ZH_DATA_IP_SIZE						44
#define ZH_DATA_HOST_SIZE					128
#define ZH_DATA_SOURCE_RECV_SIZE			256


//链表的类型
typedef enum _EckCfgNodeType{
	eckEleDefault,
	eckEleSerial,
	eckEleTcp,
	eckEleService,
	eckEleServiceAllowIP,
	eckEleServiceStartupSource,
	eckEleOpen,
	eckEleFiles,
	eckEleLog,
	eckEleSourceCmd,
	eckEleSourceTcpClient
}EckCfgNodeType;


#endif







