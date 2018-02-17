#include <stdio.h>

typedef struct _Node {
  unsigned char axis, coord;
  unsigned int col;
  unsigned int left, right;
} Node;

Node *coltree;

unsigned int mapColor(unsigned char r, unsigned char g, unsigned char b) {
  unsigned char col[3]={r,g,b};
  Node *p;
  p=coltree;
  while (!(p->left==-1&&p->right==-1)) p=(Node*)((unsigned char*)coltree+col[p->axis]<p->coord? p->left: p->right);
  return (unsigned int)p->col;
}

int main() {
  printf("size of node: %i\n",sizeof(Node));
}

