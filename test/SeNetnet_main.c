#include "SeList.h"
#include <stdio.h>
#include "SeHash.h"
#include "SeTime.h"
#include <stdbool.h>
#include "SeNetCore.h"

bool kSetHeaderLenFun(unsigned char* pcHeader, const int iheaderlen, const int ilen)
{

	if (iheaderlen == 2)
	{
		pcHeader[0] = (ilen >> 8) & 0xff;
		pcHeader[1] = ilen & 0xff;
		return true;
	}
/*
	if (iheaderlen == 0)
	{
		return true;
	}
*/
	return false;
}

bool kGetHeaderLenFun(const unsigned char* pcHeader, const int iheaderlen, int *ilen)
{
	if (iheaderlen == 2)
	{
		*ilen = pcHeader[0] << 8 | pcHeader[1];
		return true;
	}
/*
	if (iheaderlen == 0)
	{
		*ilen = 0;
		return true;
	}
*/
	return false;
}

int main()
{
	unsigned long long num;
	unsigned long long timer;
	unsigned long long timera;

	bool ret;
	int iLen;
	int riEvent;
	int rSSize;
	int rRSize;
	HSOCKET khsocket, socket;
	HSOCKET rkHSocket;
	char *buf = (char*)SeMallocMem(1024*1024*4);
	HSOCKET rkListenHSocket;
	struct SENETCORE kNet;
	
	num = 0;
	timer = SeTimeGetTickCount();

	SeNetCoreInit(&kNet, "out.txt", 1000*30, 10000);
	khsocket = SeNetCoreTCPListen(&kNet, "0.0.0.0", 8888, 2, &kGetHeaderLenFun, &kSetHeaderLenFun);
	
	while(khsocket != 0)
	{
		iLen = 1024*1024*4;
		ret = SeNetCoreRead(&kNet, &riEvent, &rkListenHSocket, &rkHSocket, buf, &iLen, &rSSize, &rRSize);
		if(!ret) { continue; }

		if(riEvent == SENETCORE_EVENT_SOCKET_IDLE)
		{
			SeTimeSleep(1);
			continue;
		}

		if(riEvent == SENETCORE_EVENT_SOCKET_CONNECT_FAILED)
		{
			printf("Client connect failed! %llx\n", rkHSocket);
		}

		if(riEvent == SENETCORE_EVENT_SOCKET_CONNECT)
		{
		}

		if(riEvent == SENETCORE_EVENT_SOCKET_DISCONNECT)
		{
			printf("disconnect! %llx\n", rkHSocket);
		}

		if(riEvent == SENETCORE_EVENT_SOCKET_RECV_DATA)
		{
			//if(iLen == 0) {break;}
			num++;
			timera = SeTimeGetTickCount();
			SeNetCoreSend(&kNet, rkHSocket, buf, iLen);

			if(timera - timer >= 2000)
			{
				iLen = (int)num;
				num = (num/(timera - timer))*1000;
				printf("num=%d,rSSize=%d,rRSize=%d,%d=%d\n", iLen, rSSize, rRSize, (int)num, (int)sizeof(long));
				num = 0;
				timer = SeTimeGetTickCount();
			}
		}
	}

	printf("end\n");

	getchar();
	SeFreeMem(buf);
	SeNetCoreFin(&kNet);
	return 0;
}
