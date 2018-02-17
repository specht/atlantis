#include <windows.h>
#include <stdio.h>
#include <math.h>
#include "bsp.h"
#include "vector.h"
#include "main.h"
#define FARCLIP 100000.0
#define epsilon 0.000001
#define SPHERE_ACCEPT 1
#define SPHERE_REJECT 2
#define SPHERE_INOUT 3

extern Camera cam;
extern unsigned int framecount;
extern double GV_sx,GV_sy;

Ray *act_ray;
BSPTree *act_tree;
int *act_index;
unsigned char c0,c1,c2;
bool fastexit;
Vector cut,normal;
extern Plane realfrustum[4];

void buildAABB(unsigned int ni) {
  Node *n=&act_tree->node[ni];
  Vector lo,hi;
  AABB *box;
  makeVec(lo,1.0e20,1.0e20,1.0e20);
  makeVec(hi,-1.0e20,-1.0e20,-1.0e20);
  if (n->front!=BSPNULL) {
    buildAABB(n->front);
    box=&act_tree->planebox[n->front];
    vecCopy(lo,box->lo);
    vecCopy(hi,box->hi);
  }
  if (n->back!=BSPNULL) {
    buildAABB(n->back);
    box=&act_tree->planebox[n->back];
    if (lo[0]<box->lo[0]) lo[0]=box->lo[0];
    if (lo[1]<box->lo[1]) lo[1]=box->lo[1];
    if (lo[2]<box->lo[2]) lo[2]=box->lo[2];
    if (hi[0]>box->hi[0]) hi[0]=box->hi[0];
    if (hi[1]>box->hi[1]) hi[1]=box->hi[1];
    if (hi[2]>box->hi[2]) hi[2]=box->hi[2];
  }
  for (unsigned int i=0; i<n->facecount; i++) {
    Face *f=&act_tree->face[i+n->faceoffs];
    for (int k=0; k<3; k++) {
      Vertex *v=&act_tree->vertex[f->v[k]];
      if (v->x<lo[0]) lo[0]=v->x;
      if (v->y<lo[1]) lo[1]=v->y;
      if (v->z<lo[2]) lo[2]=v->z;
      if (v->x>hi[0]) hi[0]=v->x;
      if (v->y>hi[1]) hi[1]=v->y;
      if (v->z>hi[2]) hi[2]=v->z;
    }
  }
  initAABB(&act_tree->planebox[ni],lo[0]-epsilon,lo[1]-epsilon,lo[2]-epsilon,hi[0]+epsilon,hi[1]+epsilon,hi[2]+epsilon);
}

void loadBSPTree(BSPTree *tree, char *filename) {
  unsigned int size;
  FILE *f;
  f=fopen(filename,"rb");
  fread(&size,1,4,f);
  tree->vertex=new Vertex[size];
  fread(tree->vertex,sizeof(Vertex),size,f);
  fread(&size,1,4,f);
  tree->face=new Face[size];
  fread(tree->face,sizeof(Face),size,f);
  fread(&size,1,4,f);
  tree->plane=new Plane[size];
  fread(tree->plane,sizeof(Plane),size,f);
  tree->cache=new PlaneCache[size];
  for (unsigned int i=0; i<size; i++) {
    PlaneCache *p=&(tree->cache[i]);
    Plane *pl=&(tree->plane[i]);
    p->framenumber=0xffffffff;
    if (fabs(pl->a)>fabs(pl->b)) {
      if (fabs(pl->a)>fabs(pl->c)) {
        c0=1; c1=2;
      } else {
        c0=0; c1=1;
      }
    } else {
      if (fabs(pl->b)>fabs(pl->c)) {
        c0=0; c1=2;
      } else {
        c0=0; c1=1;
      }
    }
    p->c0=c0;
    p->c1=c1;
  }
  fread(&size,1,4,f);
  tree->planebox=new AABB[size];
  tree->node=new Node[size];
  fread(tree->node,sizeof(Node),size,f);
  fclose(f);
  act_tree=tree;
  buildAABB(0);
}


void killBSPTree(BSPTree *tree) {
  delete [] tree->vertex;
  delete [] tree->face;
  delete [] tree->plane;
  delete [] tree->cache;
  delete [] tree->node;
  delete [] tree->planebox;
}

bool insideTriangle(int index) {
  Face *f=&(act_tree->face[index]);
  Vertex *u=&act_tree->vertex[f->v[0]];
  Vertex *v=&act_tree->vertex[f->v[1]];
  Vertex *w=&act_tree->vertex[f->v[2]];
  Vector v0,v1,v2;
  makeVec(v0,u->x,u->y,u->z);
  makeVec(v1,v->x,v->y,v->z);
  makeVec(v2,w->x,w->y,w->z);
  double k0,k1,k2;
  double ex,ey,fx,fy;
  ex=v1[c0]-v0[c0];
  ey=v1[c1]-v0[c1];
  fx=cut[c0]-v0[c0];
  fy=cut[c1]-v0[c1];
  k0=ex*fy-ey*fx;
  ex=v2[c0]-v1[c0];
  ey=v2[c1]-v1[c1];
  fx=cut[c0]-v1[c0];
  fy=cut[c1]-v1[c1];
  k1=ex*fy-ey*fx;
  ex=v0[c0]-v2[c0];
  ey=v0[c1]-v2[c1];
  fx=cut[c0]-v2[c0];
  fy=cut[c1]-v2[c1];
  k2=ex*fy-ey*fx;
  return (k0<0.0 && k1<0.0 && k2<0.0) || (k0>0.0 && k1>0.0 && k2>0.0);
}

int cullBoundingSphere(Node *n) {
  bool accept=true;
  for (int i=0; i<4; i++) {
    double d=n->m[0]*realfrustum[i].a+n->m[1]*realfrustum[i].b+n->m[2]*realfrustum[i].c+realfrustum[i].d;
    if (d<-n->r) return SPHERE_REJECT;
    if (d<n->r) accept=false;
  }
  return accept? SPHERE_ACCEPT: SPHERE_INOUT;
}

bool hitBoundingSphere(Face *f, Ray *ray) {
  Vector tp;
  vecSub(tp,ray->p,f->m);
  double b=dotPrd(ray->dir,tp);
  double c=dotPrd(tp,tp)-f->r2;
  return b*b-c>=0.0;
}

bool hitBoundingSphere(Node *n, Ray *ray) {
  Vector tp;
  vecSub(tp,ray->p,n->m);
  double b=dotPrd(ray->dir,tp);
  double c=dotPrd(tp,tp)-n->r2;
  return b*b-c>=0.0;
}

void recurseTree(unsigned int ni, bool trivaccept) {
  Node* n=&act_tree->node[ni];
  bool acceptchildren=trivaccept;
  Plane *p=&(act_tree->plane[n->plane]);
  PlaneCache *cache=&(act_tree->cache[n->plane]);
  double tri_hit_term;
  if (cache->framenumber==framecount) tri_hit_term=cache->d; else {
    tri_hit_term=act_ray->p[0]*p->a+act_ray->p[1]*p->b+act_ray->p[2]*p->c+p->d;
    cache->d=tri_hit_term;
    cache->framenumber=framecount;
  }
  bool facing=tri_hit_term>0.0;

  if (!trivaccept) {
    int res=cullBoundingSphere(n);
    if (res==SPHERE_REJECT) return;
    if (res==SPHERE_ACCEPT) acceptchildren=true;
//    AABB *box;
//    box=&act_tree->planebox[ni];
//    if (framecount!=box->framenumber) projectAABB(box,&cam);
//    if (box->status==BOX_REJECT) return;
//    if (box->status==BOX_ACCEPT) acceptchildren=true;
  }

  if (facing) {
    if (n->back!=BSPNULL) recurseTree(n->back,acceptchildren);
  } else if (n->front!=BSPNULL) recurseTree(n->front,acceptchildren);
  if (fastexit) return;

  if (facing) {
    AABB *box;
    box=&act_tree->planebox[ni];
//    if (framecount!=box->framenumber) projectAABB(box,&cam);
    if (hitBoundingSphere(n,act_ray)) {
      if (testAABB(box,act_ray)) {

        c0=cache->c0; c1=cache->c1; c2=3-c0-c1;
        double d=act_ray->dir[0]*p->a+act_ray->dir[1]*p->b+act_ray->dir[2]*p->c;
        if (fabs(d)>EPSILON) {
          tri_hit_term/=-d;

          if (tri_hit_term>0.0) {
            cut[c0]=act_ray->p[c0]+tri_hit_term*act_ray->dir[c0];
            cut[c1]=act_ray->p[c1]+tri_hit_term*act_ray->dir[c1];
            for (int i=0; i<n->facecount; i++) if (hitBoundingSphere(&act_tree->face[n->faceoffs+i],act_ray)){
              if (insideTriangle(n->faceoffs+i)) {
                *act_index=n->faceoffs+i;
                fastexit=true;
                cut[c2]=act_ray->p[c2]+tri_hit_term*act_ray->dir[c2];
                normal[0]=p->a;
                normal[1]=p->b;
                normal[2]=p->c;
              }
            }
          }
        }
      }
    }
  }
  if (fastexit) return;

  if (facing) {
    if (n->front!=BSPNULL) recurseTree(n->front,acceptchildren);
  } else if (n->back!=BSPNULL) recurseTree(n->back,acceptchildren);
}

bool hitBSPTree(BSPTree *tree, Ray *ray, int *index, Vector cutp, Vector cutn) {
  act_tree=tree; act_ray=ray; act_index=index;
  fastexit=false;
  recurseTree(0,false);
  vecCopy(cutp,cut);
  vecCopy(cutn,normal);
  return fastexit;
}

