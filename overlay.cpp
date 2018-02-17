#include "overlay.h"
#include <stdio.h>

void loadOverlay(unsigned char *scr, char *filename) {
  FILE *f;
  f=fopen(filename,"rb");
  fread(scr,4,4000,f);
  fclose(f);
}

void overlayScreen(void *framebuf, void *scr, unsigned char opacity) {
  if (opacity==0) return;
  unsigned int count;
  asm {
    mov edi,[scr]
    mov ecx,[framebuf]
    add edi,1920
    add ecx,1920
    mov [count],3040
  @overlay:

    mov eax,[edi]
    mov ebx,eax
    shr eax,24
    xor edx,edx
    mov dl,[opacity]
    mul dx
    mov al,ah
    ror eax,8

    mov edx,[ecx]
    push ecx
    and edx,0x00ffffff

// edx - framebuffer pixel
// ebx - overlay pixel
// eax - oo0000oo opacity

// ----------------------
    not al

    mul dl
    mov dl,ah

    rol eax,8
    mov ah,al
    ror eax,8

    mul bl
    add dl,ah

    rol eax,8
    mov ah,al
    ror eax,8

    ror edx,8
    shr ebx,8

// ----------------------
    not al
    mul dl
    mov dl,ah

    rol eax,8
    mov ah,al
    ror eax,8

    mul bl
    add dl,ah

    rol eax,8
    mov ah,al
    ror eax,8

    ror edx,8
    shr ebx,8

// ----------------------
    not al
    mul dl
    mov dl,ah

    rol eax,8
    mov ah,al
    ror eax,8

    mul bl
    add dl,ah

    ror edx,16

    pop ecx
    mov [ecx],edx

    add edi,4
    add ecx,4

    dec [count]
    jnz @overlay

  }
}

/*void overlayScreen(void *framebuf, void *scr, unsigned char opacity) {
  if (opacity==0) return;
  unsigned int count;
  asm {
    mov edi,[scr]
    mov ecx,[framebuf]
    add edi,1920
    add ecx,1920
    mov [count],3040
  @overlay:

    mov eax,[edi]
    mov ebx,eax
    shr eax,24
    xor edx,edx
    mov dl,[opacity]
    mul dx
    mov al,ah
    ror eax,8

    mov edx,[ecx]
    push ecx
    and edx,0x00ffffff
    mov cl,al

// edx - framebuffer pixel
// ebx - overlay pixel
// eax - oo0000oo opacity

// ----------------------

    cmp al,128
    jl @red1
    xchg dl,bl
    not al
    
@red1:

    mov ch,dl
    sub dl,bl
    mul dl
    mov dl,ah
    add dl,ch

    mov al,cl

    ror edx,8
    shr ebx,8

// ----------------------
    cmp al,128
    jl @green1
    xchg dl,bl
    not al

@green1:

    mov ch,dl
    sub dl,bl
    mul dl
    mov dl,ah
    add dl,ch

    mov al,cl

    ror edx,8
    shr ebx,8

// ----------------------
    cmp al,128
    jl @blue1
    xchg dl,bl
    not al

@blue1:

    mov ch,dl
    sub dl,bl
    mul dl
    mov dl,ah
    add dl,ch

    ror edx,16

    pop ecx
    mov [ecx],edx

    add edi,4
    add ecx,4

    dec [count]
    jnz @overlay

  }
}
*/
