#include "matrix.h"

typedef double Matrix[16];

void transformPoint(Vector res, Vector a, Matrix m) {
  res[0]=a[0]*m[0]+a[1]*m[1]+a[2]*m[2]+m[3];
  res[1]=a[0]*m[4]+a[1]*m[5]+a[2]*m[6]+m[7];
  res[2]=a[0]*m[8]+a[1]*m[9]+a[2]*m[10]+m[11];
}

void transformDir(Vector res, Vector a, Matrix m) {
  res[0]=a[0]*m[0]+a[1]*m[1]+a[2]*m[2];
  res[1]=a[0]*m[4]+a[1]*m[5]+a[2]*m[6];
  res[2]=a[0]*m[8]+a[1]*m[9]+a[2]*m[10];
}

void multiply(Matrix res, Matrix a, Matrix b) {
  for (int i=0; i<4; i++) for (int j=0; j<4; j++) {
    int offs=j<<2|i;
    res[offs]=0.0;
    for (int k=0; k<4; k++) res[offs]+=a[j<<2|k]*b[k<<2|i];
  }
}

