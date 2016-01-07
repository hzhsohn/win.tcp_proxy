#ifndef CK_WEBSOCKET_CK_DEFINE_H__
#define CK_WEBSOCKET_CK_DEFINE_H__

//Դ�������ļ���
#define ZH_PROXY_NET_XML			"tcp_data_proxy.config"

//��չ��
#define ZH_PROXY_NET_LOG_EXT			"zh_proxy_log"		//��־�ļ�

//�������״̬��ʱ����
#define ZH_CHECK_TCP_DATA_INTERVAL			3000

//���������ݳ���
#define	ZH_DATA_SOURCE_SIZE					26
#define ZH_DATA_FILENAME_SIZE				64
#define ZH_DATA_SOURCE_CMD_SIZE				512
#define ZH_DATA_MD5_SIZE					33
#define ZH_DATA_PORT_SIZE					26
#define ZH_DATA_NAME_SIZE					26
#define ZH_DATA_IP_SIZE						44
#define ZH_DATA_HOST_SIZE					128
#define ZH_DATA_SOURCE_RECV_SIZE			256


//���������
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







