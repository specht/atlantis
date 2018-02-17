#include "img.h"
#include <stdio.h>

void loadImage(void *buffer, char *filename, unsigned int size) {
  FILE *f;
  f=fopen(filename,"rb");
  fread(buffer,1,size,f);
  fclose(f);
}

