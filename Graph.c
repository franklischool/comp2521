// Graph ADT interface for Ass2 (COMP2521)
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "Graph.h"

struct GraphRep {
	//
	// Core
	//
	// just need a list of nodes
	// everything within the node has been abstracted away
	AdjList* list;
	size_t maxVertices;

	//
	// Helper members
	//
	AdjList* helperList; //for inbound connections
};

// costly method
static int numVertices(Graph g) {
	int totalUsedVertices = 0;
	int vertexCache[g->maxVertices]; 
	for (int i = 0; i < g->maxVertices; i++) vertexCache[i] = 0; // initialise to 0
	for (int src = 0; src < g->maxVertices; src++) {
		if (g->list[src] != NULL) vertexCache[src] = 1;
		for (adjListNode* curNode = g->list[src]; curNode != NULL; curNode = curNode->next){
			vertexCache[curNode->w] = 1;
		}
	}

	for (int i = 0; i < g->maxVertices; i++)
		if (vertexCache[i] == 1) totalUsedVertices++;
	return totalUsedVertices;
}

Graph newGraph(int noNodes) {
	assert(noNodes > 0); //cannot have a max size of 0
	Graph freshGraph = malloc(sizeof(struct GraphRep)); assert(freshGraph != NULL);
	freshGraph->maxVertices = noNodes;
	freshGraph->list = calloc(noNodes, sizeof(AdjList)); assert(freshGraph->list != NULL);
	freshGraph->helperList = calloc(noNodes, sizeof(AdjList)); assert(freshGraph->helperList != NULL);

	for (int i = 0; i < freshGraph->maxVertices; i++) {
		freshGraph->helperList[i] = malloc(sizeof(adjListNode));
		freshGraph->helperList[i]->w = i;
	}

	return freshGraph;
}

// misspelling it's vertices not verticies
int numVerticies(Graph g) {
	return numVertices(g);
}

void insertEdge(Graph g, Vertex src, Vertex dest, int weight) {
	if (src >= g->maxVertices || dest >= g->maxVertices) { fprintf(stderr, "insertEdge: vertex range exceeded\n"); exit(-1); }
	if (g->list[src] == NULL) { // case empty
		g->list[src] = malloc(sizeof(adjListNode)); assert(g->list[src] != NULL);
		g->list[src]->w = dest;
		g->list[src]->weight = weight;
		g->list[src]->next = NULL;
	} else {
		for (adjListNode* curVertex = g->list[src]; curVertex != NULL; curVertex=curVertex->next) {
			assert(curVertex->w != dest);	// assumes at most 1 path from same src and dest
			if (curVertex->next == NULL) { 	//last one
				curVertex->next = malloc(sizeof(adjListNode)); assert(curVertex->next != NULL);
				curVertex->next->w = dest;
				curVertex->next->weight = weight;
				curVertex->next->next = NULL;
				break;
			}
		}
	}
	return;
}

// one thing to keep in mind is the fact that if a src has no edges, it must be NULL
void removeEdge(Graph g, Vertex src, Vertex dest) {
	if (g->list[src] == NULL) return;	// if there are no outgoing edges from src
	adjListNode* prevVertex = NULL;		// cant put this within initialisation scope of the below loop for some reason
	for (adjListNode* curVertex = g->list[src]; curVertex != NULL; curVertex=curVertex->next) {
		if (curVertex->w == dest) {
			if (prevVertex == NULL) { // first vertex
				g->list[src] = curVertex->next; 
			} else prevVertex->next = curVertex->next;
			free(curVertex);
			break;
		} else prevVertex = curVertex;
	}
}

bool adjacent(Graph g, Vertex src, Vertex dest) {
	for (adjListNode* curVertex = g->list[src]; curVertex != NULL; curVertex=curVertex->next)
		if (curVertex->w == dest) return true;
	return false;
}

AdjList outIncident(Graph g, Vertex v) {
	return g->list[v];
}

AdjList inIncident(Graph g, Vertex v) {
	AdjList list = NULL;
	adjListNode* retListCurNode = NULL;
	for (int src = 0; src < g->maxVertices; src++) {
		if (src == v) continue; // skip own
		for (adjListNode* node = g->list[src]; node != NULL; node=node->next) {
			if (node->w == v) {
				if (list == NULL) { // first one
					list = retListCurNode = g->helperList[src];
				} else {
					retListCurNode->next = g->helperList[src];
					retListCurNode = retListCurNode->next;
				} 
				// retListCurNode->w = node->w;
				retListCurNode->weight = node->weight;
				retListCurNode->next = NULL;
			}
		}
	}

	return list;
}

void showGraph(Graph g) {
	assert (g != NULL);
	printf ("Graph has %d vertices:\n", numVertices(g));
	for (size_t i = 0; i < g->maxVertices; i++) {
		printf("Vertex %zu, connects to: ", i);
		for (adjListNode* curVertex = g->list[i]; curVertex != NULL; curVertex=curVertex->next) {
			printf ("[%d:%d], ", curVertex->w, curVertex->weight);
		}
		putchar ('\n');
	}
}

void freeGraph(Graph g) {
	if (g == NULL) return;
	for (int src = 0; src < g->maxVertices; src++) {
		for (adjListNode* node = g->list[src]; node != NULL; ) {
			adjListNode* curNode = node;
			node = node->next;
			free(curNode);
		}
		free(g->helperList[src]);
	}
	free(g->list);
	free(g->helperList);
}
