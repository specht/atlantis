#include "lights.h"

void initSpot(Spot *spot, Vector _p, Vector _dir, double angle) {
  vecCopy(spot->p,_p);
  vecCopy(spot->dir,_dir);
  spot->angle=angle;
}

