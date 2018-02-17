#include <stdio.h>
#include "bsp.h"

BSPTree tree;

int main() {
  printf("attemptimg to load BSP tree...\n");
  loadBSPTree(&tree,"bsp.out");
  for (int i=0; i<35; i++) printf("vertex %i: [%1.16lf|%1.16lf|%1.16lf]\n",i,tree.vertex[i][0],tree.vertex[i][1],tree.vertex[i][2]);
  killBSPTree(&tree);
}

