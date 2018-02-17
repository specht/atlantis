#ifndef _CAMERAH
#define _CAMERAH

#include "vector.h"
#include "matrix.h"

typedef struct _Camera {
  Vector from,at,up;
  double angle,aspect;
// "private" stuff
  Vector xvec,yvec,zvec;
  double xsize,ysize,maxx,maxy;
} Camera;

typedef struct _Ray {
  Vector p,dir,dir_1; // 1.0 / dir
} Ray;

typedef struct _Plane {
  double a,b,c,d;
} Plane;

void initCamera(Camera *cam);
void putCamera(Camera *cam, Vector _from, Vector _at, Vector _up);
void putCamera(Camera *cam, double fx, double fy, double fz,
                            double ax, double ay, double az,
                            double ux, double uy, double uz);
                            
extern Plane frustum[4];

#endif
