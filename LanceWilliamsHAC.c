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
#include <float.h>

#include "PQ.h" // USE PRIORITY QUEUE

// note this constant only works with this partcular distance function
const double INFINITY = DBL_MAX;

// return max edge weight but min distance
// change INFINITY if this distanceFunction is to be changed
static double distanceFunction(double d1, double d2) {
  return d1 < d2 ? d1 : d2;
}

//
// DynamicMatrix
// 
// see appendix

typedef struct DynamicMatrix* DM;
static DM newDynamicMatrix(int, double);
static double get(DM, int, int);
static void set(DM, int, int, double);
static void freeDynamicMatrix(DM);

static void showDynamicMatrix(DM);

//
// Dendrogram specific stuff
//

// static Dendrogram newDendrogram(int vertex) {
//   Dendrogram d = malloc(sizeof(struct DNode)); assert(d != NULL);
//   d->vertex = vertex;
//   d->left = d->right = NULL;
//   return d;
// }

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
          1.0 / outList->weight
        )
      );
    }
  } 

  if (numVertices < 20) showDynamicMatrix(dm);
  else printf("Hiding matrix, too many vertices\n");

  //
  // start clustering
  //
  // first step is to make every vertex a cluster

  // Dendrogram clusterList[numVertices];
  // for (int i = 0; i < numVertices; i++) clusterList[i] = newDendrogram(i);

  if (method == 1) {

  } else if (method == 2) {

  } else {
    fprintf(stderr, "Incorrect method given by user for LanceWilliamsHAC\n");
    return NULL;
  }

  freeDynamicMatrix(dm);

  return NULL;
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
// Appendix
//
// This ADT comes in handy when memory is a premium and
//  for ease of access in a triangular matrix
//  but adds O(c) to retrieval of data

typedef struct DynamicMatrix {
    int numVertices;
    double defaultValue;
    double **matrix;
} DynamicMatrix;

static DynamicMatrix* newDynamicMatrix(int numVertices, double _defaultValue) {
    DynamicMatrix* matrix = malloc(sizeof(DynamicMatrix)); assert(matrix != NULL);
    matrix->defaultValue = _defaultValue;
    matrix->numVertices = numVertices;
    matrix->matrix = calloc(numVertices, sizeof(double*)); assert(matrix->matrix != NULL);
    for (int i = 0; i < numVertices; i++) {
        matrix->matrix[i] = calloc(i, sizeof(double));
        assert(matrix->matrix[i] != NULL);
        for (int j = 0; j < i; j++) matrix->matrix[i][j] = _defaultValue;
    }

    return matrix;
}

static double get(DynamicMatrix* dm, int a, int b) {
    assert(a >= 0 && b >= 0);
    assert(a < dm->numVertices && b < dm->numVertices);
    if (a > b) return dm->matrix[a][b];
    else if (a < b) return dm->matrix[b][a];
    else return 0;
}

static void set(DynamicMatrix* dm, int a, int b, double val) {
    assert(a >= 0 && b >= 0);
    assert(a < dm->numVertices && b < dm->numVertices);
    if (a > b) dm->matrix[a][b] = val;
    else if (a < b) dm->matrix[b][a] = val;
}

// only for validating this ADT
static void showDynamicMatrix(DynamicMatrix* dm) {
    for (int i = 0; i < dm->numVertices; i++) {
        printf("%d: ", i);
        for (int j = 0; j < i; j++)
            printf("%.4f ", dm->matrix[i][j] == dm->defaultValue ? 0 : dm->matrix[i][j]);
        putchar('\n');
    }
    printf("   ");
    for (int i = 0; i < dm->numVertices - 1; i++)
        printf("%d:     ", i);
    putchar('\n');
}

static void freeDynamicMatrix(DynamicMatrix* dm) {
    for (int i = 0; i < dm->numVertices; i++) free(dm->matrix[i]);
    free(dm->matrix);
    free(dm);
}