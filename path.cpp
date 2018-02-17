#include "vector.h"
#include "path.h"
#include "stdio.h"

#define VIEWDISTANCE 2.0

void loadPath(Path *path, char *filename) {
  FILE *f;
  f=fopen(filename,"rb");
  unsigned int size;
  fread(&path->length,1,8,f);
  fread(&size,1,4,f);
  path->splinecount=size-3;
  path->pv=new PathVertex[size];
  fread(path->pv,sizeof(PathVertex),size,f);
  fclose(f);
  path->startspline=0;
  path->travelled=0.0;
}

void killPath(Path *path) {
  delete [] path->pv;
}

bool performStep(Path *path) {
  if (path->startspline+1>=path->splinecount) return false;
  PathVertex *p=&path->pv[path->startspline];
  path->travelled+=p->len;
  path->startspline++;
  return true;
}

bool needStep(Path *path, double s) {
  PathVertex *p=&(path->pv[path->startspline]);
  return (s>=path->travelled+p->len);
}

void getSplinePoint(Path *path, double s, Vector v) {
  int thisoffs=path->startspline;
  double thistravel=path->travelled;
  PathVertex *p=&path->pv[thisoffs];
  while (s>=thistravel+p->len) {
    thistravel+=p->len;
    p=&path->pv[++thisoffs];
  }
  double t=(s-thistravel)/p->len;
  if (t<0.0) t=0.0;
  if (t>1.0) t=1.0;
  double t2=t*t;
  double t3=t2*t;
  Vector p0,p1,p2,p3;
  vecCopy(p0,p->v); p++;
  vecCopy(p1,p->v); p++;
  vecCopy(p2,p->v); p++;
  vecCopy(p3,p->v);
  for (int i=0; i<3; i++) v[i]=0.5*((2.0*p1[i])+(p2[i]-p0[i])*t+
                                    (2.0*p0[i]-5.0*p1[i]+4.0*p2[i]-p3[i])*t2+
                                    (3.0*p1[i]-p0[i]-3.0*p2[i]+p3[i])*t3);
}

void getCamera(Path *path, double s, Vector from, Vector at, Vector up) {
  if (s<0.0) s=0.0;
  if (s>path->length) s=path->length;
  if (s>path->length-VIEWDISTANCE) {
    makeVec(from,0.0,0.0,5.0);
    makeVec(at,0.0,0.0,0.0);
    makeVec(up,0.0,1.0,0.0);
    return;
  }
  while (needStep(path,s)) performStep(path);
  getSplinePoint(path,s,from);
  getSplinePoint(path,s+VIEWDISTANCE,at);
  makeVec(up,0.0,1.0,0.0);
}

