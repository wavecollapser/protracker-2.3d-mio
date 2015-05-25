/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <stdint.h>
#include "pt_header.h"
#include "pt_palette.h"
#include "pt_tables.h"
#include "pt_textout.h"
#include "pt_helpers.h"

static uint8_t periodToNote(int16_t period)
{
    uint8_t l;
    uint8_t m;
    uint8_t h;

    l = 0;
    h = 35;

    while (h >= l)
    {
        m = (h + l) / 2;
        if (m >= 36) break; // should never happen, but let's stay on the safe side

             if (PeriodTable[m] == period) return (m);
        else if (PeriodTable[m]  > period) l = m + 1;
        else                               h = m - 1;
    }

    return (255); // illegal period
}

void drawPatternData(uint32_t *frameBuffer, const MODULE *modEntry)
{
    int8_t rowMiddlePos;
    uint8_t i;
    uint8_t j;
    uint8_t tempNote;
    uint8_t rowDispCheck;

    uint16_t putXOffset;
    uint16_t putYOffset;
    uint16_t rowData;

    modnote_t note;

    for (i = 0; i < 15; ++i)
    {
        rowMiddlePos = i - 7;
        rowDispCheck = modEntry->currRow + rowMiddlePos;

        if (rowDispCheck < MOD_ROWS)
        {
            rowData    = rowDispCheck * 4;
            putYOffset = 140 + (i * 7);

            if (i == 7) // are we on the play row (middle)?
            {
                putYOffset++; // align font to play row (middle)

                // put current row number
                printTwoDecimalsBig(frameBuffer, 8, putYOffset, rowMiddlePos + modEntry->currRow, palette[PAL_GENTXT]);

                // pattern data
                for (j = 0; j < PAULA_CHANNELS; ++j)
                {
                    note       = modEntry->patterns[modEntry->currPattern][rowData + j];
                    putXOffset = 26 + (j * 72);

                    if (note.period == 0)
                    {
                        textOutBig(frameBuffer, putXOffset + 6, putYOffset, "---", palette[PAL_GENTXT]);
                    }
                    else
                    {
                        tempNote = periodToNote(note.period);
                        if (tempNote == 255)
                            textOutBig(frameBuffer, putXOffset + 6, putYOffset, "???", palette[PAL_GENTXT]);
                        else
                            textOutBig(frameBuffer, putXOffset + 6, putYOffset, editor.Accidental ? NoteNames2[tempNote] : NoteNames1[tempNote], palette[PAL_GENTXT]);
                    }

                    if (!editor.ui.blankzero || (note.sample & 0xF0))
                        printOneHexBig(frameBuffer, putXOffset + 30, putYOffset, note.sample >> 4, palette[PAL_GENTXT]);

                    printOneHexBig(frameBuffer, putXOffset + 38, putYOffset, note.sample & 0x0F, palette[PAL_GENTXT]);
                    printOneHexBig(frameBuffer, putXOffset + 46, putYOffset, note.command,       palette[PAL_GENTXT]);
                    printTwoHexBig(frameBuffer, putXOffset + 54, putYOffset, note.param,         palette[PAL_GENTXT]);
                }
            }
            else
            {
                if (i > 7)
                    putYOffset += 7; // beyond play row, jump some pixels out of the row (middle)

                // put current row number
                printTwoDecimals(frameBuffer, 8, putYOffset, rowMiddlePos + modEntry->currRow, palette[PAL_PATTXT]);

                // pattern data
                for (j = 0; j < PAULA_CHANNELS; ++j)
                {
                    note       = modEntry->patterns[modEntry->currPattern][rowData + j];
                    putXOffset = 26 + (j * 72);

                    if (note.period == 0)
                    {
                        textOut(frameBuffer, putXOffset + 6, putYOffset, "---", palette[PAL_PATTXT]);
                    }
                    else
                    {
                        tempNote = periodToNote(note.period);
                        if (tempNote == 255)
                            textOut(frameBuffer, putXOffset + 6, putYOffset, "???", palette[PAL_PATTXT]);
                        else
                            textOut(frameBuffer, putXOffset + 6, putYOffset, editor.Accidental ? NoteNames2[tempNote] : NoteNames1[tempNote], palette[PAL_PATTXT]);
                    }

                    if (!editor.ui.blankzero || (note.sample & 0xF0))
                        printOneHex(frameBuffer, putXOffset + 30, putYOffset, note.sample >> 4, palette[PAL_PATTXT]);

                    printOneHex(frameBuffer, putXOffset + 38, putYOffset, note.sample & 0x0F, palette[PAL_PATTXT]);
                    printOneHex(frameBuffer, putXOffset + 46, putYOffset, note.command,       palette[PAL_PATTXT]);
                    printTwoHex(frameBuffer, putXOffset + 54, putYOffset, note.param,         palette[PAL_PATTXT]);
                }
            }
        }
    }
}
