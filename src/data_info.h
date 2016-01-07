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
//���ں�TCPͨѶ����
typedef enum _EckCollectDirect
{
	eckDirectUnknow=0,
	eckDirectInside=1,
	eckDirectOutside =2
}EckCollectDirect;

//��������Ľṹ
typedef struct _TzhListDataCmd
{
	char source[ZH_DATA_SOURCE_SIZE+1];
	unsigned char btCmd[ZH_DATA_SOURCE_CMD_SIZE +1];//����������ֽ�64������TagCmdһ��
	short rLen;
	DWORD dwOperatorTime;
	int delay;
}TzhListDataCmd;

//�ļ�������
typedef struct _TckSrcCfgFiles
{
	char filename[ZH_DATA_FILENAME_SIZE+1];
	int nFileSize;
	char md5[ZH_DATA_MD5_SIZE+1];
}TckSrcCfgFiles;

//////////////////////////////////////////
//TCP�ṹ
typedef enum _EckProjTcpStatus{
	eckProjTcpDisconnect,
	eckProjTcpConnecting,
	eckProjTcpConnected
}EckProjTcpStatus;

typedef struct _TckSrcCfgTcp
{
	//�ṹ����
	struct{
		char source[ZH_DATA_SOURCE_SIZE +1];
		char host[ZH_DATA_HOST_SIZE +1];
		int port;
	}obj;

	struct{
		bool isAlertConnectedFail;
		TYPE_CS csStatus;
		//��ǰ����״̬
		EckProjTcpStatus tStatus;
		SOCKET s;
		//����TCP�ڶϿ�״̬,������Զ����ӵ�ʱ��,���ZH_CHECK_TCP_DATA_INTERVAL
		time_t tmeLastReopen;
		//���ݻᾭ��datascource�Ĵ������������ӦTCP��SERIAL������
		TzhList lst_waitCmd;
		TYPE_CS csWaitCmd;
	}logic;
}TckSrcCfgTcp;
//////////////////////////////////////////
//data�ṹ

//��ؿͻ���
typedef struct _TckSrcCfgTcpClient
{
	SOCKET sockClient;
	DWORD dwConnectedTime;

	char ip[45];
	int port;
}TckSrcCfgTcpClient;

//proxy�Ľṹ��
typedef struct _TckCfgPorxySocket
{
	//�ṹ����
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


//config�Ľṹ
typedef struct _TckSourceObj
{

	struct{
		struct{
			//������������
			TzhList lst_eleTCP;
		}obj;
		struct{
			TYPE_CS csTcp;			//tcp������
			//���ݻᾭ��collect�Ĵ������������ӦTCP������
			TzhList lst_insideCmd;	//�ڲ�ͨ����������
			TzhList lst_outsideCmd;	//�ⲿͨ����������
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
	//config�Ľṹ
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
