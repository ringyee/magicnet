#include "SeNetTcp.h"

void SeNetTcpCreate(struct SENETTCP *pkNetTcp, char *pcLogName)
{
	int iBegin;

	pkNetTcp->kHandle = SE_INVALID_HANDLE;
	pkNetTcp->usCounter = 0;
	
	SeInitLog(&pkNetTcp->kLog, pcLogName);
	SeNetCSocketInit(&pkNetTcp->kFreeCSocketList);
	SeNetCSocketInit(&pkNetTcp->kAcceptCSocketList);
	SeNetCSocketInit(&pkNetTcp->kActiveCSocketList);
	SeNetCSocketInit(&pkNetTcp->kConnectCSocketList);
	SeNetCSocketInit(&pkNetTcp->kDisConnectCSocketList);

	for(iBegin = 0; iBegin < MAX_SOCKET_LEN; iBegin++) {
		SeNetCSocketNodeInit(&pkNetTcp->kClientSocket[iBegin]);
		pkNetTcp->kClientSocket[iBegin].iFlag = iBegin;
		SeNetCSocketHeadAdd(&pkNetTcp->kFreeCSocketList, &pkNetTcp->kClientSocket[iBegin]);
	}

	pkNetTcp->pkOnConnectFunc = 0;
	pkNetTcp->pkOnDisconnectFunc = 0;
	pkNetTcp->pkOnRecvDataFunc = 0;
}

void SeNetTcpFree(struct SENETTCP *pkNetTcp)
{
	struct SENETSTREAM kMemCache;
	struct SECSOCKETNODE *pkNetCSocketNode;
	struct SENETSTREAMNODE *pkNetStreamNode;
	
	SeNetSreamInit(&kMemCache);
	SeFinLog(&pkNetTcp->kLog);
	
	pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kFreeCSocketList);
	while(pkNetCSocketNode) {
		SeNetCSocketNodeFin(pkNetCSocketNode, &kMemCache);
		pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kFreeCSocketList);
	}

	pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kActiveCSocketList);
	while(pkNetCSocketNode) {
		SeCloseSocket(SeGetSocketByHScoket(pkNetCSocketNode->kHSocket));
		SeNetCSocketNodeFin(pkNetCSocketNode, &kMemCache);
		pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kActiveCSocketList);
	}

	pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kConnectCSocketList);
	while(pkNetCSocketNode) {
		SeCloseSocket(SeGetSocketByHScoket(pkNetCSocketNode->kHSocket));
		SeNetCSocketNodeFin(pkNetCSocketNode, &kMemCache);
		pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kConnectCSocketList);
	}

	pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kDisConnectCSocketList);
	while(pkNetCSocketNode) {
		SeCloseSocket(SeGetSocketByHScoket(pkNetCSocketNode->kHSocket));
		SeNetCSocketNodeFin(pkNetCSocketNode, &kMemCache);
		pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kDisConnectCSocketList);
	}

	pkNetStreamNode = SeNetSreamHeadPop(&kMemCache);
	while(pkNetStreamNode) {
		SeFreeMem((void*)pkNetStreamNode);
		pkNetStreamNode = SeNetSreamHeadPop(&kMemCache);
	}
	
	pkNetTcp->usCounter = 0;
	SeCloseHandle(pkNetTcp->kHandle);
	pkNetTcp->kHandle = SE_INVALID_HANDLE;

	pkNetTcp->pkOnConnectFunc = 0;
	pkNetTcp->pkOnDisconnectFunc = 0;
	pkNetTcp->pkOnRecvDataFunc = 0;
}

struct SECSOCKETNODE* SeNetTcpAddSvr(struct SENETTCP *pkNetTcp, const char *pcIP, unsigned short usPort, int iMemSize, int iProtoFormat)
{
	SOCKET kSocket;
	struct linger so_linger;
	struct sockaddr kServerAddr;
	struct SECSOCKETNODE *pkNetCSocketNode;

	if(pkNetTcp->kHandle == SE_INVALID_HANDLE) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init Handle feaild\n");
		return 0;
	}

	pkNetCSocketNode = SeNetCSocketPop(&pkNetTcp->kFreeCSocketList);
	if(!pkNetCSocketNode) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "CSocketNode malloc feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}
	SeNetCSocketNodeInit(pkNetCSocketNode);
	
	kSocket = SeSocket(SOCK_STREAM);
	if(kSocket == SE_INVALID_SOCKET) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init Listen socket feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}
	
	SeSetSockAddr(&kServerAddr, pcIP, usPort);
	SeSetReuseAddr(kSocket);

	if(SeBind(kSocket, &kServerAddr) != 0) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init Bind socket feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}

	if(SeListen(kSocket, 1000) != 0) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init Listen socket feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}

	if(SeSetNoBlock(kSocket, true) != 0) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init set no black feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}

	so_linger.l_onoff = true;
	so_linger.l_linger = 0;
	if(SeSetSockOpt(kSocket,SOL_SOCKET,SO_LINGER,(char*)&so_linger,sizeof(so_linger)) != 0) {
		SeLogWrite(&pkNetTcp->kLog, LT_CRITICAL, true, "Init set SO_LINGER feaild, addr=%s, port=%d\n", pcIP, (int)usPort);
		return 0;
	}
	
	pkNetTcp->usCounter++;
	pkNetCSocketNode->kHSocket = SeGetHSocket(pkNetTcp->usCounter, pkNetCSocketNode->iFlag, kSocket);
	pkNetCSocketNode->kSvrSocket = kSocket;
	pkNetCSocketNode->iStatus = CSOCKET_STATUS_ACCEPT;
	pkNetCSocketNode->iProtoFormat = iProtoFormat;
	pkNetCSocketNode->llMemSize = iMemSize;

	return pkNetCSocketNode;
}
