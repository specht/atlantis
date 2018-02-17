#ifndef _LIGHTSH
#define _LIGHTSH

#include "vector.h"

typedef struct _Spot {
  Vector p,dir;
  double angle;
} Spot;

void initSpot(Spot *spot, Vector _p, Vector _dir, double angle);

#endif

