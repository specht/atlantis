#ifndef _VECTORH
#define _VECTORH

typedef double Vector[3];

void makeVec(Vector a, double x, double y, double z);
void vecCopy(Vector dest, Vector src);
void vecAdd(Vector res, Vector a, Vector b);
void vecSub(Vector res, Vector a, Vector b);
void vecMul(Vector res, Vector a, double s);
void crossPrd(Vector res, Vector a, Vector b);
double dotPrd(Vector a, Vector b);
double vecLen(Vector a);
void normalize(Vector a);

#endif
