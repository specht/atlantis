#include <stdio.h>

typedef struct _Node {
  unsigned char axis, coord;
  unsigned int col;
  unsigned int left, right;
} Node;

int main() {
  printf("size is: %i\n",sizeof(Node));
  return;
}

