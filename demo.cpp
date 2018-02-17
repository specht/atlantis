#include <stdio.h>
#include <dos.h>
#include <math.h>
#include <conio.h>
#include "demo.h"
#include "main.h"
#include "camera.h"
#include "objects.h"
#include "animate.h"
#include "overlay.h"
#include "bsp.h"
#include "lights.h"
#include "tunnel.h"
#include "path.h"
#include "img.h"

#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>

const int numlayers=8;
int aalevel=1;
ColorBuffer framebuf;
Buffer charbuf,idbuf;
unsigned char *martin,*daniel,*mischa,*fussel,*pclogo,*title,*brought;
extern Plane frustum[4];

Camera cam;
Floor water,sky,inwater;
Sphere moon;
BSPTree birds,ruins,cave;
Spinner oceanripple={0.28,0.0}, cameraspin={0.17,1.5}, spotspin={0.3,0.0};
Color fadecolor={140,159,204,0};
Spot spot;
Tunnel tunnel,tunnel2;
Path path;
FSOUND_STREAM *stream;
int channel;
double activeframerate=0.0;
double GV_sx, GV_sy;
int phase=0;
const int numkeyframes=8;
double keyframes[numkeyframes]={0.0,17.3,34.0,47.7,63.0,76.56,111.0,999999.0};
const int numvkeys=14;
double vkeys[numvkeys][2]={{0.0,6.2543},{17.3,6.2543},{17.301,4.856},{34.0,4.856},
                           {34.001,8.737},{47.7,8.737},{47.701,11.96},{63.0,11.96},
                           {63.001,8.2929},{77.0,8.2929},{77.001,13.85},{89.0,13.85},{100.0,0.0},{10000.0,0.0}};
char phasetitle[numkeyframes][30]={"  Ocean","  Diving","  Ruins","  Tunnel 1","   Cave","   Tunnel 2","   Outside","     End"};
double ssum=0.0;
int vkeyoffs=0;
double demotime=0.0;
Floor layers[numlayers],ground;
double layerstransform[numlayers][2];
bool quit=false;

void initSound() {
  if (FSOUND_GetVersion()<FMOD_VERSION)	exit(1);
  if (!FSOUND_Init(44100, 32, 0)) exit(1);
  FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
  stream=FSOUND_Stream_OpenFile("data/atlantis.mp3",FSOUND_NORMAL|FSOUND_MPEGACCURATE,0);
  if (!stream) exit(1);
  channel=FSOUND_Stream_PlayEx(FSOUND_FREE,stream,NULL,TRUE);
}

void playSound() {
  FSOUND_SetPaused(channel,false);
}

void killSound() {
  FSOUND_Stream_Close(stream);
  FSOUND_Close();
}

void waveTexture(Color c, Vector v, double fade) {
  if (phase>4) vecMul(v,v,20.0); else vecMul(v,v,7.0);
  double n=sin(v[0]*cos(v[1]-sin(v[2]+oceanripple.value)))*0.5+0.5;
  fade=sqrt(fade);
  n=(fade*n)+(0.5*(1.0-n));
//  n=n*n;

  c[0]=(unsigned char)(n*100.0);
  c[1]=(unsigned char)(n*180.0);
  c[2]=(unsigned char)(n*234.0);
}

void groundTexture(Color c, Vector v, double fade) {
  vecMul(v,v,0.9);
  double n=sin(v[0])*sin(v[2])*0.5+0.5;
  vecMul(v,v,1.4);
  n=n*(sin(v[0]+cos(v[2]*sin(v[0])))*0.5+0.5);
  n=1.0-n*0.6;
//  n=n*n*n*n;
  //  double n=1.0;

  c[0]=(unsigned char)(n*156.0);
  c[1]=(unsigned char)(n*213.0);
  c[2]=(unsigned char)(n*234.0);
/*  double x=v[0]*0.2, y=v[1]*0.2, z=v[2]*0.2;
  x=fmod(fabs(x),1.0);
  y=fmod(fabs(y),1.0);
  z=fmod(fabs(z),1.0);
  if (y<0.5 && x<0.2 || z<0.4) {
    c[0]=0;
    c[1]=0;
    c[2]=0;
  } */
}

void wallTexture(Color c, Vector v, double fade) {
  c[0]=80;
  c[1]=47;
  c[2]=13;
  double x=v[0], y=v[1], z=v[2];
  x=fmod(fabs(x),1.0);
  y=fmod(fabs(y),1.0);
  z=fmod(fabs(z),1.0);
  if (y<0.5 && x<0.8 || z<0.4) {
    c[0]=255;
    c[1]=255;
    c[2]=255;
  }
}

void ruinTexture(Color c, Vector v, double fade) {
  c[0]=80;
  c[1]=47;
  c[2]=13;
  double x=v[0], y=v[1], z=v[2];
  x=fmod(fabs(x),1.0);
  y=fmod(fabs(y),1.0);
  z=fmod(fabs(z),1.0);
if(1){//  if (y>0.3 && x>0.3 || z>0.3) {
    c[0]=212;
    c[1]=212;
    c[2]=212;
  }
}

void makeRandomColor(Color res, int i) {
  res[0]=(i*44+60)%256;
  res[1]=(i*72+43)%256;
  res[2]=(i*111+27)%256;
}

bool renderBSPModel(BSPTree *bsp, Color c, Ray *ray, int *which) {
  Vector cut,n;
  int w;
  if (hitBSPTree(bsp,ray,&w,cut,n)) {
    Color tempcol;
//    makeRandomColor(tempcol,w);
//    wallTexture(tempcol,cut,1.0);
    tempcol[0]=255;
    tempcol[1]=255;
    tempcol[2]=255;
    if (bsp==&cave) {
      tempcol[0]=50;
      tempcol[1]=90;
      tempcol[2]=192;
    }
    if (phase==2) ruinTexture(tempcol,cut,1.0);
    double f,g;
    if (phase==0) f=1.0; else {
      f=-dotPrd(spot.dir,n);
      f=fabs(f);

      Vector l;
      vecSub(l,cut,spot.p);
      normalize(l);
      g=dotPrd(l,spot.dir);
      if (phase==2) g=g*g*g*g*g; else g=g*g*g*g*g;
      if (g<0.0) g=0.0;
//      f=f*g;

      if (f<0.0) f=0.0;
      if (bsp==&cave) f=f*0.7+0.3; else f=f*0.95+0.05;
      
    }

    if (bsp==&cave) {
      g*=0.7;
      c[0]=tempcol[0]*f*(1-g)+255.0*g;
      c[1]=tempcol[1]*f*(1-g)+255.0*g;
      c[2]=tempcol[2]*f*(1-g)+255.0*g;
    } else {
      c[0]=tempcol[0]*f;
      c[1]=tempcol[1]*f;
      c[2]=tempcol[2]*f;
    }
    (*which)=12002+w;

    if (phase==2) if (demotime<35.0) {
      double attenuation=demotime-34.0;
      c[0]=attenuation*c[0]+(1.0-attenuation)*fadecolor[0];
      c[1]=attenuation*c[1]+(1.0-attenuation)*fadecolor[1];
      c[2]=attenuation*c[2]+(1.0-attenuation)*fadecolor[2];
    }
    return true;
  } else return false;
}

unsigned int getColor(double x, double y, Color c) {
/*  double f=x/80.0;
  unsigned char gray=(int)(f*f*256.0);
  c[0]=gray;
  c[1]=gray;
  c[2]=gray;
  return 0; */
  Ray ray;
  vecCopy(ray.p,cam.from);
  Vector vx,vy;
  double sx=(x-width2)*cam.xsize,
         sy=(y-height2)*cam.ysize;
  GV_sx=sx; GV_sy=sy;
  vecMul(vx,cam.xvec,sx);
  vecMul(vy,cam.yvec,sy);
  vecAdd(ray.dir,vx,vy);
  vecSub(ray.dir,ray.dir,cam.zvec);
  normalize(ray.dir);
  for (int i=0; i<3; i++) if (fabs(ray.dir[i])<EPSILON) ray.dir_1[i]=1.0;
                        else ray.dir_1[i]=1.0/ray.dir[i];
  
  double t;

  int which;
  Vector cut,n;
  double dist;
  if (phase==1) {
    int ix,iy;
    unsigned char *img=NULL;
    if (demotime>22.925 && demotime<24.8) {
      double pos=0.0;
      if (demotime<23.125) pos=(23.125-demotime)*450.0;
      if (demotime>24.6) pos=(demotime-24.6)*450.0;
      img=daniel;
      ix=(int)(GV_sx*90.0+20.0-pos);
      iy=(int)(GV_sy*90.0+60.0-pos);  // Daniel
    }
    if (demotime>24.8 && demotime<26.675) {
      double pos=0.0;
      if (demotime<25.0) pos=(25.0-demotime)*450.0;
      if (demotime>26.475) pos=(demotime-26.475)*450.0;
      img=mischa;
      ix=(int)(GV_sx*90.0+60.0);
      iy=(int)(GV_sy*90.0+70.0-pos);  // Mischa
    }
    if (demotime>26.675 && demotime<28.55) {
      double pos=0.0;
      if (demotime<26.875) pos=(26.875-demotime)*450.0;
      if (demotime>28.35) pos=(demotime-28.35)*450.0;
      img=martin;
      ix=(int)(GV_sx*100.0+100.0+pos);
      iy=(int)(GV_sy*100.0+70.0-pos);  // Martin
    }
    if (img!=NULL) if (ix>=0 && ix<128 && iy>=0 && iy<128) {
      unsigned char mask;
      unsigned int offs=iy*128+ix;
      memcpy(&mask,&img[offs*4+3],1);
      if (mask>127) {
        memcpy(c,&img[offs*4],3);
        return 0x20000000+offs;
      }
    }
    
    hitFloor(&ground,&ray,&t);
    Color tex;
    Vector cut;
    vecMul(cut,ray.dir,t);
    vecAdd(cut,cut,ray.p);
    double attenuation;
    if (t>555.0) attenuation=0.0; else attenuation=1.0/exp(t*0.05);
    double r=(double)120.0*attenuation+fadecolor[0]*(1.0-attenuation);
    double g=(double)192.0*attenuation+fadecolor[1]*(1.0-attenuation);
    double b=(double)222.0*attenuation+fadecolor[2]*(1.0-attenuation);
    if (r<0.0) r=0.0;
    if (g<0.0) g=0.0;
    if (b<0.0) b=0.0;
    c[0]=(unsigned char)r;
    c[1]=(unsigned char)g;
    c[2]=(unsigned char)b;

    double f=0.0;
    for (int i=0; i<numlayers; i++) if (hitFloor(&layers[i],&ray,&t)) {
      vecMul(cut,ray.dir,t);
      double fade=-dotPrd(cam.zvec,cut);
      vecAdd(cut,cut,ray.p);
      unsigned char ch;
      int ix,iy;
      double cx,cy;
      cx=cut[0]*layerstransform[i][0]-cut[2]*layerstransform[i][1];
      cy=-cut[0]*layerstransform[i][0]+cut[2]*layerstransform[i][1];
      cx=cut[0]; cy=cut[2];
      ix=cx*500.0;
      iy=cy*390.0;
      while (ix<0) ix+=512;
      while (iy<0) iy+=512;
      ix%=512; iy%=512;
      unsigned int offs=iy*512+ix;
      memcpy(&ch,&fussel[offs],1);
      if (ch>127) {
        fade=fade*0.1;
        if (fade>1.0) fade=1.0;
        fade=1.0-fade;
/*        c[0]=(unsigned char)(255.0*fade);
        c[1]=(unsigned char)(255.0*fade);
        c[2]=(unsigned char)(255.0*fade);
        return 0x30000000+offs; */
        f+=fade*0.8;
      }
    }
    if (f>1.0) f=1.0;
    if (demotime>30.0) f*=(34.0-demotime)*0.25;
    c[0]=255.0*f+c[0]*(1.0-f);
    c[1]=255.0*f+c[1]*(1.0-f);
    c[2]=255.0*f+c[2]*(1.0-f);
    return 0;
  }
  if (phase==2 || phase==3) if (hitTunnel(&tunnel,&ray,&which,cut,n,&dist)) {
    Color tempcol;
    wallTexture(tempcol,cut,1.0);
    double f=-dotPrd(spot.dir,n);

/*    Vector q;
    vecSub(q,cut,spot.p);
    double qf=dotPrd(q,spot.dir);
    qf*=0.01;
    qf=1.0-qf;
    if (qf<0.0) qf=0.0;
    f*=qf; */

    Vector t;
    vecSub(t,cut,cam.from);
    double g=-dotPrd(cam.zvec,t);
    if (g>1111.0) g=0.0; else g=1.0/exp(g*0.08);
    f*=g;


    if (f<0.0) f=0.0;
    f=f*0.8+0.2;
//    f=1.0;

    c[0]=(unsigned char)((double)tempcol[0]*f);
    c[1]=(unsigned char)((double)tempcol[1]*f);
    c[2]=(unsigned char)((double)tempcol[2]*f);
    return 2002+which;
  }
  if (phase==4 || phase==5) if (hitTunnel(&tunnel2,&ray,&which,cut,n,&dist)) {
    Color tempcol;
    wallTexture(tempcol,cut,1.0);
    double f=-dotPrd(spot.dir,n);

    Vector q;
    vecSub(q,cut,spot.p);
    double qf=dotPrd(q,spot.dir);
    qf*=0.01;
    qf=1.0-qf;
    if (qf<0.0) qf=0.0;
    f*=qf;

    Vector t;
    vecSub(t,cut,cam.from);
    double g=-dotPrd(cam.zvec,t);
    if (g>1111.0) g=0.0; else g=1.0/exp(g*0.08);
    f*=g;


    if (f<0.0) f=0.0;
    f=f*0.73+0.22;
//    f=1.0;


    c[0]=(unsigned char)((double)tempcol[0]*f);
    c[1]=(unsigned char)((double)tempcol[1]*f);
    c[2]=(unsigned char)((double)tempcol[2]*f);
    return 2002+which;
  }
/*  c[0]=0;
  c[1]=0;
  c[2]=0;
  return 1; */
  
  if (phase==0) if (renderBSPModel(&birds,c,&ray,&which)) return which;
  if (phase==2) if (renderBSPModel(&ruins,c,&ray,&which)) return which;
  if (phase==3 || phase==4) if (renderBSPModel(&cave,c,&ray,&which)) return which;
  
  if (phase==0 || phase==5 || phase==6) if (testSphere(&moon,&ray)) {
    c[0]=255;
    c[1]=255;
    c[2]=192;
    return 1;
  }

  if (phase==0 || phase==5 || phase==6) if (hitFloor(&sky,&ray,&t)) {
    Vector cut;
    vecMul(cut,ray.dir,t);
    vecAdd(cut,cut,ray.p);
    double attenuation;
    if (t>10000000.0) attenuation=0.0; else attenuation=1.0/exp(t*0.00004);
    if (phase>=5) attenuation=1.0-attenuation;
    double r=(double)58.0*attenuation+fadecolor[0]*(1.0-attenuation);
    double g=(double)131.0*attenuation+fadecolor[1]*(1.0-attenuation);
    double b=(double)165.0*attenuation+fadecolor[2]*(1.0-attenuation);
    if (r<0.0) r=0.0;
    if (g<0.0) g=0.0;
    if (b<0.0) b=0.0;
    c[0]=(unsigned char)r;
    c[1]=(unsigned char)g;
    c[2]=(unsigned char)b;
    return 0x10000000;
  }
  
/*  if (phase==4) if (hitFloor(&inwater,&ray,&t)) {
    Vector cut;
    vecMul(cut,ray.dir,t);
    vecAdd(cut,cut,ray.p);
    double attenuation;
    if (t>10000000.0) attenuation=0.0; else attenuation=1.0/exp(t*0.00004);
    double r=(double)58.0*attenuation+fadecolor[0]*(1.0-attenuation);
    double g=(double)131.0*attenuation+fadecolor[1]*(1.0-attenuation);
    double b=(double)165.0*attenuation+fadecolor[2]*(1.0-attenuation);
    if (r<0.0) r=0.0;
    if (g<0.0) g=0.0;
    if (b<0.0) b=0.0;
    c[0]=(unsigned char)r;
    c[1]=(unsigned char)g;
    c[2]=(unsigned char)b;
    return 0x10000000;
  } */

  if (phase==0 || phase>4) if (hitFloor(&water,&ray,&t)) {
    Color tex;
    Vector cut;
    vecMul(cut,ray.dir,t);
    vecAdd(cut,cut,ray.p);
    double attenuation;
    if (t>1111.0) attenuation=0.0; else attenuation=1.0/exp(t*0.005);
    waveTexture(tex,cut,attenuation);
//    attenuation=attenuation*0.6+0.4;
    if (phase>4) attenuation*=0.6;
    double r=(double)tex[0]*attenuation+fadecolor[0]*(1.0-attenuation);
    double g=(double)tex[1]*attenuation+fadecolor[1]*(1.0-attenuation);
    double b=(double)tex[2]*attenuation+fadecolor[2]*(1.0-attenuation);
    if (r<0.0) r=0.0;
    if (g<0.0) g=0.0;
    if (b<0.0) b=0.0;
    c[0]=(unsigned char)r;
    c[1]=(unsigned char)g;
    c[2]=(unsigned char)b;
    return 0x10000001;
  }
  
  if (phase==1 || phase==2) if (hitFloor(&ground,&ray,&t)) {
    Color tex;
    Vector cut;
    vecMul(cut,ray.dir,t);
    vecAdd(cut,cut,ray.p);
    double attenuation;
    if (t>555.0) attenuation=0.0; else attenuation=1.0/exp(t*0.05);
    groundTexture(tex,cut,attenuation); // 120,192,222
//    attenuation=attenuation*0.6+0.4;
    double r=(double)tex[0]*attenuation+fadecolor[0]*(1.0-attenuation);
    double g=(double)tex[1]*attenuation+fadecolor[1]*(1.0-attenuation);
    double b=(double)tex[2]*attenuation+fadecolor[2]*(1.0-attenuation);
    if (r<0.0) r=0.0;
    if (g<0.0) g=0.0;
    if (b<0.0) b=0.0;
    c[0]=(unsigned char)r;
    c[1]=(unsigned char)g;
    c[2]=(unsigned char)b;
    return 0x10000001;
  }
  if (phase==2) {
    memcpy(c,fadecolor,3);
    return 0;
  }
  memcpy(c,fadecolor,3);
  return 0;
}

bool sameId(unsigned int a, unsigned int b, unsigned int c, unsigned int d) {
  if (a!=b) return false;
  if (a!=c) return false;
  if (a!=d) return false;
  if (b!=c) return false;
  if (b!=d) return false;
  if (c!=d) return false;
  return true;
}

void mixCols(unsigned int res, unsigned int a, unsigned int b) {
  framebuf[res][0]=((unsigned int)framebuf[a][0]+framebuf[b][0])>>1;
  framebuf[res][1]=((unsigned int)framebuf[a][1]+framebuf[b][1])>>1;
  framebuf[res][2]=((unsigned int)framebuf[a][2]+framebuf[b][2])>>1;
  idbuf[res]=idbuf[a];
}

void mix4Cols(Color c, Color c0, Color c1, Color c2, Color c3) {
  c[0]=((unsigned int)c0[0]+c1[0]+c2[0]+c3[0])>>2;
  c[1]=((unsigned int)c0[1]+c1[1]+c2[1]+c3[1])>>2;
  c[2]=((unsigned int)c0[2]+c1[2]+c2[2]+c3[2])>>2;
}

bool colsClose(Color c0, Color c1) {
  if (c0[0]>>4 != c1[0]>>4) return false;
  if (c0[1]>>4 != c1[1]>>4) return false;
  if (c0[2]>>4 != c1[2]>>4) return false;
  return true;
}

bool colsDiff(Color c0, Color c1, Color c2, Color c3) {   // sind vier Farben unterschiedlich?
  if (!colsClose(c0,c1)) return true;
  if (!colsClose(c0,c2)) return true;
  if (!colsClose(c0,c3)) return true;
  if (!colsClose(c1,c2)) return true;
  if (!colsClose(c1,c3)) return true;
  if (!colsClose(c2,c3)) return true;
  return false;
}

void recurse(double x, double y, double dx, double dy,   // adaptives Antiaslising...
             Color res, Color c0, Color c1, Color c2, Color c3, int level) {
  if (level==0 || (!colsDiff(c0,c1,c2,c3))) {
    mix4Cols(res,c0,c1,c2,c3);
    return;
  }
  Color n,s,w,e,m,r;
  double dx2=dx*0.5, dy2=dy*0.5;
  getColor(x+dx2,y,n);
  getColor(x,y+dy2,w);
  getColor(x+dx2,y+dy2,m);
  getColor(x+dx,y+dy2,e);
  getColor(x+dx2,y+dy,s);
  recurse(x,y,dx2,dy2,r,c0,n,w,m,level-1);
  memcpy(c0,r,3);
  recurse(x+dx2,y,dx2,dy2,r,n,c1,m,e,level-1);
  memcpy(c1,r,3);
  recurse(x,y+dy2,dx2,dy2,r,w,m,c2,s,level-1);
  memcpy(c2,r,3);
  recurse(x+dx2,y+dy2,dx2,dy2,r,m,e,s,c3,level-1);
  memcpy(c3,r,3);
  mix4Cols(res,c0,c1,c2,c3);
}

void renderFrame() {
  fillBuf(idbuf,-1,4000);
  unsigned int offs=480;
  for (int y=0; y<38; y+=2, offs+=80) for (int x=0; x<80; x+=2, offs+=2) {
    Color col;
    idbuf[offs]=getColor(x,y,framebuf[offs]);
    if (x) {
      if (idbuf[offs-2]==idbuf[offs]) mixCols(offs-1,offs-2,offs);
        else getColor(x-1,y,framebuf[offs-1]);
    }
    if (y) {
      if (idbuf[offs]==idbuf[offs-160]) mixCols(offs-80,offs-160,offs);
        else idbuf[offs-80]=getColor(x,y-1,framebuf[offs-80]);
      if (x) if (sameId(idbuf[offs-162],idbuf[offs-160],idbuf[offs-2],idbuf[offs]))
        mixCols(offs-81,offs-1,offs-161);
          else idbuf[offs-81]=getColor(x-1,y-1,framebuf[offs-81]);
    }
  }
  offs=559;
  for (int y=0; y<38; y++, offs+=80) idbuf[offs]=getColor(79,y,framebuf[offs]);

  offs=561;
  for (int y=1; y<36; y++, offs+=2) for (int x=1; x<79; x++, offs++) {
    if (!sameId(idbuf[offs],idbuf[offs+1],idbuf[offs+80],idbuf[offs+81])) {
      Color c0,c1,c2,c3;
      memcpy(c0,framebuf[offs],3);
      memcpy(c1,framebuf[offs+1],3);
      memcpy(c2,framebuf[offs+80],3);
      memcpy(c3,framebuf[offs+81],3);
      recurse(x,y,1.0,1.0,framebuf[offs],c0,c1,c2,c3,aalevel);
    }
  }
  offs=480;
  for (int y=0; y<38; y++, offs+=80) {
    framebuf[offs][0]=0;
    framebuf[offs][1]=0;
    framebuf[offs][2]=0;
    framebuf[offs+79][0]=0;
    framebuf[offs+79][1]=0;
    framebuf[offs+79][2]=0;
  }
  if (phase==0) {
    if (demotime>2.0 && demotime<6.0) {
      double ovr;
      if (demotime<3.0) ovr=demotime-2.0; else
      if (demotime>5.0) ovr=6.0-demotime; else
      ovr=1.0;
      overlayScreen((unsigned char*)&framebuf,pclogo,(unsigned char)(ovr*255));
    }
    if (demotime>7.0 && demotime<13.0) {
      double ovr;
      if (demotime<8.0) ovr=demotime-7.0; else
      if (demotime>12.0) ovr=13.0-demotime; else
      ovr=1.0;
      overlayScreen((unsigned char*)&framebuf,title,(unsigned char)(ovr*255));
    }
  }
  if (phase==1) {
    if (demotime>19.175 && demotime<22.925) {
      double f=22.925-demotime;
      if (f>1.0) f=1.0;
      overlayScreen((unsigned char*)&framebuf,brought,(unsigned char)(f*255.0));
    }
  }
/*  double ovr=0.5+cos(daytime)*0.5;
  ovr*=ovr;
  overlayScreen((unsigned char*)&framebuf,title,(unsigned char)(ovr*255)); */
//  overlayScreen((void*)framebuf,(void*)title,255);

  unsigned char globalfade=255;
  if (demotime<2.0) globalfade=255.0/2.0*demotime;
  if (demotime>107.0) globalfade=(111.0-demotime)*0.25*255;
  if (globalfade<255) {
    for (int i=480; i<3440; i++) {
      framebuf[i][0]=(unsigned short)framebuf[i][0]*globalfade>>8;
      framebuf[i][1]=(unsigned short)framebuf[i][1]*globalfade>>8;
      framebuf[i][2]=(unsigned short)framebuf[i][2]*globalfade>>8;
    }
  }
  for (int i=480; i<560; i++) charbuf[i]=mapColor(framebuf[i][0]>>1,framebuf[i][1]>>1,framebuf[i][2]>>1);
  for (int i=560; i<3360; i++) charbuf[i]=mapColor(framebuf[i][0],framebuf[i][1],framebuf[i][2]);
  for (int i=3360; i<3440; i++) charbuf[i]=mapColor(framebuf[i][0]>>1,framebuf[i][1]>>1,framebuf[i][2]>>1);
}

void runDemo() {
  initSound();
  startTiming();
  demotime=getTime();
  daniel=new unsigned char[65536];
  mischa=new unsigned char[65536];
  martin=new unsigned char[65536];
  fussel=new unsigned char[512*512];
  pclogo=new unsigned char[16000];
  title=new unsigned char[16000];
  brought=new unsigned char[16000];
  loadImage(fussel,"data/fussel.raw",512*512);
  loadImage(daniel,"data/daniel.ts",65536);
  loadImage(mischa,"data/mischa.ts",65536);
  loadImage(martin,"data/toony.ts",65536);
  loadImage(pclogo,"data/pc.ts",16000);
  loadImage(title,"data/title.ts",16000);
  loadImage(brought,"data/brought.ts",16000);
  loadBSPTree(&birds,"data/birds.out");
  loadBSPTree(&ruins,"data/ruins.out");
  loadBSPTree(&cave,"data/cave.out");
  initCamera(&cam);
  memset(charbuf,0,16000);
  memset(framebuf,0,16000);

  loadTunnel(&tunnel,"data/tunnel1.out");
  loadTunnel(&tunnel2,"data/tunnel2.out");
  loadPath(&path,"data/camera.out");

  initFloor(&water,100.0);
  initFloor(&sky,10000.0);
  initFloor(&ground,0.0);
  initFloor(&inwater,-85.0);
  initSphere(&moon,5000.0,2500.0,-10000.0,700.0);
  flipBuffer(charbuf);
  while (!kbhit() && getTime()-demotime<3.0);
  playSound();
  startTiming();
  double t=0.0;
  Vector from,at,up;
  phase=0; vkeyoffs=0;
  ssum=0.0;
  while (!kbhit() && !quit) {
    demotime=FSOUND_Stream_GetTime(stream)/1000.0;
    if (demotime>111.0) quit=true;
//    demotime=getTime()+13.0;
    while (demotime>keyframes[phase+1]) {
      phase++;
      if (phase==4) initFloor(&water,-10.0);
    }
    while (demotime>vkeys[vkeyoffs+1][0]) {
      ssum+=(vkeys[vkeyoffs+1][1]+vkeys[vkeyoffs][1])*(vkeys[vkeyoffs+1][0]-vkeys[vkeyoffs][0])*0.5;
      vkeyoffs++;
    }
    double thist=(demotime-vkeys[vkeyoffs][0])/(vkeys[vkeyoffs+1][0]-vkeys[vkeyoffs][0]);
    double thiss=((vkeys[vkeyoffs+1][1]-vkeys[vkeyoffs][1])*thist+vkeys[vkeyoffs][1]+vkeys[vkeyoffs][1])*(vkeys[vkeyoffs+1][0]-vkeys[vkeyoffs][0])*0.5*thist;
    t=ssum+thiss;
    getCamera(&path,t,from,at,up);
    if (demotime>44.4 && demotime<47.5) {
      double angle=(47.5-demotime)/3.1*M_PI;
      makeVec(at,0.0,-sin(angle)*3.0,0.0);
    }
    putCamera(&cam,from,at,up);
    Vector a,a2,b;
    vecMul(a,cam.xvec,cos(spotspin.value)*0.9);
    vecMul(a2,cam.yvec,sin(spotspin.value)*0.4);
    vecAdd(a,a,cam.from);
    vecAdd(a,a,a2);
    vecSub(b,cam.at,a);
    normalize(b);
    initSpot(&spot,a,b,0.5);
    if (phase==0) {
      fadecolor[0]=208;
      fadecolor[1]=234;
      fadecolor[2]=243;
    }
    if (phase==1) {
      fadecolor[0]=(unsigned char)(157.0*from[1]*0.01+32.0);
      fadecolor[1]=(unsigned char)(91.0*from[1]*0.01+100.0);
      fadecolor[2]=(unsigned char)(68.0*from[1]*0.01+160.0);
      int startheight=2*(int)(cam.from[1]*0.5);
      for (int i=0; i<numlayers; i++) {
        initFloor(&layers[i],(double)startheight-2.0*i);
        layerstransform[i][0]=cos((double)(startheight-2.0*i)*from[1]*10.0);
        layerstransform[i][1]=sin((double)(startheight-2.0*i)*from[1]*10.0);
      }
    }
    if (phase==2) {
      fadecolor[0]=30.0;
      fadecolor[1]=100.0;
      fadecolor[2]=160.0;
      tunnel.startgate=0;
      tunnel.mingate=0;
    }
    if (phase==2 || phase==3) {
      prepareTunnel(&tunnel);
    }
    if (phase==3) {
      fadecolor[0]=30.0;
      fadecolor[1]=50.0;
      fadecolor[2]=100.0;
    }
    if (phase==4) {
      tunnel2.startgate=0;
      tunnel2.mingate=0;
    }
    if (phase==4 || phase==5) {
      prepareTunnel(&tunnel2);
    }
    if (phase==5 || phase==6) {
      fadecolor[0]=0;
      fadecolor[1]=0;
      fadecolor[2]=0;
    }

    aalevel=1;
    if (phase==2) aalevel=0;
    renderFrame();

    doTiming();
    spinIt(&oceanripple);
    spinIt(&cameraspin);
    spinIt(&spotspin);

    if (validFramerate()) activeframerate=getFramerate();
    
/*    char s[200];
    sprintf(s,"Atlantis TMDC Entry | %i days to go | frame rate: %7.2lf fps | time: %7.2lf",daystogo,activeframerate,demotime);
    putString(charbuf,1,1,0x07,s);
    sprintf(s,"startspline=%4d   startgate=%4d   distance travelled: %4.2lf m",path.startspline,tunnel2.startgate,t);
    putString(charbuf,1,2,0x07,s);
    putString(charbuf,77-strlen(phasetitle[phase]),2,0x0B,phasetitle[phase]); */

    flipBuffer(charbuf);
  }
  killPath(&path);
  killTunnel(&tunnel);
  killTunnel(&tunnel2);
  killBSPTree(&birds);
  killBSPTree(&ruins);
  killBSPTree(&cave);
  killSound();
  delete [] daniel;
  delete [] mischa;
  delete [] fussel;
  delete [] pclogo;
  delete [] title;
  delete [] brought;
  delete [] martin;
}

