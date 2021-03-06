/**********************************************************************
 *
 * selist.c
 *
 * NOTE: This is a C version queue operation wrapper
 * 
 *
 **********************************************************************/

#include "SeHash.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "SeTool.h"

/* Our hash table capability is a power of two */
int tableCapability(int size)
{
	int i = 1024;
	int iMax = 65536;
	
	assert((i&(i - 1)) == 0);
	assert((iMax&(iMax - 1)) == 0);
	if (size >= iMax)
	{
		return iMax;
	}
	while(1)
	{
		if(i >= size)
		{
			return i;
		}
		i *= 2;
	}
}

void SeHashInit(struct SEHASH *root, int max)
{
	int i;

	assert(max > 0);
	root->max = tableCapability(max);
	root->pkMain = (struct SELIST *)SeMallocMem(sizeof(struct SELIST)*root->max);
	SeListInit(&root->list);
	
	for(i = 0; i < root->max; i++)
	{
		SeListInit(&root->pkMain[i]);
	}
}

void SeHashFin(struct SEHASH *root)
{
	SeFreeMem(root->pkMain);
	root->max = 0;
	root->pkMain = 0;
	SeListInit(&root->list);
}

void SeHashNodeInit(struct SEHASHNODE *node)
{
	node->id = 0;
	SeListInitNode(&node->main);
	SeListInitNode(&node->list);
}

void SeHashAdd(struct SEHASH *root, int id, struct SEHASHNODE *node)
{
	int hashid;
	struct SELIST *pkMain;
	
	assert(id >= 0);
	node->id = id;
	hashid = id&(root->max - 1);
	assert(hashid >= 0 && hashid < root->max);
	pkMain = &root->pkMain[hashid];
	SeListTailAdd(pkMain, &node->main);
	SeListTailAdd(&root->list, &node->list);
}

struct SEHASHNODE *SeHashGet(struct SEHASH *root, int id)
{
	int hashid;
	struct SENODE *pkNode;
	struct SELIST *pkMain;
	struct SEHASHNODE *pkHashNode;
	
	assert(id >= 0);
	hashid = id&(root->max - 1);
	assert(hashid >= 0 && hashid < root->max);
	pkMain = &root->pkMain[hashid];

	pkNode = pkMain->head;
	if (!pkNode)
	{
		return 0;
	}
	pkHashNode = SE_CONTAINING_RECORD(pkNode, struct SEHASHNODE, main);
	if (pkHashNode->id == id)
	{
		return pkHashNode;
	}
	pkNode = pkNode->next;
	while(pkNode)
	{
		pkHashNode = SE_CONTAINING_RECORD(pkNode, struct SEHASHNODE, main);
		if (pkHashNode->id == id)
		{
			return pkHashNode;
		}
		pkNode = pkNode->next;
	}
	return 0;
}

struct SEHASHNODE *SeHashGetHead(struct SEHASH *root)
{
	struct SENODE *pkNode;
	struct SEHASHNODE *pkHashNode;

	pkNode = root->list.head;
	if (!pkNode)
	{
		return 0;
	}
	pkHashNode = SE_CONTAINING_RECORD(pkNode, struct SEHASHNODE, list);
	return pkHashNode;
}

struct SEHASHNODE *SeHashRemove(struct SEHASH *root, struct SEHASHNODE *node)
{
	int hashid;
	struct SELIST *pkMain;

	hashid = node->id&(root->max - 1);
	assert(hashid >= 0 && hashid < root->max);
	pkMain = &root->pkMain[hashid];

	SeListRemove(pkMain, &node->main);
	SeListRemove(&root->list, &node->list);
	return node;
}

struct SEHASHNODE *SeHashPop(struct SEHASH *root)
{
	int hashid;
	struct SENODE *pkNode;
	struct SELIST *pkMain;
	struct SEHASHNODE *pkHashNode;

	pkNode = SeListHeadPop(&root->list);
	if (!pkNode)
	{
		return 0;
	}
	pkHashNode = SE_CONTAINING_RECORD(pkNode, struct SEHASHNODE, list);

	hashid = pkHashNode->id&(root->max - 1);
	assert(hashid >= 0 && hashid < root->max);
	pkMain = &root->pkMain[hashid];
	SeListRemove(pkMain, &pkHashNode->main);

	return pkHashNode;
}

void SeHashMoveToEnd(struct SEHASH *root, struct SEHASHNODE *node)
{
	SeListRemove(&root->list, &node->list);
	SeListTailAdd(&root->list, &node->list);
}
