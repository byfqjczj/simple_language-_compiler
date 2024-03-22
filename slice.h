
#include <stdlib.h>
#include <stdbool.h>


typedef struct Slice {
    char* start;
    size_t len;
}Slice;

bool sliceEq(Slice* a, Slice* b)
{
    if(a->len!=b->len)
	{
        return false;
    }
	for(int i = 0;(size_t)i<a->len;i++)
	{
		if(a->start[i]!=b->start[i])
		{
			return false;
		}
	}
	return true;

}
typedef struct sliceNode{
	struct sliceNode* nextNode;
	Slice* key;
	uint64_t val;
}sliceNode;

typedef struct SliceToIntHashMap{
	sliceNode** arrStart;
	int nBuckets;
}SliceToIntHashMap;

int sliceToIntHashFunction(Slice* key){
	char* temp = key->start;
	int len = key->len;
	int cur = 0;
	for(int i=0;i<len;i++)
	{
		cur = cur + (int)temp[i];
	}
	return cur;
}

struct SliceToIntHashMap* createSliceToIntHashMap(int numBuckets){
	sliceNode** hmapStart= (sliceNode**) calloc(numBuckets,sizeof(sliceNode*));
	SliceToIntHashMap* hshmp = (SliceToIntHashMap*) malloc(sizeof(SliceToIntHashMap));
	hshmp->arrStart=hmapStart;
	hshmp->nBuckets=numBuckets;
	return hshmp;
}

void sliceToIntHashMapInsert(struct SliceToIntHashMap* hmap, Slice* key, uint64_t value){
	int ky = sliceToIntHashFunction(key);
	int hash = ky % hmap->nBuckets;
	sliceNode* newNodePtr = (sliceNode*) malloc(sizeof(sliceNode));
	newNodePtr->nextNode=0;
	newNodePtr->key=key;
	newNodePtr->val=value;
	sliceNode* toPoint = hmap->arrStart[hash];
	if(toPoint==0)
	{
		hmap->arrStart[hash]=newNodePtr;
		return;
	}
	while(toPoint!=0)
	{
		if(sliceEq(toPoint->key,key))
		{
			toPoint->val=value;
			free(newNodePtr);
			return;
		}
		if(toPoint->nextNode==0)
		{
			toPoint->nextNode=newNodePtr;
			return;
		}
		toPoint = toPoint->nextNode;
	}
	return;
}

uint64_t sliceToIntHashMapGet(struct SliceToIntHashMap* hmap, Slice* key){
	int ky = sliceToIntHashFunction(key);
	int hash = ky % hmap->nBuckets;
	sliceNode* toPoint = hmap->arrStart[hash];
	if(toPoint==0)
	{
		return 0;
	}
	while(toPoint!=0)
	{
		if(sliceEq(toPoint->key,key))
		{
			return toPoint->val;
		}
		toPoint = toPoint->nextNode;
	}
	return 0;
}