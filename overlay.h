#ifndef _OVERLAYH
#define _OVERLAYH

void loadOverlay(unsigned char *scr, char *filename);
void overlayScreen(void *framebuf, void *scr, unsigned char opacity);

#endif
