/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <SDL/SDL.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include "pt_helpers.h"
#include "pt_header.h"
#include "pt_tables.h"
#include "pt_palette.h"

void updateWindowTitle(MODULE *modEntry, int8_t modified)
{
    char titleTemp[48];

    if (modified)
        modEntry->modified = true;

    if (*modEntry->head.moduleTitle != '\0')
    {
        if (modified)
        {
            if (editor.diskop.moddot)
                sprintf(titleTemp, "ProTracker v2.3D *(mod.%s)", modEntry->head.moduleTitle);
            else
                sprintf(titleTemp, "ProTracker v2.3D *(%s.mod)", modEntry->head.moduleTitle);
        }
        else
        {
            if (editor.diskop.moddot)
                sprintf(titleTemp, "ProTracker v2.3D (mod.%s)", modEntry->head.moduleTitle);
            else
                sprintf(titleTemp, "ProTracker v2.3D (%s.mod)", modEntry->head.moduleTitle);
        }

        SDL_WM_SetCaption(titleTemp, titleTemp);
    }
    else
    {
        if (modified)
        {
            if (editor.diskop.moddot)
                SDL_WM_SetCaption("ProTracker v2.3D *(mod.untitled)", "ProTracker v2.3D *(mod.untitled)");
            else
                SDL_WM_SetCaption("ProTracker v2.3D *(untitled.mod)", "ProTracker v2.3D *(untitled.mod)");
        }
        else
        {
            if (editor.diskop.moddot)
                SDL_WM_SetCaption("ProTracker v2.3D (mod.untitled)", "ProTracker v2.3D (mod.untitled)");
            else
                SDL_WM_SetCaption("ProTracker v2.3D (untitled.mod)", "ProTracker v2.3D (untitled.mod)");
        }
    }
}

void recalcChordLength(MODULE *modEntry)
{
    int8_t note;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[editor.currentSample];

    if (editor.ChordLengthMin)
    {
        note = MAX(MAX((editor.Note1 == 36) ? -1 : editor.Note1,
                       (editor.Note2 == 36) ? -1 : editor.Note2),
                   MAX((editor.Note3 == 36) ? -1 : editor.Note3,
                       (editor.Note4 == 36) ? -1 : editor.Note4));
    }
    else
    {
        note = MIN(MIN(editor.Note1, editor.Note2), MIN(editor.Note3, editor.Note4));
    }

    if ((note < 0) || (note > 35))
        editor.ChordLength = 0;
    else
        editor.ChordLength = ((s->length * PeriodTable[(37 * s->fineTune) + note]) / PeriodTable[editor.tuningNote]) & 0xFFFFFFFE;
}

uint8_t HexToInteger2(char *ptr)
{
    char lo;
    char hi;

    // This routine must ONLY be used on an address
    // where two bytes can be read. It will mess up
    // if the ASCII values are not '0 .. 'F'

    hi = ptr[0];
    lo = ptr[1];

    // high nybble
    if (hi >= 'a')
        hi -= ' ';

    hi -= '0';
    if (hi > 9)
        hi -= 7;

    // low nybble
    if (lo >= 'a')
        lo -= ' ';

    lo -= '0';
    if (lo > 9)
        lo -= 7;

    return ((hi << 4) | lo);
}

void displayMsg(const char *msg)
{
    input.mouse.buttonErrorMsgActive = true;
    input.mouse.buttonErrorMsgBlock  = false;
    input.mouse.errorMsgCounter      = 0;

    if (*msg != '\0')
        setStatusMessage(msg, NO_CARRY);
}

void displayErrorMsg(const char *msg)
{
    input.mouse.buttonErrorMsgActive = true;
    input.mouse.buttonErrorMsgBlock  = true;
    input.mouse.errorMsgCounter      = 0;

    if (*msg != '\0')
        setStatusMessage(msg, NO_CARRY);

    pointerErrorMode();
}

void setPrevStatusMessage(void)
{
    strcpy(editor.ui.statusMessage, editor.ui.previousStatusMessage);
}

void setStatusMessage(const char *message, uint8_t carry)
{
    if (carry)
        strcpy(editor.ui.previousStatusMessage, message);

   strcpy(editor.ui.statusMessage, message);
}

void calcRealModSize(MODULE *modEntry)
{
    uint8_t i;

    // + song name          (20   bytes)
    //
    // ---- x31 ------------------------
    // + sample text        (22   bytes)
    // + sample length      (2    bytes)
    // + sample finetune    (1    byte )
    // + sample volume      (1    byte )
    // + sample repeat      (2    bytes)
    // + sample replen      (2    bytes)
    // =                    (30   bytes)
    // ---------------------------------
    // =                    (930  bytes)
    //
    // + order count        (1    byte )
    // + restart pos (0x7F) (1    byte )
    // + order list         (128  bytes)
    // + module ID          (4    bytes)
    // + 1 pattern          (1024 bytes)
    //
    // =                    (2108 bytes)

    modEntry->head.totalSampleSize = 0;
    for (i = 0; i < MOD_SAMPLES; ++i)
        modEntry->head.totalSampleSize += modEntry->samples[i].length;

    modEntry->head.patternCount = 0;
    for (i = 0; i < MOD_ORDERS; ++i)
    {
        if (modEntry->head.order[i] > modEntry->head.patternCount)
            modEntry->head.patternCount = modEntry->head.order[i];
    }

    modEntry->head.moduleSize = 2108 + modEntry->head.totalSampleSize + (1024 * modEntry->head.patternCount);
}
