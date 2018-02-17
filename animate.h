#ifndef _ANIMATEH
#define _ANIMATEH

#define FRAMERATE_REFRESH_TIME 0.5

typedef struct _Spinner {
  double freq;
  double value;
} Spinner;

extern unsigned int framecount;

void spinIt(Spinner *spinner);
void startTiming();
void doTiming();
int getFrameCount();
bool validFramerate();
double getFramerate();
double getTime();

#endif
