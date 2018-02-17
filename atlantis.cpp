#include <windows.h>
#include <wincon.h>
#include "main.h"
#include "demo.h"

const char temptitle[20]="Atlantis22092312";
HWND hwnd;
HANDLE console;

void createConsole() {
  console=CreateConsoleScreenBuffer(GENERIC_WRITE,0,NULL,CONSOLE_TEXTMODE_BUFFER,NULL);
  SetConsoleActiveScreenBuffer(console);
  COORD c;
  c.X=80; c.Y=50;
  SetConsoleScreenBufferSize(console,c);
  SetConsoleMode(console,0);
  SetConsoleTitle(temptitle);
  hwnd=FindWindow(NULL,temptitle);
  SetConsoleTitle("Atlantis (press Alt-Enter to see fullscreen)");
  ShowWindow(hwnd,SW_SHOW);
  CONSOLE_CURSOR_INFO cci;
  cci.bVisible=false;
  cci.dwSize=1;
  SetConsoleCursorInfo(console,&cci);
}

int main() {
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
//  AllocConsole();
  createConsole();
  initConsole(console);

  initCounter();
  loadColors();
  
  runDemo();

  return 0;
}

