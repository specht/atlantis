#include <stdio.h>

unsigned int palette[300];
char s[10];

int main() {
  FILE *f;
  f=fopen("data.out","rb");
  for (int i=0; i<300; i++) {
    s[0]='0';
    s[1]='0';
    fread(s+2,1,8,f);
    s[8]=0;
    unsigned int v;
    sscanf(s,"%x",&v);
    palette[i]=((v>>24) & 0xFF)
    | (((v>>16) & 0xFF) << 8)
    | (((v>>8) & 0xFF) << 16)
    | ((v & 0xFF) << 24);
    palette[i]>>=8;
  }
  fclose(f);
  f=fopen("palette.dat","wb");
  fwrite(palette,4,300,f);
  fclose(f);
}

