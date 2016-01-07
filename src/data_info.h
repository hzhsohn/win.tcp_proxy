#ifndef _ZH_USERINFO_H_

#include "platform.h"
#include "zhlist_exp.h"
#include "socket.h"
#include "ck_define.h"
#include "micro.h"

#ifdef __cplusplus
extern "C"{
#endif

//////////////////////////////////////////
//串口和TCP通讯方向
typedef enum _EckCollectDirect
{
	eckDirectUnknow=0,
	eckDirectInside=1,
	eckDirectOutside =2
}EckCollectDirect;

//链表命令的结构
typedef struct _TzhListDataCmd
{
	char source[ZH_DATA_SOURCE_SIZE+1];
	unsigned char btCmd[ZH_DATA_SOURCE_CMD_SIZE +1];//最大命令保存的字节64与所有TagCmd一样
	short rLen;
	DWORD dwOperatorTime;
	int delay;
}TzhListDataCmd;

//文件的链表
typedef struct _TckSrcCfgFiles
{
	char filename[ZH_DATA_FILENAME_SIZE+1];
	int nFileSize;
	char md5[ZH_DATA_MD5_SIZE+1];
}TckSrcCfgFiles;

//////////////////////////////////////////
//TCP结构
typedef enum _EckProjTcpStatus{
	eckProjTcpDisconnect,
	eckProjTcpConnecting,
	eckProjTcpConnected
}EckProjTcpStatus;

typedef struct _TckSrcCfgTcp
{
	//结构参数
	struct{
		char source[ZH_DATA_SOURCE_SIZE +1];
		char host[ZH_DATA_HOST_SIZE +1];
		int port;
	}obj;

	struct{
		bool isAlertConnectedFail;
		TYPE_CS csStatus;
		//当前连接状态
		EckProjTcpStatus tStatus;
		SOCKET s;
		//假设TCP在断开状态,最后尝试自动连接的时间,配合ZH_CHECK_TCP_DATA_INTERVAL
		time_t tmeLastReopen;
		//数据会经由datascource的此链表分流到对应TCP或SERIAL的链表
		TzhList lst_waitCmd;
		TYPE_CS csWaitCmd;
	}logic;
}TckSrcCfgTcp;
//////////////////////////////////////////
//data结构

//监控客户端
typedef struct _TckSrcCfgTcpClient
{
	SOCKET sockClient;
	DWORD dwConnectedTime;

	char ip[45];
	int port;
}TckSrcCfgTcpClient;

//proxy的结构体
typedef struct _TckCfgPorxySocket
{
	//结构参数
	struct{
		int port;
		char source[ZH_DATA_SOURCE_SIZE +1];
	}obj;
	struct{
		SOCKET listen_sock; 
		TzhList lst_client;
		TYPE_CS csClient;
	}logic;
}TckCfgPorxySocket;


//config的结构
typedef struct _TckSourceObj
{

	struct{
		struct{
			//串口配置数据
			TzhList lst_eleTCP;
		}obj;
		struct{
			TYPE_CS csTcp;			//tcp互斥锁
			//数据会经由collect的此链表分流到对应TCP的链表
			TzhList lst_insideCmd;	//内部通道处理链表
			TzhList lst_outsideCmd;	//外部通道处理链表
			TYPE_CS csInsideCmd;
			TYPE_CS csOutsideCmd;
		}logic;
	}collect;
	struct{
		struct{
			TzhList lst_porxySocket;
		}obj;
		/*
		struct{
		}logic;
		*/
	}PorxySocket;
}TckWebsocketObj;

typedef struct _TckTcpSocketCfg
{
	//config的结构
	TckWebsocketObj obj;
	struct{
		SOCKET	listen_sock;
	}logic;
}TckTcpSocketCfg;

#ifdef __cplusplus
}
#endif
#define _ZH_USERINFO_H_
#endif
