#include "objects.h"
#include "main.h"
#include "camera.h"
#include <math.h>
#define FARCLIP -10.0

extern unsigned int framecount;
extern Camera cam;
extern double GV_sx,GV_sy;

void initAABB(AABB *aabb, double lx, double ly, double lz, double hx, double hy, double hz) {
  if (lx>hx) swap(&lx,&hx);
  if (ly>hy) swap(&ly,&hy);
  if (lz>hz) swap(&lz,&hz);
  makeVec(aabb->lo,lx,ly,lz);
  makeVec(aabb->hi,hx,hy,hz);
  aabb->framenumber=0xffffffff;
}

void projectAABB(AABB *aabb, Camera *cam) {
  if (aabb->framenumber==framecount) return;
  aabb->framenumber=framecount;
  bool vis=false;
  aabb->x0=1.0e20; aabb->y0=1.0e20;
  aabb->x1=-1.0e20; aabb->y1=-1.0e20;
  for (int a=0; a<2; a++) for (int b=0; b<2; b++) for (int c=0; c<2; c++) {
    Vector v;
    makeVec(v,a==0?aabb->lo[0]:aabb->hi[0],
              b==0?aabb->lo[1]:aabb->hi[1],
              c==0?aabb->lo[2]:aabb->hi[2]);
    vecSub(v,v,cam->from);
    Vector v2;
    v2[2]=dotPrd(cam->zvec,v);
    if (v2[2]<EPSILON/* && v2[2]>FARCLIP*/) {
      vis=true;
      v2[0]=dotPrd(cam->xvec,v);
      v2[1]=dotPrd(cam->yvec,v);
      double sx,sy,sz_1=-1.0/v2[2];
      sx=v2[0]*sz_1;
      sy=v2[1]*sz_1;
      if (sx<aabb->x0) aabb->x0=sx;
      if (sx>aabb->x1) aabb->x1=sx;
      if (sy<aabb->y0) aabb->y0=sy;
      if (sy>aabb->y1) aabb->y1=sy;
    }
  }
  if (vis) {
    unsigned char rx0,ry0,rx1,ry1;
    if (aabb->x0<-cam->maxx) rx0=0; else if (aabb->x0>cam->maxx) rx0=2; else rx0=1;
    if (aabb->x1<-cam->maxx) rx1=0; else if (aabb->x1>cam->maxx) rx1=2; else rx1=1;
    if (aabb->y0<-cam->maxy) ry0=0; else if (aabb->y0>cam->maxy) ry0=2; else ry0=1;
    if (aabb->y1<-cam->maxy) ry1=0; else if (aabb->y1>cam->maxy) ry1=2; else ry1=1;
    aabb->status=BOX_INOUT;
    unsigned char code=rx0|(ry0<<2)|(rx1<<4)|(ry1<<6);
    if (code==85) aabb->status=BOX_ACCEPT; else
    if (code & 51==0) aabb->status=BOX_REJECT; else
    if (code & 51==34) aabb->status=BOX_REJECT; else
    if (code & 204==0) aabb->status=BOX_REJECT; else
    if (code & 204==136) aabb->status=BOX_REJECT;
  } else aabb->status=BOX_REJECT;
}

void projectFarAABB(AABB *aabb, Camera *cam) {
  if (aabb->framenumber==framecount) return;
  bool vis=false;
  aabb->framenumber=framecount;
  aabb->x0=1.0e20; aabb->y0=1.0e20;
  aabb->x1=-1.0e20; aabb->y1=-1.0e20;
  for (int a=0; a<2; a++) for (int b=0; b<2; b++) for (int c=0; c<2; c++) {
    Vector v;
    makeVec(v,a==0?aabb->lo[0]:aabb->hi[0],
              b==0?aabb->lo[1]:aabb->hi[1],
              c==0?aabb->lo[2]:aabb->hi[2]);
    vecSub(v,v,cam->from);
    Vector v2;
    v2[2]=dotPrd(cam->zvec,v);
    if (v2[2]<EPSILON) {
      vis=true;
      v2[0]=dotPrd(cam->xvec,v);
      v2[1]=dotPrd(cam->yvec,v);
      double sx,sy,sz_1=-1.0/v2[2];
      sx=v2[0]*sz_1;
      sy=v2[1]*sz_1;
      if (sx<aabb->x0) aabb->x0=sx;
      if (sx>aabb->x1) aabb->x1=sx;
      if (sy<aabb->y0) aabb->y0=sy;
      if (sy>aabb->y1) aabb->y1=sy;
    }
  }
  if (vis) {
    unsigned char rx0,ry0,rx1,ry1;
    if (aabb->x0<-cam->maxx) rx0=0; else if (aabb->x0>cam->maxx) rx0=2; else rx0=1;
    if (aabb->x1<-cam->maxx) rx1=0; else if (aabb->x1>cam->maxx) rx1=2; else rx1=1;
    if (aabb->y0<-cam->maxy) ry0=0; else if (aabb->y0>cam->maxy) ry0=2; else ry0=1;
    if (aabb->y1<-cam->maxy) ry1=0; else if (aabb->y1>cam->maxy) ry1=2; else ry1=1;
    aabb->status=BOX_INOUT;
    unsigned char code=rx0|(ry0<<2)|(rx1<<4)|(ry1<<6);
    if (code==85) aabb->status=BOX_ACCEPT; else
    if (code & 51==0) aabb->status=BOX_REJECT; else
    if (code & 51==34) aabb->status=BOX_REJECT; else
    if (code & 204==0) aabb->status=BOX_REJECT; else
    if (code & 204==136) aabb->status=BOX_REJECT;
  } else aabb->status=BOX_REJECT;
}

bool testAABB(AABB *aabb, Ray *ray) {
  if (ray->p[0]>=aabb->lo[0] && ray->p[0]<=aabb->hi[0] &&
      ray->p[1]>=aabb->lo[1] && ray->p[1]<=aabb->hi[1] &&
      ray->p[2]>=aabb->lo[2] && ray->p[2]<=aabb->hi[2]) return true;
  double t1,t2,tnear=-1.0e20,tfar=1.0e20;
  for (int i=0; i<3; i++) {
    if (fabs(ray->dir[i])<EPSILON) {
      if (ray->p[i]<aabb->lo[i] || ray->p[i]>aabb->hi[i]) return false;
    } else {
      t1=(aabb->lo[i]-ray->p[i])*ray->dir_1[i];
      t2=(aabb->hi[i]-ray->p[i])*ray->dir_1[i];
      if (t1>t2) swap(&t1,&t2);
      if (t1>tnear) tnear=t1;
      if (t2<tfar) tfar=t2;
      if (tnear>tfar) return false;
      if (tfar<0.0) return false;
    }
  }
  return true;
}

bool testAABB(AABB *aabb, double sx, double sy) {
  return sx>=aabb->x0 && sy>=aabb->y0 && sx<=aabb->x1 && sy<=aabb->y1;
}

void initFloor(Floor *floor, double _height) {
  floor->height=_height;
}

bool hitFloor(Floor *floor, Ray *ray, double *t) {
  if (fabs(ray->dir[1])<EPSILON) return false;
  if (ray->dir[1]<0.0) {
    if (ray->p[1]>floor->height) {
      *t=-(ray->p[1]-floor->height)/ray->dir[1];
      return true;
    }
  } else {
    if (ray->p[1]<floor->height) {
      *t=-(ray->p[1]-floor->height)/ray->dir[1];
      return true;
    }
  }
  return false;
}

void initSphere(Sphere *sphere, double x, double y, double z, double r) {
  makeVec(sphere->m,x,y,z);
  sphere->r2=r*r;
  initAABB(&sphere->aabb,x-r,y-r,z-r,x+r,y+r,z+r);
}

bool testSphere(Sphere *sphere, Ray *ray) {
  if (framecount!=sphere->aabb.framenumber) projectFarAABB(&sphere->aabb,&cam);
  if (!testAABB(&sphere->aabb,GV_sx,GV_sy)) return false;
  Vector localp;
  vecSub(localp,ray->p,sphere->m);
  if (dotPrd(localp,ray->dir)>0.0) return false;
  double b=2.0*(ray->dir[0]*localp[0]+ray->dir[1]*localp[1]+ray->dir[2]*localp[2]);
  double c=localp[0]*localp[0]+localp[1]*localp[1]+localp[2]*localp[2]-sphere->r2;
  double det=b*b*0.25-c;
  return det>0.0;
}

