/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <stdint.h>
#include "pt_palette.h"
#include "pt_header.h"

uint32_t palette[13] =
{
    0xFF000000, // 01- PAL_BACKGRD
    0xFFBBBBBB, // 02- PAL_BORDER
    0xFF888888, // 03- PAL_GENBKG
    0xFF555555, // 04- PAL_GENBKG2
    0xFFFFDD00, // 05- PAL_QRDSCP
    0xFFDD0044, // 06- PAL_CURSOR
    0xFF000000, // 07- PAL_GENTXT
    0xFF3344FF, // 08- PAL_PATTXT

    0xFF444444, // 09- PAL_POINTER_1
    0xFF777777, // 10- PAL_POINTER_2
    0xFFAAAAAA, // 11- PAL_POINTER_3
    0xFF000000, // 12- PAL_LEMCKEY
    0xFFC0FFEE, // 13- PAL_COLORKEY
};

void pointerErrorMode(void)
{
    palette[PAL_POINTER_1] = 0xFF770000;
    palette[PAL_POINTER_2] = 0xFF990000;
    palette[PAL_POINTER_3] = 0xFFCC0000;
}

void setMsgPointer(void)
{
    editor.ui.pointerMode  = POINTER_MODE_READ_DIR;

    palette[PAL_POINTER_1] = 0xFF004400;
    palette[PAL_POINTER_2] = 0xFF007700;
    palette[PAL_POINTER_3] = 0xFF00AA00;
}

void pointerSetMode(int8_t pointerMode, uint8_t carry)
{
    switch (pointerMode)
    {
        case POINTER_MODE_IDLE:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF444444;
            palette[PAL_POINTER_2] = 0xFF777777;
            palette[PAL_POINTER_3] = 0xFFAAAAAA;
        }
        break;

        case POINTER_MODE_PLAY:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF444400;
            palette[PAL_POINTER_2] = 0xFF777700;
            palette[PAL_POINTER_3] = 0xFFAAAA00;
        }
        break;

        case POINTER_MODE_EDIT:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF000066;
            palette[PAL_POINTER_2] = 0xFF004499;
            palette[PAL_POINTER_3] = 0xFF0055BB;
        }
        break;

        case POINTER_MODE_EDIT_PLAY:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF000066;
            palette[PAL_POINTER_2] = 0xFF004499;
            palette[PAL_POINTER_3] = 0xFF0055BB;
        }
        break;

        case POINTER_MODE_MSG1:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF440044;
            palette[PAL_POINTER_2] = 0xFF770077;
            palette[PAL_POINTER_3] = 0xFFAA00AA;
        }
        break;

        case POINTER_MODE_READ_DIR:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF004400;
            palette[PAL_POINTER_2] = 0xFF007700;
            palette[PAL_POINTER_3] = 0xFF00AA00;
        }
        break;

        case POINTER_MODE_LOAD:
        {
            editor.ui.pointerMode = pointerMode;

            if (carry)
                editor.ui.previousPointerMode = editor.ui.pointerMode;

            palette[PAL_POINTER_1] = 0xFF0000AA;
            palette[PAL_POINTER_2] = 0xFF000077;
            palette[PAL_POINTER_3] = 0xFF000044;
        }
        break;

        default: break;
    }
}

void pointerSetPreviousMode(void)
{
    if (editor.ui.getLineFlag || editor.ui.askScreenShown || editor.ui.clearScreenShown)
        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
    else
        pointerSetMode(editor.ui.previousPointerMode, NO_CARRY);
}
