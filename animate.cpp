#include <math.h>
#include "animate.h"
#include "main.h"

unsigned int framecount,validframecount;
double validcounter,lastcounter,framerate,frametime,starttime;

void spinIt(Spinner *spinner) {
  spinner->value+=frametime*spinner->freq*PI2;
  while (spinner->value>PI2) spinner->value-=PI2;
}

void startTiming() {
  framecount=0;
  lastcounter=getCounter();
  validcounter=lastcounter;
  starttime=lastcounter;
  validframecount=0;
}

void doTiming() {
  double c=getCounter();
  frametime=c-lastcounter;
  lastcounter=c;
  framecount++;
}

int getFrameCount() {
  return framecount;
}

bool validFramerate() {
  double c=getCounter();
  if (c-validcounter>FRAMERATE_REFRESH_TIME) {
    framerate=(framecount-validframecount)/(c-validcounter);
    validcounter=c;
    validframecount=framecount;
    return true;
  }
  return false;
}

double getFramerate() {
  return framerate;
}

double getTime() {
  return getCounter()-starttime;
}

