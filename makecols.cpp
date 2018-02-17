#include <stdio.h>

unsigned char pal[1000][3];
unsigned int charinfo[1000];
int numcols=0;
unsigned int palette[16][16][16];

void makeCol(unsigned char *p, unsigned char r, unsigned char g,unsigned char b) {
  *p=r;
  *(unsigned char*)(p+1)=g;
  *(unsigned char*)(p+2)=b;
}

int main() {
  numcols=16;
  makeCol(pal[ 0],0,0,0);
  makeCol(pal[ 1],0,0,128);
  makeCol(pal[ 2],0,128,0);
  makeCol(pal[ 3],0,128,128);
  makeCol(pal[ 4],128,0,0);
  makeCol(pal[ 5],128,0,128);
  makeCol(pal[ 6],128,128,0);
  makeCol(pal[ 7],192,192,192);
  makeCol(pal[ 8],128,128,128);
  makeCol(pal[ 9],0,0,255);
  makeCol(pal[10],0,255,0);
  makeCol(pal[11],0,255,255);
  makeCol(pal[12],255,0,0);
  makeCol(pal[13],255,0,255);
  makeCol(pal[14],255,255,0);
  makeCol(pal[15],255,255,255);
  charinfo[ 0]=0x00000020;
  charinfo[ 1]=0x00100020;
  charinfo[ 2]=0x00200020;
  charinfo[ 3]=0x00300020;
  charinfo[ 4]=0x00400020;
  charinfo[ 5]=0x00500020;
  charinfo[ 6]=0x00600020;
  charinfo[ 7]=0x00700020;
  charinfo[ 8]=0x00800020;
  charinfo[ 9]=0x00900020;
  charinfo[10]=0x00a00020;
  charinfo[11]=0x00b00020;
  charinfo[12]=0x00c00020;
  charinfo[13]=0x00d00020;
  charinfo[14]=0x00e00020;
  charinfo[15]=0x00f00020;
  for (unsigned int i=0; i<16; i++) for (unsigned int k=i+1; k<16; k++) {
    for (unsigned int s=1; s<4; s++) {
      double ts=(double)s*0.25;
      if (ts>1.0) ts=1.0;
      double t=1.0-ts;
      makeCol(pal[numcols],(int)(ts*pal[i][0]+t*pal[k][0]),
                           (int)(ts*pal[i][1]+t*pal[k][1]),
                           (int)(ts*pal[i][2]+t*pal[k][2]));
      charinfo[numcols]=(i << 16) | (k << 20) | ((unsigned int)(0xb0+s-1));
      bool ok=true;
      for (int j=0; j<numcols && ok; j++) if (pal[j][0]==pal[numcols][0] &&
          pal[j][1]==pal[numcols][1] && pal[j][2]==pal[numcols][2]) ok=false;
      if (ok) numcols++;
    }
  }
  printf("number of colors: %i\n",numcols);
  for (int r=0; r<16; r++) for (int g=0; g<16; g++) for (int b=0; b<16; b++) {
    int red=r*255/15;
    int green=g*255/15;
    int blue=b*255/15;
    red=red*red/255;
    green=green*green/255;
    blue=blue*blue/255;
    int best=-1; int bestdist=1000000;
    for (int i=0; i<numcols; i++) {
      int dist=(pal[i][0]-red)*(pal[i][0]-red)
              +(pal[i][1]-green)*(pal[i][1]-green)
              +(pal[i][2]-blue)*(pal[i][2]-blue);
      if (dist<bestdist) {
        best=i;
        bestdist=dist;
      }
    }
    palette[b][g][r]=charinfo[best];
  }
  FILE *f;
  f=fopen("palette.dat","wb");
  fwrite(palette,4,4096,f);
  fclose(f);
}

