#ifndef _BSPH
#define _BSPH
#include "vector.h"
#include "camera.h"
#include "animate.h"
#include "objects.h"

#define BSPNULL 0xffffffff

typedef struct _Vertex {
  double x,y,z;
} Vertex;

typedef struct _Face {
  unsigned int v[3],dummy;
  Vector m;
  double r,r2;
} Face;

typedef struct _PlaneCache {
  unsigned int framenumber;
  double d;
  unsigned char c0,c1;
} PlaneCache;

typedef struct _Node {
  unsigned int plane,facecount,faceoffs,dummy;
  Vector m;
  double r,r2;
  unsigned int front,back;
} Node;

typedef struct _BSPTree {
  Vertex *vertex;
  Face *face;
  Plane *plane;
  PlaneCache *cache;
  Node *node;
  AABB *planebox;
} BSPTree;

void loadBSPTree(BSPTree *tree, char *filename);
void killBSPTree(BSPTree *tree);
bool hitBSPTree(BSPTree *tree, Ray *ray, int *index, Vector cutp, Vector cutn);

#endif

