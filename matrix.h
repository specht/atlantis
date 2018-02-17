#ifndef _MATRIXH
#define _MATRIXH

#include "vector.h"

typedef double Matrix[16];

void transformPoint(Vector res, Vector a, Matrix m);
void transformDir(Vector res, Vector a, Matrix m);
void multiply(Matrix res, Matrix a, Matrix b);

#endif
