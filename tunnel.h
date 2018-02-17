#ifndef _TUNNELH
#define _TUNNELH

#include "camera.h"
#include "vector.h"
const unsigned int MAX_SCREEN_VERTEX=2048;

typedef struct _TVertex {
  double x,y,z;
} TVertex;

typedef struct _TFace {
  unsigned int v0,v1,v2,dummyedge;
  double a,b,c,d;
} TFace;

typedef struct _TGate {
  unsigned int voffs,vcount;
} TGate;

typedef struct _TSegment {
  unsigned int foffs,fcount;
} TSegment;

typedef struct _TFaceCache {
  double d;
  unsigned char c0,c1;
//  double lx,ly,hx,hy;
} TFaceCache;

typedef struct _TGateCache {
  unsigned int framenumber,svoffs,svcount;
} TGateCache;

typedef struct _TScreenVertex {
  double x,y;
} TScreenVertex;

typedef struct _Tunnel {
  TVertex *vertex;
  TFace *face;
  TGate *gate;
  TSegment *segment;
  TFaceCache *fcache;
  TGateCache *gcache;
  TScreenVertex *sv;
  unsigned int gatecount,startgate,svoffs;
  int mingate;
} Tunnel;

void loadTunnel(Tunnel *tunnel, char *filename);
void prepareTunnel(Tunnel *tunnel);
bool hitTunnel(Tunnel *tunnel, Ray *ray, int *index, Vector cut, Vector n, double *t);
void killTunnel(Tunnel *tunnel);

#endif

