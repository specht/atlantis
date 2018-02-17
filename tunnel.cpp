#include "tunnel.h"
#include <stdio.h>
#include <mem.h>
#include <math.h>
#include "main.h"

typedef struct _ClipPoint {
  Vector v;
  double clipcode[4];
} ClipPoint;

extern double demotime;
extern double GV_sx,GV_sy;
extern Camera cam;
extern unsigned int framecount;
extern int phase;
Tunnel *act_tunnel;
Vector act_cut,act_n;
Ray *tact_ray;
int *tact_index;
ClipPoint vin[64],vout[64];
unsigned int cin,cout;
unsigned char tc0,tc1,tc2;
char s[200];

void loadTunnel(Tunnel *tunnel, char *filename) {
  FILE *f;
  f=fopen(filename,"rb");
  unsigned int size;

  fread(&size,1,4,f);
  tunnel->vertex=new TVertex[size];
  fread(tunnel->vertex,sizeof(TVertex),size,f);

  fread(&size,1,4,f);
  tunnel->face=new TFace[size];
  tunnel->fcache=new TFaceCache[size];
  fread(tunnel->face,sizeof(TFace),size,f);
  for (unsigned int i=0; i<size; i++) {
    TFaceCache *p=&tunnel->fcache[i];
    TFace *pl=&tunnel->face[i];
    unsigned char c0,c1;
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
  tunnel->gatecount=size;
  tunnel->startgate=0;
  tunnel->mingate=0;
  tunnel->gate=new TGate[size];
  tunnel->gcache=new TGateCache[size];
  for (int i=0; i<size; i++) (&(tunnel->gcache[i]))->framenumber=0xffffffff;
  fread(tunnel->gate,sizeof(TGate),size,f);
  fread(&size,1,4,f);
  tunnel->segment=new TSegment[size];
  fread(tunnel->segment,sizeof(TSegment),size,f);
  fclose(f);
  tunnel->sv=new TScreenVertex[MAX_SCREEN_VERTEX];
}

void prepareTunnel(Tunnel *tunnel) {
  act_tunnel=tunnel;
  tunnel->svoffs=0;
  tunnel->startgate=tunnel->mingate;
  tunnel->mingate=9999999;
}

bool diffSgn(double a, double b) { // !!!!!!!!!!!!!!!!!! SLOW
  return (a*b<0.0);
}

void clipToPlane(unsigned int which, ClipPoint *vin, unsigned int *cin, ClipPoint *vout, unsigned int *cout) {
  *cout=0;
  for (unsigned int i=0; i<(*cin); i++) {
    unsigned int k=(i+1)%(*cin);
    if (vin[i].clipcode[which]>0.0) {
      memcpy(&vout[(*cout)],&vin[i],sizeof(ClipPoint));
      (*cout)++;
    }
    if (diffSgn(vin[i].clipcode[which],vin[k].clipcode[which])) {
      double scale=vin[i].clipcode[which]/(vin[i].clipcode[which]-vin[k].clipcode[which]);
      vecSub(vout[*cout].v,vin[k].v,vin[i].v);
      vecMul(vout[*cout].v,vout[*cout].v,scale);
      vecAdd(vout[*cout].v,vout[*cout].v,vin[i].v);
      
      for (int j=0; j<4; j++) vout[*cout].clipcode[j]=
        frustum[j].a*vout[*cout].v[0]+
        frustum[j].b*vout[*cout].v[1]+
        frustum[j].c*vout[*cout].v[2]+
        frustum[j].d;
        
      (*cout)++;
    }
  }
}

int projectGate(unsigned int gatenum) {
/*  sprintf(s,"projecting gate no. %i",gatenum);
  MessageBox(hwnd,s,"",MB_OK); */
// clip, project,
// build facecache for all gate faces
// return false if completely clipped

  TGate *act_gate=&act_tunnel->gate[gatenum];
  TGateCache *act_gcache=&act_tunnel->gcache[gatenum];
  act_gcache->framenumber=framecount;

  cin=act_gate->vcount;
  for (unsigned int i=0; i<cin; i++) {
    makeVec(vin[i].v,(&act_tunnel->vertex[act_gate->voffs+i])->x,
                     (&act_tunnel->vertex[act_gate->voffs+i])->y,
                     (&act_tunnel->vertex[act_gate->voffs+i])->z);
    for (int k=0; k<4; k++) vin[i].clipcode[k]=frustum[k].a*vin[i].v[0]+
      frustum[k].b*vin[i].v[1]+frustum[k].c*vin[i].v[2]+frustum[k].d;

  }

  Vector a,b,c;
  vecSub(a,vin[1].v,vin[0].v);
  vecSub(b,vin[2].v,vin[1].v);
  crossPrd(c,a,b);
  vecSub(a,cam.from,vin[0].v);
  if (dotPrd(a,c)<0.0) {
    if (phase==2 || phase ==4) return 1;
    act_tunnel->startgate=gatenum+1;
    if (act_tunnel->startgate>=act_tunnel->gatecount) act_tunnel->startgate=act_tunnel->gatecount-1;
    act_tunnel->mingate=act_tunnel->startgate;
    return 1;
  }

  clipToPlane(0,vin,&cin,vout,&cout);
  if (cout<3) return 2;
  clipToPlane(1,vout,&cout,vin,&cin);
  if (cin<3) return 2;
  clipToPlane(2,vin,&cin,vout,&cout);
  if (cout<3) return 2;
  clipToPlane(3,vout,&cout,vin,&cin);
  if (cin<3) return 2;
  
// projection coming right up...
  act_gcache->svoffs=act_tunnel->svoffs;
  act_gcache->svcount=cin;
  for (unsigned int i=0; i<cin; i++) {
    TScreenVertex *act_sv=&act_tunnel->sv[act_tunnel->svoffs];
    Vector p;
    vecSub(p,vin[i].v,cam.from);
    double d=-1.0/dotPrd(p,cam.zvec);
    act_sv->x=dotPrd(p,cam.xvec)*d;
    act_sv->y=dotPrd(p,cam.yvec)*d;
    act_tunnel->svoffs++;
  }
  
// building facecache...

  int segnum=(int)gatenum-1;
  if (segnum>=0) {
    TSegment *act_tsegment=&act_tunnel->segment[segnum];
    for (unsigned int i=0; i<act_tsegment->fcount; i++) {
      TFaceCache *act_fcache=&act_tunnel->fcache[i+act_tsegment->foffs];
      TFace *p=&act_tunnel->face[i+act_tsegment->foffs];
      act_fcache->d=cam.from[0]*p->a+cam.from[1]*p->b+cam.from[2]*p->c+p->d;
    }
  }
/*  FILE *f;
  f=fopen("atlantis.log","a");
  fprintf(f,"Frame no. %i: projected gate no. %i\n",framecount,gatenum);
  for (int i=0; i<act_gcache->svcount; i++) {
    TScreenVertex *sv=&act_tunnel->sv[act_gcache->svoffs+i];
    fprintf(f,"screen vertex: [%.2lf|%.2lf]\n",sv->x,sv->y);
  }
  fclose(f); */
  return 0;
}

bool insideGate(unsigned int gatenum) {
  TGateCache *act_gcache;

  act_gcache=&act_tunnel->gcache[gatenum];
  if (act_gcache->framenumber!=framecount) {
    int a=projectGate(gatenum);
    if (a==1) return true;
    if (a==2) return false;
//      sprintf(s,"couldn't project gate %i, not setting offset to %i",gatenum,act_tunnel->startgate+1);
//      MessageBox(hwnd,s,"",MB_OK);
//      act_tunnel->startgate++;
  }

  for (unsigned int i=0; i<act_gcache->svcount; i++) {
    TScreenVertex *sv0,*sv1;
    sv0=&act_tunnel->sv[act_gcache->svoffs+i];
    sv1=&act_tunnel->sv[act_gcache->svoffs+((i+1)%act_gcache->svcount)];
    double ex=GV_sx-sv0->x, ey=GV_sy-sv0->y;
    double fx=sv1->x-sv0->x, fy=sv1->y-sv0->y;
    if (ex*fy-fx*ey<0.0) return false;
  }
  return true;
}

bool insideTriangle(unsigned int index) {
  TFace *f=&act_tunnel->face[index];
  TVertex *u=&act_tunnel->vertex[f->v0];
  TVertex *v=&act_tunnel->vertex[f->v1];
  TVertex *w=&act_tunnel->vertex[f->v2];
  Vector v0,v1,v2;
  makeVec(v0,u->x,u->y,u->z);
  makeVec(v1,v->x,v->y,v->z);
  makeVec(v2,w->x,w->y,w->z);
  double k0,k1,k2;
  double ex,ey,fx,fy;
  ex=v1[tc0]-v0[tc0];
  ey=v1[tc1]-v0[tc1];
  fx=act_cut[tc0]-v0[tc0];
  fy=act_cut[tc1]-v0[tc1];
  k0=ex*fy-ey*fx;
  ex=v2[tc0]-v1[tc0];
  ey=v2[tc1]-v1[tc1];
  fx=act_cut[tc0]-v1[tc0];
  fy=act_cut[tc1]-v1[tc1];
  k1=ex*fy-ey*fx;
  ex=v0[tc0]-v2[tc0];
  ey=v0[tc1]-v2[tc1];
  fx=act_cut[tc0]-v2[tc0];
  fy=act_cut[tc1]-v2[tc1];
  k2=ex*fy-ey*fx;
  return (k0<0.0 && k1<0.0 && k2<0.0) || (k0>0.0 && k1>0.0 && k2>0.0);
}

bool hitTunnel(Tunnel *tunnel, Ray *ray, int *index, Vector cut, Vector n, double *t) {
  tact_ray=ray;
  tact_index=index;
  int g=tunnel->startgate;
  while (insideGate(g) && g<act_tunnel->gatecount-1) g++;
  if (g==0 || g>=act_tunnel->gatecount) return false; // there is no segment #0
  if (g<tunnel->mingate) tunnel->mingate=g;
  g--;

/*  (*index)=g+1;
  makeVec(cut,0.0,0.0,0.0);
  makeVec(n,0.0,1.0,0.0);
  return true; */
  
  TSegment *act_segment=&tunnel->segment[g];
  TFaceCache *fcache;
  TFace *p;
  double tri_hit_term,d;

  for (unsigned int i=0; i<act_segment->fcount; i++) {
    p=&act_tunnel->face[act_segment->foffs+i];
    fcache=&act_tunnel->fcache[act_segment->foffs+i];
    tc0=fcache->c0; tc1=fcache->c1; tc2=3-tc0-tc1;
    tri_hit_term=fcache->d;
    d=tact_ray->dir[0]*p->a+tact_ray->dir[1]*p->b+tact_ray->dir[2]*p->c;
    if (fabs(d)>EPSILON) {
      tri_hit_term/=-d;
      if (tri_hit_term>-0.000001) {
        act_cut[tc0]=tact_ray->p[tc0]+tri_hit_term*tact_ray->dir[tc0];
        act_cut[tc1]=tact_ray->p[tc1]+tri_hit_term*tact_ray->dir[tc1];
        if (insideTriangle(act_segment->foffs+i)) {
          (*index)=act_segment->foffs+i;
          act_cut[tc2]=tact_ray->p[tc2]+tri_hit_term*tact_ray->dir[tc2];
          vecCopy(cut,act_cut);
          makeVec(n,p->a,p->b,p->c);
          (*t)=tri_hit_term;
          return true;
        }
      }
    }
  }
  return false;
}

void killTunnel(Tunnel *tunnel) {
  delete [] tunnel->vertex;
  delete [] tunnel->face;
  delete [] tunnel->gate;
  delete [] tunnel->segment;
  delete [] tunnel->fcache;
  delete [] tunnel->gcache;
  delete [] tunnel->sv;
}

