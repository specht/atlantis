#include <stdio.h>

#define xsize 80
#define ysize 50

unsigned char img[xsize*ysize*3],mask[xsize*ysize],combined[xsize*ysize*4];

int main() {
  FILE *f;
  f=fopen("bimg.raw","rb");
  fread(&img[0],1,xsize*ysize*3,f);
  fclose(f);
  f=fopen("bmask.raw","rb");
  fread(&mask[0],1,xsize*ysize,f);
  fclose(f);
  for (unsigned int i=0; i<xsize*ysize; i++) {
    combined[i*4+0]=img[i*3+0];
    combined[i*4+1]=img[i*3+1];
    combined[i*4+2]=img[i*3+2];
    combined[i*4+3]=mask[i];
  }
  f=fopen("brought.ts","wb");
  fwrite(&combined[0],1,xsize*ysize*4,f);
  fclose(f);
}

