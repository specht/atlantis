#include <windows.h>
#include <wincon.h>
#include <stdio.h>
#include <dos.h>
#include "main.h"

#define MAX_DITHER 6337

double freq1;
HANDLE localconsole;
unsigned int palette[4096];
int dither[MAX_DITHER];
int ditheroffs=0;
int daystogo;

void initConsole(HANDLE _console) {
  localconsole=_console;
  date today;
  getdate(&today);
  if (today.da_mon==11) daystogo=41-today.da_day; else daystogo=11-today.da_day;
}

double getCounterFreq() {
  LARGE_INTEGER li;
  QueryPerformanceFrequency(&li);
  return (double)(li.HighPart*4294967296.0+li.LowPart);
}

void initCounter() {
  freq1=1.0/getCounterFreq();
}

double getCounter() {
  LARGE_INTEGER li;
  QueryPerformanceCounter(&li);
  return ((double)(li.HighPart*4294967296.0+li.LowPart))*freq1;
}

void loadColors() {
  FILE *f;
  f=fopen("data/palette.dat","rb");
  fread(palette,4,4096,f);
  fclose(f);
  for (int i=0; i<MAX_DITHER; i++) dither[i]=rand()*30/RAND_MAX-15;
//  for (int i=0; i<MAX_DITHER; i++) dither[i]=0;
}

unsigned int mapColor(unsigned char r, unsigned char g, unsigned char b) {
  int ir;
  asm {
    xor eax,eax
    mov al,[r]
    mov edx,ditheroffs
    
    inc edx
    cmp edx,MAX_DITHER
    jl @ok1
    xor edx,edx
  @ok1:

    add eax,[dither+edx*4]
    cmp eax,0
    jnl @ok2
    xor eax,eax
  @ok2:
    cmp ah,0
    je @ok3
    mov eax,255
  @ok3:
    mov [ir],eax

  }
  int ig=g, ib=b;
  ig+=dither[(ditheroffs++)%MAX_DITHER];
  ib+=dither[(ditheroffs++)%MAX_DITHER];
  ditheroffs%=MAX_DITHER;
  if (ig<0) ig=0;
  if (ib<0) ib=0;
  if (ig>255) ig=255;
  if (ib>255) ib=255;
  unsigned int col;
  asm {
    mov eax,[ib]
    shl eax,4
    and eax,0x00000F00
    or eax,[ig]
    mov ebx,[ir]
    and eax,0x00000FF0
    shr ebx,4
    or eax,ebx
    shl eax,2
    mov edx,[palette+eax]
    mov [col],edx
  }
  return col;
}

void flipBuffer(unsigned int* buf) {
  COORD a,b;
  SMALL_RECT r;
  a.X=80; a.Y=50;
  b.X=0; b.Y=0;
  r.Left=0; r.Top=0;
  r.Right=79; r.Bottom=49;
  WriteConsoleOutput(localconsole,(CHAR_INFO*)buf,a,b,&r);
}

void putString(unsigned int *buf, int x, int y, unsigned char col, char *s) {
  unsigned int offs=y*80+x;
  while (*s) *(unsigned int*)(buf+(offs++))=(unsigned int)(col)<<16|(*(s++));
}

void fillBuf(unsigned int *buf, unsigned int value, unsigned int count) {
  asm {
    mov eax,[value]
    mov edi,[buf]
    mov ecx,[count]
    rep stosd
  }
}

void swap(int *a, int *b) {
  int t;
  t=*a;
  *a=*b;
  *b=t;
}

void swap(unsigned int *a, unsigned int *b) {
  unsigned int t;
  t=*a;
  *a=*b;
  *b=t;
}

void swap(double *a, double *b) {
  double t;
  t=*a;
  *a=*b;
  *b=t;
}

