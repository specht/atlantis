#ifndef _MAINH
#define _MAINH
#include <windows.h>

#define EPSILON 0.00001
#define PI2 6.2831853

typedef unsigned char Color[4];
typedef unsigned int Buffer[4000];
typedef unsigned char ColorBuffer[4000][4];

extern unsigned int palette[4096];
extern HWND hwnd;

extern int daystogo;

void initConsole(HANDLE _console);
void initCounter();
double getCounter();
void loadColors();
unsigned int mapColor(unsigned char r, unsigned char g, unsigned char b);
void flipBuffer(unsigned int* buf);
void putString(unsigned int *buf, int x, int y, unsigned char col, char* s);
void fillBuf(unsigned int *buf, unsigned int value, unsigned int count);
void swap(int *a, int *b);
void swap(unsigned int *a, unsigned int *b);
void swap(double *a, double *b);

#endif

