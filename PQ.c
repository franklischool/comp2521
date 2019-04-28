// PQ ADT interface for Ass2 (COMP2521)
#include "PQ.h"
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

// #define STATIC_LENGTH 10000
#define REALLOC_SIZE 10000 // fix this
#define UNUSED -1
struct PQRep {
	// make the memory dynamic later on just use a static array for now
	ItemPQ *heap; // index only starts at array[1] instead of array[0], so first one is empty
	// hashTable uses closed hashing since there are no duplicates with
	// 	numeric keys which have values that directly translate
	// 	to the heap's [index] 
	int *hashTable;
	size_t n;
	size_t index;
	size_t currentDynamicSize;
};

// helper functions
static void swap(PQ pq, size_t index1, size_t index2) {
	ItemPQ tempItem = pq->heap[index1];
	pq->heap[index1] = pq->heap[index2];
	pq->heap[index2] = tempItem;

	size_t key1 = pq->heap[index1].key;
	size_t key2 = pq->heap[index2].key;

	pq->hashTable[key1] = index1;
	pq->hashTable[key2] = index2;
}

// returns the index in which the element ends up on
static size_t propagateUp(PQ pq, size_t index) {
	if (index <= 1) return index; // at root
	// check if parent is larger and also follows FIFO with < instead of <=
	if (pq->heap[index].value < pq->heap[index/2].value) { 
		// pass by copy instead of pass by reference
		swap(pq, index, index/2);
		return propagateUp(pq, index/2);
	} else return index;
}

static size_t findSmallestValidChild(PQ pq, size_t index) {
	// check if these children even exist, very convenient that 0 is bool false and also NaN index
	size_t leftChild = index*2 <= pq->index ? index*2 : false;
	size_t rightChild = index*2+1 <= pq->index ? index*2+1: false;
	if (!leftChild) return false; // no children
	if (leftChild && !rightChild) return leftChild;
	if (leftChild && rightChild)
		return pq->heap[leftChild].value < pq->heap[rightChild].value ? leftChild : rightChild;
	return false;
}

static size_t propagateDown(PQ pq, size_t index) {
	if (index >= pq->index) return index; //at leaf
	// assuming there isn't a case where parent is larger than children

	size_t child = findSmallestValidChild(pq, index);
	if (child && pq->heap[child].value <= pq->heap[index].value) {
		swap(pq, index, child);
		return propagateDown(pq, child);
	} else return index;
}
// helper functions

PQ newPQ() {
	// must use struct and not PQ since PQ is a pointer which has a size of only 8bytes
	PQ freshPQ = malloc(sizeof(struct PQRep)); 
	freshPQ->heap = calloc(REALLOC_SIZE, sizeof(ItemPQ));
	freshPQ->hashTable = calloc(REALLOC_SIZE, sizeof(int));
	freshPQ->currentDynamicSize = REALLOC_SIZE;
	// initialises hashtable
	for (int i = 0; i < freshPQ->currentDynamicSize; i++)
		freshPQ->hashTable[i] = UNUSED;
	freshPQ->index = 0;
	return freshPQ;
}

int PQEmpty(PQ p) {
	if (p->index == 0) return true;
	else return false;
}

void addPQ(PQ pq, ItemPQ element) {
	// check if need to realloc
	if (pq->index+1 >= pq->currentDynamicSize) {
		pq->currentDynamicSize += REALLOC_SIZE;
		pq->heap = realloc(pq->heap, pq->currentDynamicSize);
		pq->hashTable = realloc(pq->hashTable, pq->currentDynamicSize);
		for (int i = pq->index + 1; i < pq->currentDynamicSize; i++)
			pq->hashTable[i] = UNUSED;
	}

	// check if key already exists on hash table
	size_t index = pq->hashTable[element.key];
	if (index == UNUSED) {
		pq->index++;
		pq->heap[pq->index] = element; // assume there is enough space
		pq->hashTable[element.key] = propagateUp(pq, pq->index); // adds index to hashtable
	} else {
		// already exists, in this case modify value
		if (element.value < pq->heap[index].value) { // propagate up only
			pq->heap[index].value = element.value;
			pq->hashTable[element.key] = propagateUp(pq, index);
		}
	}
}

// dont bother deallocing memory 
ItemPQ dequeuePQ(PQ pq) {
	assert(pq->index != 0);
	ItemPQ throwAway = pq->heap[1];
	pq->heap[1] = pq->heap[pq->index];
	pq->index--;

	pq->hashTable[throwAway.key] = UNUSED;
	if (pq->index != 0) pq->hashTable[pq->heap[1].key] = 1;
	propagateDown(pq, 1);
	return throwAway;
}

// UPDATED to log time
void updatePQ(PQ pq, ItemPQ element) {
	size_t index = pq->hashTable[element.key];
	if (index == UNUSED) return; 								// nothing to update

	if (element.value < pq->heap[index].value) { 				// propagate up only
		pq->heap[index].value = element.value;
		pq->hashTable[element.key] = propagateUp(pq, index);
	} else if (element.value > pq->heap[index].value) { 		// propagate down only
		pq->heap[index].value = element.value;
		pq->hashTable[element.key] = propagateDown(pq, index);
	}
	// if value is equal do nothing
}

void showPQ(PQ pq) {
	printf("printing in format [index:key:value]:\n");
	int inc = 0;
	for (size_t tracker = pq->index; tracker >= 1; tracker/=2) inc++; 
	for (size_t index = 1, tracker = 2; index <= pq->index; index++) {
		if (index == tracker) {
			putchar('\n');
			tracker*=2;
			inc--;
		}
		printf("[%lu:%d:%d] ", index, pq->heap[index].key, pq->heap[index].value);
	} putchar('\n');
	const int maxDigits = 20;
	printf("printing first %d digits of hashTable:\n", maxDigits);
	for (size_t index = 0; index < maxDigits; index++) {
		printf("%d ", pq->hashTable[index]);
	}
}

void freePQ(PQ pq) {
	free(pq->heap);
	free(pq->hashTable);
	free(pq);
}
