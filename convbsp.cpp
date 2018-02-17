#include <math.h>
#include <stdio.h>
#include "vector.h"
#include "matrix.h"

typedef struct _Face {
  unsigned int v[3];
} Face;

typedef struct _Node {
  double a,b,c,d;
  unsigned int facecount,faceoffs,front,back;
} Node;

Vector *vertex;
Face *face;
Node *node;

int main() {
  unsigned int size;
  FILE *f;
  f=fopen("bsp.out","rb");
  fread(&size,1,4,f);
  printf("reading %i vertices...\n",size);
  vertex=new Vector[size];
  fread(vertex,sizeof(Vector),size,f);
  fread(&size,1,4,f);
  printf("reading %i faces...\n",size);
  face=new Face[size];
  fread(face,sizeof(Face),size,f);
  fread(&size,1,4,f);
  printf("reading %i nodes...\n",size);
  node=new Node[size];
  fread(node,sizeof(Node),size,f);
  fclose(f);
  
  delete [] vertex;
  delete [] face;
  delete [] node;

}

