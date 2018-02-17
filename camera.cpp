#include "camera.h"
#include "vector.h"
#include <math.h>
#include "demo.h"
#include <stdio.h>
#include <windows.h>
#include "main.h"

Plane frustum[4],realfrustum[4];

void initCamera(Camera *cam) {
  cam->angle=45.0;
  cam->aspect=16.0/9.0;
  cam->xsize=tan(cam->angle*M_PI/180.0);
  cam->ysize=cam->xsize/cam->aspect;
  cam->maxx=cam->xsize;
  cam->maxy=cam->ysize;
  cam->xsize/=40.0;
  cam->ysize/=19.0;
  putCamera(cam,0.0,0.0,8.0,0.0,0.0,0.0,0.0,1.0,0.0);
}

void putCamera(Camera *cam, Vector _from, Vector _at, Vector _up) {
  putCamera(cam,_from[0],_from[1],_from[2],_at[0],_at[1],_at[2],_up[0],_up[1],_up[2]);
}

void putCamera(Camera *cam, double fx, double fy, double fz,
                            double ax, double ay, double az,
                            double ux, double uy, double uz) {
  makeVec(cam->from,fx,fy,fz);
  makeVec(cam->at,ax,ay,az);
  makeVec(cam->up,ux,uy,uz);
  vecSub(cam->zvec,cam->from,cam->at);
  normalize(cam->zvec);
  crossPrd(cam->xvec,cam->up,cam->zvec);
  normalize(cam->xvec);
  crossPrd(cam->yvec,cam->xvec,cam->zvec);
  normalize(cam->yvec);
  Vector v[4],xv,yv;
  vecMul(xv,cam->xvec,cam->maxx);
  vecMul(yv,cam->yvec,cam->maxy);
  vecMul(xv,xv,2.0);
  vecMul(yv,yv,2.0);
  for (int i=0; i<3; i++) {
    v[0][i]=-xv[i]-yv[i]+cam->zvec[i];
    v[1][i]=+xv[i]-yv[i]+cam->zvec[i];
    v[2][i]=+xv[i]+yv[i]+cam->zvec[i];
    v[3][i]=-xv[i]+yv[i]+cam->zvec[i];
  }
  for (int i=0; i<4; i++) {
    int k=(i+1)%4;
    Vector n;
    crossPrd(n,v[i],v[k]);
    normalize(n);
    frustum[i].a=n[0];
    frustum[i].b=n[1];
    frustum[i].c=n[2];
    frustum[i].d=-dotPrd(cam->from,n)+0.0000001;
  }
  vecMul(xv,cam->xvec,cam->maxx);
  vecMul(yv,cam->yvec,cam->maxy);
  for (int i=0; i<3; i++) {
    v[0][i]=-xv[i]-yv[i]+cam->zvec[i];
    v[1][i]=+xv[i]-yv[i]+cam->zvec[i];
    v[2][i]=+xv[i]+yv[i]+cam->zvec[i];
    v[3][i]=-xv[i]+yv[i]+cam->zvec[i];
  }
  for (int i=0; i<4; i++) {
    int k=(i+1)%4;
    Vector n;
    crossPrd(n,v[i],v[k]);
    normalize(n);
    realfrustum[i].a=n[0];
    realfrustum[i].b=n[1];
    realfrustum[i].c=n[2];
    realfrustum[i].d=-dotPrd(cam->from,n)+0.0000001;
  }
/*  char s[200];
  sprintf(s,"frustum: %.2lf, %.2lf, %.2lf, %.2lf |%.2lf, %.2lf, %.2lf, %.2lf |",
  frustum[0].a,frustum[0].b,frustum[0].c,frustum[0].d,
  frustum[1].a,frustum[1].b,frustum[1].c,frustum[1].d);
  MessageBox(hwnd,s,"",MB_OK); */
}

