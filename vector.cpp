#include "vector.h"
#include "mem.h"
#include <math.h>
#include "main.h"

void makeVec(Vector a, double x, double y, double z) {
  a[0]=x; a[1]=y; a[2]=z;
}

void vecCopy(Vector dest, Vector src) {
  memcpy(dest,src,sizeof(Vector));
}

void vecAdd(Vector res, Vector a, Vector b) {
  res[0]=a[0]+b[0];
  res[1]=a[1]+b[1];
  res[2]=a[2]+b[2];
}

void vecSub(Vector res, Vector a, Vector b) {
  res[0]=a[0]-b[0];
  res[1]=a[1]-b[1];
  res[2]=a[2]-b[2];
}

void vecMul(Vector res, Vector a, double s) {
  res[0]=a[0]*s;
  res[1]=a[1]*s;
  res[2]=a[2]*s;
}

void crossPrd(Vector res, Vector a, Vector b) {
  res[0]=a[1]*b[2]-b[1]*a[2];
  res[1]=a[2]*b[0]-b[2]*a[0];
  res[2]=a[0]*b[1]-b[0]*a[1];
}

double dotPrd(Vector a, Vector b) {
  return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

double vecLen(Vector a) {
  return sqrt(dotPrd(a,a));
}

void normalize(Vector a) {
  double len=vecLen(a);
  if (fabs(len)<EPSILON) len=1.0;
  vecMul(a,a,1.0/len);
}

