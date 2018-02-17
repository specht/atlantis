#include <windows.h>
#include <wincon.h>
#include <stdio.h>

const char temptitle[20]="Atlantis22092312";
HWND hwnd;
HANDLE console;
double freq1,starttime;
unsigned int count=0;

double getCounterFreq() {
  LARGE_INTEGER li;
  QueryPerformanceFrequency(&li);
  return (double)(li.HighPart*4294967296.0+li.LowPart);
}

double getCounter() {
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return ((double)(li.HighPart*4294967296.0+li.LowPart))*freq1;
}

void createConsole() {
  console=CreateConsoleScreenBuffer(GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
  SetConsoleActiveScreenBuffer(console);
  COORD c;
  c.X=80; c.Y=50;
  SetConsoleScreenBufferSize(console,c);
  SetConsoleMode(console,0);
  SetConsoleTitle(temptitle);
  hwnd=FindWindow(NULL,temptitle);
  SetConsoleTitle("Atlantis");
  ShowWindow(hwnd,SW_MAXIMIZE);
  CONSOLE_CURSOR_INFO cci;
  cci.bVisible=false;
  cci.dwSize=1;
  SetConsoleCursorInfo(console,&cci);
}

void flipBuffer(unsigned int* buf) {
  COORD a,b;
  SMALL_RECT r;
  a.X=80; a.Y=50;
  b.X=0; b.Y=0;
  r.Left=0; r.Top=0;
  r.Right=79; r.Bottom=49;
  WriteConsoleOutput(console,(CHAR_INFO*)buf,a,b,&r);
}

void drawTestScreen() {
  unsigned int c[4000];
  for (int i=0; i<4000; i++) c[i]=0x001b00b1;
  flipBuffer(c);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
  freq1=1.0/getCounterFreq();
  if (!AllocConsole()) return 0;
  createConsole();

  starttime=getCounter();
  while (1) {
    count++;
    drawTestScreen();
  }

  return 0;
}

