#ifndef _OBJECTSH
#define _OBJECTSH

#include "camera.h"
#include "vector.h"

#define BOX_INOUT  0
#define BOX_ACCEPT 1
#define BOX_REJECT 2

typedef struct _AABB {
  Vector lo,hi;
  double x0, y0, x1, y1;
  unsigned int framenumber;
  unsigned int status;
} AABB;

typedef struct _Floor {
  double height;
} Floor;

typedef struct _Sphere {
  Vector m;
  double r2;
  AABB aabb;
} Sphere;

void initAABB(AABB *aabb, double lx, double ly, double lz, double hx, double hy, double hz);
void projectAABB(AABB *aabb, Camera *cam);
void projectFarAABB(AABB *aabb, Camera *cam);
bool testAABB(AABB *aabb, Ray *ray);
bool testAABB(AABB *aabb, double sx, double sy);

void initFloor(Floor *floor, double _height);
bool hitFloor(Floor *floor, Ray *ray, double *t);

void initSphere(Sphere *sphere, double x, double y, double z, double r);
//bool hitSphere(Sphere *sphere, Ray *ray, double *t);
bool testSphere(Sphere *sphere, Ray *ray);

#endif
