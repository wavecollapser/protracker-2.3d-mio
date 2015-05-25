/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_BUTTONS_H
#define __PT_BUTTONS_H

#include <SDL/SDL.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <stdint.h>
#include "pt_header.h"

// taken from the ptplay project
enum ptbuttons
{
    PTB_DUMMY,

    // ALL SCREENS
    PTB_SONGNAME,
    PTB_SAMPLENAME,
    PTB_LOADSAMPLE,

    // QUIT SCREEN
    PTB_SUREY,
    PTB_SUREN,

    // POS ED SCREEN
    PTB_PE_BADGE,
    PTB_PE_PATT,
    PTB_PE_PATTNAME,
    PTB_PE_SCROLLTOP,
    PTB_PE_SCROLLUP,
    PTB_PE_EXIT,
    PTB_PE_SCROLLDOWN,
    PTB_PE_SCROLLBOT,

    // EDIT OP SCREEN
    PTB_EO_TITLEBAR,
    PTB_EO_1,
    PTB_EO_2,
    PTB_EO_3,
    PTB_EO_EXIT,

    // screen 1
    PTB_EO_TRACK_NOTE_UP,
    PTB_EO_PATT_NOTE_UP,
    PTB_EO_TRACK_NOTE_DOWN,
    PTB_EO_PATT_NOTE_DOWN,
    PTB_EO_TRACK_OCTA_UP,
    PTB_EO_PATT_OCTA_UP,
    PTB_EO_TRACK_OCTA_DOWN,
    PTB_EO_PATT_OCTA_DOWN,

    // screen 2
    PTB_EO_RECORD,
    PTB_EO_DELETE,
    PTB_EO_KILL,
    PTB_EO_QUANTIZE,
    PTB_EO_EXCHGE,
    PTB_EO_COPY,
    PTB_EO_METRO_1,
    PTB_EO_METRO_2,
    PTB_EO_FROM,
    PTB_EO_FROM_NUM,
    PTB_EO_FROM_UP,
    PTB_EO_FROM_DOWN,
    PTB_EO_KEYS,
    PTB_EO_TO,
    PTB_EO_TO_NUM,
    PTB_EO_TO_UP,
    PTB_EO_TO_DOWN,

    // screen 3
    PTB_EO_MIX,
    PTB_EO_ECHO,
    PTB_EO_POS,
    PTB_EO_POS_NUM,
    PTB_EO_POS_UP,
    PTB_EO_POS_DOWN,
    PTB_EO_BOOST,
    PTB_EO_FILTER,
    PTB_EO_MOD,
    PTB_EO_MOD_NUM,
    PTB_EO_MOD_UP,
    PTB_EO_MOD_DOWN,
    PTB_EO_X_FADE,
    PTB_EO_BACKWD,
    PTB_EO_CB,
    PTB_EO_CHORD,
    PTB_EO_FU,
    PTB_EO_FD,
    PTB_EO_UPSAMP,
    PTB_EO_DNSAMP,
    PTB_EO_VOL,
    PTB_EO_VOL_NUM,
    PTB_EO_VOL_UP,
    PTB_EO_VOL_DOWN,

    // screen 4
    PTB_EO_DOCHORD,
    PTB_EO_MAJOR,
    PTB_EO_MAJOR7,
    PTB_EO_NOTE1,
    PTB_EO_NOTE1_UP,
    PTB_EO_NOTE1_DOWN,
    PTB_EO_RESET,
    PTB_EO_MINOR,
    PTB_EO_MINOR7,
    PTB_EO_NOTE2,
    PTB_EO_NOTE2_UP,
    PTB_EO_NOTE2_DOWN,
    PTB_EO_UNDO,
    PTB_EO_SUS4,
    PTB_EO_MAJOR6,
    PTB_EO_NOTE3,
    PTB_EO_NOTE3_UP,
    PTB_EO_NOTE3_DOWN,
    PTB_EO_LENGTH,
    PTB_EO_MINOR6,
    PTB_EO_NOTE4,
    PTB_EO_NOTE4_UP,
    PTB_EO_NOTE4_DOWN,

    // DISK OP SCREEN
    PTB_DO_BADGE,
    PTB_DO_PACKMOD,
    PTB_DO_MODARROW,
    PTB_DO_LOADMODULE,
    PTB_DO_SAVEMODULE,
    PTB_DO_SAMPLEFORMAT,
    PTB_DO_SAMPLEARROW,
    PTB_DO_LOADSAMPLE,
    PTB_DO_SAVESAMPLE,
    PTB_DO_DATAPATH,
    PTB_DO_PARENT,
    PTB_DO_REFRESH,
    PTB_DO_SCROLLTOP,
    PTB_DO_SCROLLUP,
    PTB_DO_EXIT,
    PTB_DO_SCROLLDOWN,
    PTB_DO_SCROLLBOT,
    PTB_DO_FILEAREA,

    // CLEAR
    PTB_CLEARSONG,
    PTB_CLEARSAMPLES,
    PTB_CLEARALL,
    PTB_CLEARCANCEL,

    // MAIN SCREEN
    PTB_QUIT,
    PTB_PLAY,
    PTB_PATTERN,
    PTB_EDIT,
    PTB_RECORD,
    PTB_STOP,
    PTB_CLEAR,
    PTB_EDITOP,
    PTB_DISKOP,
    PTB_MOD2WAV,
    PTB_POSED,
    PTB_PAT2SMP,
    PTB_SAMPLER,
    PTB_CHAN1,
    PTB_CHAN2,
    PTB_CHAN3,
    PTB_CHAN4,
    PTB_ABOUT,
    PTB_POSS,
    PTB_POSU,
    PTB_POSD,
    PTB_PATTERNS,
    PTB_PATTERNU,
    PTB_PATTERND,
    PTB_LENGTHS,
    PTB_LENGTHU,
    PTB_LENGTHD,
    PTB_FTUNEU,
    PTB_FTUNED,
    PTB_SAMPLES,
    PTB_SAMPLEU,
    PTB_SAMPLED,
    PTB_SVOLUMES,
    PTB_SVOLUMEU,
    PTB_SVOLUMED,
    PTB_SLENGTHS,
    PTB_SLENGTHU,
    PTB_SLENGTHD,
    PTB_SREPEATS,
    PTB_SREPEATU,
    PTB_SREPEATD,
    PTB_SREPLENS,
    PTB_SREPLENU,
    PTB_SREPLEND,
    PTB_TEMPOU,
    PTB_TEMPOD,
    PTB_POS,
    PTB_POSINS,
    PTB_POSDEL,
    PTB_VISUALS,
    PTB_PATTBOX,
    PTB_PATTDATA,

    // SAMPLER SCREEN
    PTB_SA_EXIT,
    PTB_SA_PLAYWAVE,
    PTB_SA_SHOWRANGE,
    PTB_SA_ZOOMOUT,
    PTB_SA_STOP,
    PTB_SA_PLAYDISPLAYED,
    PTB_SA_SHOWALL,
    PTB_SA_RANGEALL,
    PTB_SA_LOOP,
    PTB_SA_PLAYRANGE,
    PTB_SA_RANGECENTER,
    PTB_SA_RANGEBEG,
    PTB_SA_RANGEEND,
    PTB_SA_CUT,
    PTB_SA_COPY,
    PTB_SA_PASTE,
    PTB_SA_VOLUME,
    PTB_SA_TUNETONE,
    PTB_SA_FIXDC,
    PTB_SA_INVERT,
    PTB_SA_RESAMPLE,
    PTB_SA_RESAMPLENOTE,
    PTB_SA_SAMPLEAREA,
    PTB_SA_ZOOMBARAREA,
    PTB_SA_VOL_FROM_NUM,
    PTB_SA_VOL_TO_NUM,

    PTB_NUMBUTTONS
};

typedef struct
{
    int32_t x1;
    int32_t y1;
    int32_t x2;
    int32_t y2;
    int32_t b;
} pt_button_s;

void posMoveLeft(void);
void posMoveRight(void);
void moveCharLeft(void);
void moveCharRight(void);

#ifdef WIN32
LRESULT CALLBACK lowLevelKeyboardProc(int32_t nCode, WPARAM wParam, LPARAM lParam);
#endif

void updateKeyModifiers(void);
int32_t checkGUIButtons(void);
void handleKeyRepeat(SDLKey keyEntry, MODULE *modEntry);
void keyUpHandler(SDLKey keyEntry);
void keyDownHandler(SDLKey keyEntry, MODULE *modEntry);

#endif
