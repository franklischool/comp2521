// Graph ADT interface for Ass2 (COMP2521)
#include "CentralityMeasures.h"
#include "Dijkstra.h"
#include "PQ.h"
#include <stdlib.h>
#include <stdio.h>

// helper functions
static NodeValues newNV(int noNodes) {
	NodeValues NV;
	NV.noNodes = noNodes;
	NV.values = calloc(noNodes, sizeof(*(NV.values))); // sets to 0 which is important
	return NV;
}

static int noNodes(AdjList list) {
	int num = 0;
	for (adjListNode* curNode = list; curNode != NULL; curNode = curNode->next) num++;
	return num;
}

static bool checkIfPathXHasVertexV(ShortestPaths* paths, PredNode* x, Vertex v) {
	for (PredNode* node = x; node != NULL;) {
		if (node->v == v) return true;
		node = paths->pred[node->v];
	}
	return false;
}
// helper functions

NodeValues outDegreeCentrality(Graph g) {
	NodeValues NV = newNV(numVerticies(g));

	for (Vertex v = 0; v < NV.noNodes; v++) {
		AdjList list = outIncident(g, v);
		NV.values[v] = noNodes(list);
	}

	return NV;
}

NodeValues inDegreeCentrality(Graph g) {
	NodeValues NV = newNV(numVerticies(g));

	for (Vertex v = 0; v < NV.noNodes; v++) {
		AdjList list = inIncident(g, v);
		NV.values[v] = noNodes(list);
	}

	return NV;
}

NodeValues degreeCentrality(Graph g) {
	NodeValues NV = newNV(numVerticies(g));

	for (Vertex v = 0; v < NV.noNodes; v++) {
		AdjList inList = inIncident(g, v);
		AdjList outList = outIncident(g, v);
		NV.values[v] = noNodes(inList) + noNodes(outList);
	}

	return NV;
}

NodeValues closenessCentrality(Graph g) {
	NodeValues NV = newNV(numVerticies(g));

	for (Vertex v = 0; v < NV.noNodes; v++) {
		ShortestPaths paths = dijkstra(g, v);
		double distance = 0;
		int noReachableNodes = 1;
		for (int i = 0; i < paths.noNodes; i++) {
			if (paths.pred[i] == NULL) continue;
			distance += paths.dist[i];
			noReachableNodes++;
		}

		double multiplier = (double)((noReachableNodes-1)*(noReachableNodes-1)) / (double)(numVerticies(g)-1);
		NV.values[v] = distance == 0 ? 0 : multiplier / distance;
		freeShortestPaths(paths);
	}

	return NV;
}

NodeValues betweennessCentrality(Graph g) {
	NodeValues NV = newNV(numVerticies(g));

	// this method is less straight forward but significantly more optimised
	// in either case this function is extremely expensive and there are alot of ways to minimise the cost
	//	but it can get really messy
	ShortestPaths paths[NV.noNodes];
	for (Vertex i = 0; i < NV.noNodes; i++) paths[i] = dijkstra(g, i);

	for (Vertex v = 0; v < NV.noNodes; v++) {
		for (Vertex src = 0; src < NV.noNodes; src++) {
			if (src == v) continue;
			for (Vertex dest = 0; dest < NV.noNodes; dest++) {
				if (dest == v || dest == src || paths[src].pred[dest] == NULL) continue;
				int noSPS = 0;
				int noSPS_ThroughV = 0;
				for (PredNode* node = paths[src].pred[dest]; node != NULL; node = node->next) {
					if (checkIfPathXHasVertexV(&(paths[src]), node, v))
						noSPS_ThroughV++;
					noSPS++;
				}
				NV.values[v] += (double)noSPS_ThroughV / (double)noSPS; 
			}
		}

	}

	for (Vertex i = 0; i < NV.noNodes; i++) freeShortestPaths(paths[i]);

	return NV;
}

NodeValues betweennessCentralityNormalised(Graph g) {
	NodeValues NV = betweennessCentrality(g);

	for (int i = 0; i < NV.noNodes; i++)
		NV.values[i] *= NV.noNodes > 2 ? 1.0 / (double)( (NV.noNodes - 1) * (NV.noNodes - 2) ) : 0.0;

	return NV;
}

void showNodeValues(NodeValues values) {
	for (int i = 0; i < values.noNodes; i++) printf("%d: %f\n", i, values.values[i]);
}

void freeNodeValues(NodeValues values) {
	// for (Vertex v = 0; v < values.noNodes; v++) // not sure if this is necessary
	// 	free(values.values[v]);
	free(values.values);
}
