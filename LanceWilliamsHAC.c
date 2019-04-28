/*Lance-Williams Algorithm for Agglomerative Clustering
  Written by
  COMP2521 2019 T1
*/
#include <stdlib.h>
#include <stdio.h>
#include "LanceWilliamsHAC.h"
#include "Graph.h"

//
// Helper Functions
//

#include <assert.h>
#include <limits.h>

#include "PQ.h"

typedef struct LinkedList* LL;
typedef struct LinkedList {
  Dendrogram d;
  LL next;
} LinkedList;

// note this constant only works with this partcular distance function
const int INFINITY = -8; // flip it over
const int EMPTY = -1;

// return max edge weight but min distance
// change INFINITY if this distanceFunction is to be changed
static int distanceFunction(int d1, int d2) {
  return d1 > d2 ? d1 : d2;
}

// static int distanceFunction2(Dendrogram d, int curMax) {
//   if (d == NULL) return INFINITY;
//   int d1 = distanceFunction2(d->left, curMax);
//   int d2 = distanceFunction2(d->right, curMax);

//   return d1 < d2 ? d1 : d2;
// }

//
// DynamicMatrix
// 
// see appendix

typedef struct DynamicMatrix* DM;
static DM newDynamicMatrix(int, int);
static int get(DM, int, int);
static void set(DM, int, int, int);
static void freeDynamicMatrix(DM);

static void showDynamicMatrix(DM);

//
// Dendrogram specific stuff
//

static Dendrogram newDendrogram(int vertex) {
  Dendrogram d = malloc(sizeof(struct DNode)); assert(d != NULL);
  d->vertex = vertex;
  d->left = d->right = NULL;
  return d;
}

static Dendrogram mergeDendrogram(Dendrogram left, Dendrogram right) {
  Dendrogram newDendrogramParent = newDendrogram(left->vertex);
  newDendrogramParent->left = left;
  newDendrogramParent->right = right;
  return newDendrogramParent;
}

// left leave -> right leave graph traversal
static void showDendrogram(Dendrogram d) {
  if (d == NULL) return;
  showDendrogram(d->left);
  showDendrogram(d->right);
  printf("%d ", d->vertex);
}

//
// Implementation
//

// assume direction does not matter
// assume weight >= 0
Dendrogram LanceWilliamsHAC(Graph g, int method) {
  assert(g != NULL);
  const int numVertices = numVerticies(g);

  // initialise distanceMatrix
  DM dm = newDynamicMatrix(numVertices, INFINITY);
  for (int src = 0; src < numVertices; src++) {
    for (AdjList outList = outIncident(g, src); outList != NULL; outList = outList->next) {
      set(dm, src, outList->w,
        distanceFunction(
          get(dm, src, outList->w), 
          outList->weight
        )
      );
    }
  } 

  // for debugging purposes only, remove this
  if (false) {
    if (numVertices < 20) showDynamicMatrix(dm);
    else printf("Hiding matrix, too many vertices\n");
  }

  // initialise PQ
  PQ pq = newPQ();
  for (int i = 0; i < numVertices; i++) {
    for (int j = 0; j < i; j++) {
      ItemPQ item;
      item.key = i;
      item.value = get(dm, i, j);
      addPQ(pq, item);
    }
  }

  // initialise clusterArray
  Dendrogram clusterArray[numVertices];
  for (int i = 0; i < numVertices; i++) {
    clusterArray[i] = newDendrogram(i);
  }

  // initialise hashtable for searching for vertices within clusters within clusterArray
  int hashTable[numVertices];
  for (int i = 0; i < numVertices; i++) {
    hashTable[i] = i;
  }

  // main cluster loop
  while (!PQEmpty(pq)) {
    ItemPQ item = dequeuePQ(pq);
    for (int i = 0; i < numVertices; i++) { // search for dest
      if (i == item.key) continue;
      if (get(dm, item.key, i) == item.value) { // doesn't matter if there are duplicate values
        // merge the clusters
        clusterArray[hashTable[item.key]] = 
          mergeDendrogram(clusterArray[hashTable[item.key]], clusterArray[hashTable[i]]);
        clusterArray[hashTable[i]] = NULL;  // no longer exists there
        hashTable[i] = hashTable[item.key]; // means this cluster now exists in that cluster
        break;
      }
    }
  }

  // attach to head
  Dendrogram head = NULL;
  for (int i = 0; i < numVertices; i++) {
    if (clusterArray[i] != NULL) {
      head = clusterArray[i];
      break;
    }
  }

  if (false) {
    showDendrogram(head); 
    putchar('\n');
  }

  if (method == 1) {

  } else if (method == 2) {

  } else {
    fprintf(stderr, "Incorrect method given by user for LanceWilliamsHAC\n");
    return NULL;
  }

  freeDynamicMatrix(dm);
  freePQ(pq);

  return head;
}

// use recursion and assume user correctly inputs a non-null d
void freeDendrogram(Dendrogram d) {
  if (d == NULL) return;
  freeDendrogram(d->left);
  freeDendrogram(d->right);
  free(d);

	return;
}

//
//
//
// Appendix (would usually be in a separate .c file)
//
//
//

// This ADT comes in handy when memory is a premium and
//  for ease of access in a triangular matrix
//  but adds O(c) to retrieval of data

typedef struct DynamicMatrix {
    int numVertices;
    int defaultValue;
    int **matrix;
} DynamicMatrix;

static DM newDynamicMatrix(int numVertices, int _defaultValue) {
    DM matrix = malloc(sizeof(DynamicMatrix)); assert(matrix != NULL);
    matrix->defaultValue = _defaultValue;
    matrix->numVertices = numVertices;
    matrix->matrix = calloc(numVertices, sizeof(int*)); assert(matrix->matrix != NULL);
    for (int i = 0; i < numVertices; i++) {
        matrix->matrix[i] = calloc(i, sizeof(int));
        assert(matrix->matrix[i] != NULL);
        for (int j = 0; j < i; j++) matrix->matrix[i][j] = _defaultValue;
    }

    return matrix;
}

static int get(DM dm, int a, int b) {
    assert(a >= 0 && b >= 0);
    assert(a < dm->numVertices && b < dm->numVertices);
    if (a > b) return dm->matrix[a][b];
    else if (a < b) return dm->matrix[b][a];
    else return 0;
}

static void set(DM dm, int a, int b, int val) {
    assert(a >= 0 && b >= 0);
    assert(a < dm->numVertices && b < dm->numVertices);
    if (a > b) dm->matrix[a][b] = val;
    else if (a < b) dm->matrix[b][a] = val;
}

// only for validating this ADT
static void showDynamicMatrix(DM dm) {
    for (int i = 0; i < dm->numVertices; i++) {
        printf("%d: ", i);
        for (int j = 0; j < i; j++)
            printf("%4d ", dm->matrix[i][j] == dm->defaultValue ? 0 : dm->matrix[i][j]);
        putchar('\n');
    }
    printf("   ");
    for (int i = 0; i < dm->numVertices - 1; i++)
        printf("%d:     ", i);
    putchar('\n');
}

static void freeDynamicMatrix(DM dm) {
    for (int i = 0; i < dm->numVertices; i++) free(dm->matrix[i]);
    free(dm->matrix);
    free(dm);
}

//
// PQ (straight copy and paste)
//

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
