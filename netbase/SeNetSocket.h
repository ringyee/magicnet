#ifndef __SE_NETSOCKET_H__
#define __SE_NETSOCKET_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "SeTime.h"
#include "SeList.h"
#include "SeTool.h"
#include "SeHash.h"
#include "SeNetBase.h"
#include "SeNetStream.h"

enum SOCKET_STATUS
{
	SOCKET_STATUS_INIT = 0,
	SOCKET_STATUS_CONNECTING = 1,
	SOCKET_STATUS_CONNECTED = 2,
	SOCKET_STATUS_CONNECTED_FAILED = 3,
	SOCKET_STATUS_DISCONNECT = 4,
	SOCKET_STATUS_COMM_IDLE = 5,
	SOCKET_STATUS_ACTIVECONNECT = 6
};

enum LISTEN_TCP_TYPE
{
	LISTEN_TCP_TYPE_SOCKET = 1,
	CLIENT_TCP_TYPE_SOCKET = 2,
	ACCEPT_TCP_TYPE_SOCKET = 3
};

enum EVENT_SOCKET
{
	READ_EVENT_SOCKET = (1<<1),
	WRITE_EVENT_SOCKET = (1<<2)
};

struct SE_ALIGN(64) SESOCKET
{
	struct SEHASHNODE		kMainNode;
	struct SEHASHNODE		kSendNode;
	struct SEHASHNODE		kRecvNode;

	HSOCKET					kHSocket;
	HSOCKET					kBelongListenHSocket;
	unsigned short			usStatus;
	unsigned short			usIndex;
	int						iHeaderLen;
	int						iTypeSocket;
	int						iEventSocket;
	char					acBindSvrName[128];
	long long				llFlag;//user flag
	unsigned long long		llTime;
	unsigned long long		llActiveTimeOut;
	SEGETHEADERLENFUN		pkGetHeaderLenFun;
	SESETHEADERLENFUN		pkSetHeaderLenFun;

	struct SENETSTREAM		kSendNetStream;
	struct SENETSTREAM		kRecvNetStream;

	struct sockaddr_in		kRemoteAddr;
};

// 64 ����
struct SE_ALIGN(64) SESOCKETMGR
{
	int						iMax;
	int						iCounter;
	struct SEHASH			*pkMainList;
	struct SEHASH			*pkActiveMainList;
	struct SEHASH			*pkSendList;
	struct SEHASH			*pkRecvList;
	struct SENETSTREAM		*pkNetStreamIdle;
	struct SESOCKET			*pkSeSocket;
};

void SeNetSocketMgrInit(struct SESOCKETMGR *pkNetSocketMgr, unsigned short usMax);

void SeNetSocketMgrFin(struct SESOCKETMGR *pkNetSocketMgr);

HSOCKET SeNetSocketMgrAdd(struct SESOCKETMGR *pkNetSocketMgr, SOCKET socket, int iTypeSocket, int iHeaderLen, \
			SEGETHEADERLENFUN pkGetHeaderLenFun, SESETHEADERLENFUN pkSetHeaderLenFun);

void SeNetSocketMgrDel(struct SESOCKETMGR *pkNetSocketMgr, HSOCKET kHSocket);

struct SESOCKET *SeNetSocketMgrGet(struct SESOCKETMGR *pkNetSocketMgr, HSOCKET kHSocket);

void SeNetSocketMgrAddSendOrRecvInList(struct SESOCKETMGR *pkNetSocketMgr, struct SESOCKET *pkNetSocket, bool bSendOrRecv);

struct SESOCKET *SeNetSocketMgrPopSendOrRecvOutList(struct SESOCKETMGR *pkNetSocketMgr, bool bSendOrRecv);

bool SeNetSocketMgrUpdateNetStreamIdle(struct SESOCKETMGR *pkNetSocketMgr, int iHeaderLen, int iBufLen);

void SeNetSocketMgrActive(struct SESOCKETMGR *pkNetSocketMgr, struct SESOCKET *pkNetSocket);

const struct SESOCKET *SeNetSocketMgrTimeOut(struct SESOCKETMGR *pkNetSocketMgr);

bool SeNetSocketMgrHasEvent(struct SESOCKET *pkNetSocket, int iEventSocket);

void SeNetSocketMgrAddEvent(struct SESOCKET *pkNetSocket, int iEventSocket);

void SeNetSocketMgrClearEvent(struct SESOCKET *pkNetSocket, int iEventSocket);

#ifdef	__cplusplus
}
#endif

#endif
