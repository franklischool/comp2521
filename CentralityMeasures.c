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

// recurse through all possible shortest paths given a starting point
// essentially counting number of leaves in a non-binary tree except with the leaves
// 	all being the same node i.e. being src
static int findPaths(int output[], ShortestPaths* paths, PredNode* node, Vertex src) {
	if (node == NULL) return 0;

	int downPaths = findPaths(output, paths, paths->pred[node->v], src);
	int sidePaths = findPaths(output, paths, node->next, src);

	output[node->v] += downPaths;
	
	return sidePaths + downPaths + (node->v == src ? 1 : 0);
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

	// go through all paths check for in betweens
	for (Vertex src = 0; src < NV.noNodes; src++) {
		ShortestPaths path = dijkstra(g, src);
		for (Vertex dest = 0; dest < NV.noNodes; dest++) {
			if (dest == src) continue;
			
			// initialise arrays
			int numPaths = 0;
			int numPathsWithInBetweenNode[NV.noNodes];
			for (int i = 0; i < NV.noNodes; i++) numPathsWithInBetweenNode[i] = 0;

			numPaths = findPaths(numPathsWithInBetweenNode, &path, path.pred[dest], src);

			for (int i = 0; i < NV.noNodes; i++)
				NV.values[i] += numPaths == 0 ? 0 : (double)numPathsWithInBetweenNode[i] / (double)numPaths;

		}
	}
	
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
