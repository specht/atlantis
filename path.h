#ifndef _PATHH
#define _PATHH

typedef struct _PathVertex {
  Vector v;
  double len;
} PathVertex;

typedef struct _Path {
  PathVertex *pv;
  unsigned int startspline;
  unsigned int splinecount;
  double travelled,length;
} Path;

void loadPath(Path *path, char *filename);
void killPath(Path *path);
void getCamera(Path *path, double s, Vector from, Vector at, Vector up);

#endif

