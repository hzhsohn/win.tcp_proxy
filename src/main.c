#include "proc.h"
#include <windows.h>


int main(int argc,char *argv[])
{
	if(argc<4)
	{
		printf("TCP���ݴ������������������:\r\n\r\n[Ŀ��������IP] [Ŀ��IP] [���ؼ����˿�]\r\n\r\n");
		system("pause");
		exit(0);
	}

	printf("------------------------------------------------------------------\r\n");
	printf("-- TCP���ݴ����������\r\n");
	printf("--\r\n");
	printf("-- ����: Han Zhi Hong\r\n");
	printf("-- ��Ʒ��ַ: http://www.hx-kong.com\r\n");
	printf("-- ��Ѷ����\r\n");
	printf("-- �汾v1.0\r\n");	
	printf("--\r\n");
	printf("------------------------------------------------------------------\r\n");
	printf("\r\n");
	printf("����Ŀ��->%s:%s\r\n",argv[1],argv[2]);
	printf("�����˿�->%s\r\n",argv[3]);
	
	InitTransChannel(argv[1],atoi(argv[2]),atoi(argv[3]));
	//��ʼ�����߼�
	InitProcThread();
	return 0;
}
