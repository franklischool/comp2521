// Dijkstra ADT interface for Ass2 (COMP2521)
#include "Dijkstra.h"
#include "PQ.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

// helper functions
#include <limits.h>

static void deletePreviousPredNodesOfVertex(ShortestPaths* paths, Vertex v) {
	for (PredNode* node = paths->pred[v], *nextNode; node != NULL; node = nextNode) {
		nextNode = node->next;
		free(node);
	}
}
// helper functions

ShortestPaths dijkstra(Graph g, Vertex v) {
	const int nV = numVerticies(g);
	ShortestPaths paths;
	paths.noNodes = nV;
	paths.src = v;
	paths.pred = calloc(nV, sizeof(PredNode*));
	paths.dist = calloc(nV, sizeof(int));

	// for the purpose of this algorithm 3 arrays are needed: Known, Dist, Pred
	bool known[nV];
	for (int i = 0; i < nV; i++) {
		known[i] = false;
		paths.dist[i] = INT_MAX;			// refer to infinity
		paths.pred[i] = NULL; 				// NULL refers to no predecesser
	}

	// initialise entry into pq
	PQ pq = newPQ();
	ItemPQ item = {.key = v, .value = 0}; 	// first value is not important
	paths.dist[item.key] = 0; 				// source has no cost associated
	addPQ(pq, item);

	// main loop
	while (!PQEmpty(pq)) {
		ItemPQ srcItem = dequeuePQ(pq);
		known[srcItem.key] = true;
		for (adjListNode* node = outIncident(g, srcItem.key); node != NULL; node = node->next) {
			if (!known[node->w]) {
				int nodalCost = paths.dist[srcItem.key] + node->weight;
				if (nodalCost < paths.dist[node->w]) {
					paths.dist[node->w] = nodalCost;
					// new predNodes
					deletePreviousPredNodesOfVertex(&paths, node->w);
					// assert(paths.pred[node->w] == NULL); // might not be set to NULL
					paths.pred[node->w] = malloc(sizeof(PredNode));
					paths.pred[node->w]->v = srcItem.key;
					paths.pred[node->w]->next = NULL;
				} else if (nodalCost == paths.dist[node->w]) {	
					// edge case where there should be multiple predecessors & dist is same
					// the break condition in for loop header should never be triggered
					for (PredNode* curPredNode = paths.pred[node->w]; curPredNode != NULL; curPredNode = curPredNode->next) {
						if (curPredNode->next == NULL) {
							curPredNode->next = malloc(sizeof(PredNode));
							curPredNode->next->v = srcItem.key;
							curPredNode->next->next = NULL;
							break;
						}
					}
				}
				item.key = node->w;
				item.value = nodalCost;
				addPQ(pq, item);			// only adds if new cost is lower
			}
		}
	}

	for (Vertex curVertex = 0; curVertex < nV; curVertex++) {
		if (paths.pred[curVertex] == NULL && paths.dist[curVertex] == INT_MAX)
			paths.dist[curVertex] = 0;
	}

	freePQ(pq);
	return paths;
}

void showShortestPaths(ShortestPaths sps) {
	int i = 0;
	printf("Node %d\n",sps.src);
	printf("  Distance\n");
	for (i = 0; i < sps.noNodes; i++) {
			if(i == sps.src)
	    	printf("    %d : X\n",i);
			else
				printf("    %d : %d\n",i,sps.dist[i]);
	} 
	printf("  Preds\n");
	for (i = 0; i < sps.noNodes; i++) {
		int numPreds = 0;
		int preds[sps.noNodes];
		printf("    %d : ",i);
		PredNode *curr = sps.pred[i];
		while (curr != NULL && numPreds < sps.noNodes) {
			preds[numPreds++] = curr->v;
			curr = curr->next;
		}
		
		// Insertion sort
		for (int j = 1; j < numPreds; j++) {
			int temp = preds[j];
			int k = j;
			while (preds[k - 1] > temp && k > 0) {
				preds[k] = preds[k - 1];
				k--;
			}
			preds[k] = temp;
		}
		
		for (int j = 0; j < numPreds; j++) {
			printf("[%d]->", preds[j]);
		}
		printf("NULL\n");
	}
}

void freeShortestPaths(ShortestPaths paths) {
	for (int i = 0; i < paths.noNodes; i++) {
		PredNode* tempNode = NULL;
		for (PredNode* node = paths.pred[i]; node != NULL; node = tempNode) {
			tempNode = node->next;
			free(node);
		}
	}
	free(paths.pred);
}
