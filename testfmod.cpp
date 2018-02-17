/*===============================================================================================
 SIMPLEST.EXE
 Copyright (c), Firelight Technologies Pty, Ltd, 1999,2000.

 This is the simplest way to play a song through FMOD.  It is basically Init, Load, Play!
===============================================================================================*/

#define WIN32

#include <stdio.h>
#include <stdlib.h>
#ifdef WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <fmod/wincompat.h>
#endif

#include <fmod/fmod.h>
#include <fmod/fmod_errors.h>	/* optional */

int main()
{
  FSOUND_STREAM *stream;
	FMUSIC_MODULE *mod = NULL;
 

	if (FSOUND_GetVersion() < FMOD_VERSION)
	{
		printf("Error : You are using the wrong DLL version!  You should be using FMOD %.02f\n", FMOD_VERSION);
		exit(1);
	}
    
	/*
	    INITIALIZE
	*/
	if (!FSOUND_Init(44100, 32, 0))
	{
		printf("%s\n", FMOD_ErrorString(FSOUND_GetError()));
		exit(1);
	}
   
	FSOUND_SetOutput(FSOUND_OUTPUT_DSOUND);
//        FSOUND_SetDriver(0);
	/*
	    LOAD SONG
	*/

//	mod = FMUSIC_LoadSong("jojo.mid");
//	if (!mod)
        stream=FSOUND_Stream_OpenFile("a.mp3",FSOUND_NORMAL|FSOUND_MPEGACCURATE,0);
        if (!stream)
	{
 		printf("Error\n%s\n", FMOD_ErrorString(FSOUND_GetError()));
		exit(1);
	}

	/*
	    PLAY SONG
	*/

//	FMUSIC_PlaySong(mod);
        int channel=FSOUND_Stream_PlayEx(FSOUND_FREE,stream,NULL,TRUE);
        printf("press any key to start playback...");
        getch();
        printf("\n");
        FSOUND_SetPaused(channel,false);

	/*
	    UPDATE INTERFACE
	*/

	printf("Press any key to quit\n");
	printf("=========================================================================\n");
//	printf("Playing %s...\n", FMUSIC_GetName(mod));
	getch();
 
 
        FSOUND_Stream_Close(stream);

	printf("\n");

	/*
	    FREE SONG AND SHUT DOWN
	*/

//	FMUSIC_FreeSong(mod);
	FSOUND_Close();

    return 0;
}
