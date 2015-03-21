#ifndef __SE_NETSOCKET_H__
#define __SE_NETSOCKET_H__

#include "SeList.h"
#include "SeNetBase.h"
#include "SeNetStream.h"

#define CSOCKET_STATUS_INIT 0
#define CSOCKET_STATUS_CONNECT 1
#define CSOCKET_STATUS_DISCONNECT 2
#define CSOCKET_STATUS_ACTIVECONNECT 3

struct SECSOCKETNODE
{
	HSOCKET				kHSocket;
	SOCKET				kBelongToListenSocket;
	int					iStatus;
	int					iFlag;
	struct SENETSTREAM	kSendNetStream;
	struct SENETSTREAM	kRecvNetStream;
	struct SENODE		kNode;
};

struct SENETCSOCKET
{
	int					iListCount;
	struct SELIST		kList;
};

void SeNetCSocketNodeInit(struct SECSOCKETNODE *pkNetCSocketNode);

void SeNetCSocketInit(struct SENETCSOCKET *pkNetCSocket);

void SeNetCSocketHeadAdd(struct SENETCSOCKET *pkNetCSocket, struct SECSOCKETNODE *pkNetCSocketNode);

void SeNetCSocketTailAdd(struct SENETCSOCKET *pkNetCSocket, struct SECSOCKETNODE *pkNetCSocketNode);

struct SECSOCKETNODE *SeNetCSocketRemove(struct SENETCSOCKET *pkNetCSocket, struct SECSOCKETNODE *pkNetCSocketNode);


struct SESSOCKETNODE
{
	SOCKET				kListenSocket;
	struct SENODE		kNode;
};

struct SENETSSOCKET
{
	int					iListCount;
	struct SELIST		kList;
};

void SeNetSSocketNodeInit(struct SESSOCKETNODE *pkNetSSocketNode);

void SeNetSSocketInit(struct SENETSSOCKET *pkNetSSocket);

void SeNetSSocketAdd(struct SENETSSOCKET *pkNetSSocket, struct SESSOCKETNODE *pkNetSSocketNode);

struct SESSOCKETNODE *SeNetSSocketPop(struct SENETSSOCKET *pkNetSSocket);

#endif
