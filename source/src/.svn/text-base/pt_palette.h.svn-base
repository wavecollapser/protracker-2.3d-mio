/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_PALETTE_H
#define __PT_PALETTE_H

#include <stdint.h>

enum
{
    PAL_BACKGRD = 0,
    PAL_BORDER  = 1,
    PAL_GENBKG  = 2,
    PAL_GENBKG2 = 3,
    PAL_QRDSCP  = 4,
    PAL_CURSOR  = 5,
    PAL_GENTXT  = 6,
    PAL_PATTXT  = 7,

    PAL_POINTER_1 =  8,
    PAL_POINTER_2 =  9,
    PAL_POINTER_3 = 10,
    PAL_LEMCKEY   = 11,
    PAL_COLORKEY  = 12,

    POINTER_MODE_IDLE      = 0,
    POINTER_MODE_EDIT      = 1,
    POINTER_MODE_PLAY      = 2,
    POINTER_MODE_MSG1      = 3,
    POINTER_MODE_LOAD      = 4,
    POINTER_MODE_EDIT_PLAY = 5,
    POINTER_MODE_READ_DIR  = 6
};

void setMsgPointer(void);
void pointerErrorMode(void);
void pointerSetMode(int8_t pointerMode, uint8_t carry);
void pointerSetPreviousMode(void);

#endif
