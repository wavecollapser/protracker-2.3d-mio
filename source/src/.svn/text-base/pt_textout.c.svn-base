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
#include "pt_helpers.h"
#include "pt_tables.h"

void charOut(uint32_t *frameBuffer, uint32_t x, uint32_t y, char ch, uint32_t fontColor)
{
    const uint8_t *fontPointer;
    uint8_t line;

    uint32_t *frameBufferPointer;

    if (ch <= ' ')
        return;

    frameBufferPointer = frameBuffer + ((y * SCREEN_W) + x);
    fontPointer        = fontBitmap  + (ch  * (FONT_CHAR_W * FONT_CHAR_H));

    line = FONT_CHAR_H;
    while (line--)
    {
        if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
        if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
        if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
        if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
        if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
        if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
        if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
        if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

        fontPointer        += FONT_CHAR_W;
        frameBufferPointer += SCREEN_W;
    }
}

void charOutBig(uint32_t *frameBuffer, uint32_t x, uint32_t y, char ch, uint32_t fontColor)
{
    const uint8_t *fontPointer;
    uint8_t line;

    uint32_t *frameBufferPointer;

    if (ch <= ' ')
        return;

    frameBufferPointer = frameBuffer + ((y * SCREEN_W) + x);
    fontPointer        = fontBitmap  + (ch * (FONT_CHAR_W * FONT_CHAR_H));

    line = FONT_CHAR_H;
    while (line--)
    {
        if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
        if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
        if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
        if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
        if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
        if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
        if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
        if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

        frameBufferPointer += (SCREEN_W);

        if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
        if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
        if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
        if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
        if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
        if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
        if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
        if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

        fontPointer        += FONT_CHAR_W;
        frameBufferPointer += (SCREEN_W);
    }
}

void textOut(uint32_t *frameBuffer, uint32_t x, uint32_t y, const char *text, uint32_t fontColor)
{
    const uint8_t *fontPointer;
    uint8_t line;

    uint32_t *frameBufferPointer;
    uint32_t i;

    frameBufferPointer = frameBuffer + ((y * SCREEN_W) + x);
    for (i = 0; text[i]; ++i)
    {
        if (text[i] <= ' ')
        {
            frameBufferPointer += FONT_CHAR_W;
            continue;
        }

        fontPointer = fontBitmap + (text[i] * (FONT_CHAR_W * FONT_CHAR_H));

        line = FONT_CHAR_H;
        while (line--)
        {
            if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
            if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
            if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
            if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
            if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
            if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
            if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
            if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

            fontPointer        += FONT_CHAR_W;
            frameBufferPointer += SCREEN_W;
        }

        frameBufferPointer -= ((FONT_CHAR_H * SCREEN_W) - FONT_CHAR_W);
    }
}

void textOutBig(uint32_t *frameBuffer, uint32_t x, uint32_t y, const char *text, uint32_t fontColor)
{
    const uint8_t *fontPointer;
    uint8_t line;

    uint32_t *frameBufferPointer;
    uint32_t i;

    frameBufferPointer = frameBuffer + ((y * SCREEN_W) + x);
    for (i = 0; text[i]; ++i)
    {
        if (text[i] <= ' ')
        {
            frameBufferPointer += FONT_CHAR_W;
            continue;
        }

        fontPointer = fontBitmap  + (text[i] * (FONT_CHAR_W * FONT_CHAR_H));

        line = FONT_CHAR_H;
        while (line--)
        {
            if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
            if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
            if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
            if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
            if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
            if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
            if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
            if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

            frameBufferPointer += (SCREEN_W);

            if (*(fontPointer + 0)) *(frameBufferPointer + 0) = fontColor;
            if (*(fontPointer + 1)) *(frameBufferPointer + 1) = fontColor;
            if (*(fontPointer + 2)) *(frameBufferPointer + 2) = fontColor;
            if (*(fontPointer + 3)) *(frameBufferPointer + 3) = fontColor;
            if (*(fontPointer + 4)) *(frameBufferPointer + 4) = fontColor;
            if (*(fontPointer + 5)) *(frameBufferPointer + 5) = fontColor;
            if (*(fontPointer + 6)) *(frameBufferPointer + 6) = fontColor;
            if (*(fontPointer + 7)) *(frameBufferPointer + 7) = fontColor;

            fontPointer        += FONT_CHAR_W;
            frameBufferPointer += (SCREEN_W);
        }

        frameBufferPointer -= (((FONT_CHAR_H * 2) * SCREEN_W) - FONT_CHAR_W);
    }
}

void printTwoDecimals(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "00", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 99);

        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, '0' + (value % 10), fontColor);
    }
}

void printTwoDecimalsBig(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOutBig(frameBuffer, x, y, "00", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 99);

        charOutBig(frameBuffer, x + (FONT_CHAR_W * 1), y, '0' + (value % 10), fontColor); value /= 10;
        charOutBig(frameBuffer, x + (FONT_CHAR_W * 0), y, '0' + (value % 10), fontColor);
    }
}

void printThreeDecimals(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 999);

        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, '0' + (value % 10), fontColor);
    }
}

void printFourDecimals(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "0000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 9999);

        charOut(frameBuffer, x + (FONT_CHAR_W * 3), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, '0' + (value % 10), fontColor);
    }
}

void printFiveDecimals(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "00000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 99999);

        charOut(frameBuffer, x + (FONT_CHAR_W * 4), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 3), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, '0' + (value % 10), fontColor); value /= 10;
        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, '0' + (value % 10), fontColor);
    }
}

// this one is used for module size and sampler screen display length (zeroes are padded with space)
void printSixDecimals(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    char NumberText[7];
    uint8_t i;

    if (value == 0)
    {
        charOut(frameBuffer, x + ((6 - 1) * FONT_CHAR_W), y, '0', fontColor);
    }
    else
    {
        PT_ASSERT(value <= 999999);

        NumberText[6] = 0;
        NumberText[5] = '0' + (value % 10); value /= 10;
        NumberText[4] = '0' + (value % 10); value /= 10;
        NumberText[3] = '0' + (value % 10); value /= 10;
        NumberText[2] = '0' + (value % 10); value /= 10;
        NumberText[1] = '0' + (value % 10); value /= 10;
        NumberText[0] = '0' + (value % 10);

        i = 0;
        while (NumberText[i] == '0')
            NumberText[i++] = ' ';

        textOut(frameBuffer, x, y, NumberText, fontColor);
    }
}

void printOneHex(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    PT_ASSERT(value <= 0x0000000F);
    charOut(frameBuffer, x, y, hexTable[value & 15], fontColor);
}

void printOneHexBig(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    PT_ASSERT(value <= 0x0000000F);
    charOutBig(frameBuffer, x, y, hexTable[value & 15], fontColor);
}

void printTwoHex(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "00", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 0x000000FF);
        value &= 0x000000FF;

        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, hexTable[value >> 4], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, hexTable[value & 15], fontColor);
    }
}

void printTwoHexBig(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOutBig(frameBuffer, x, y, "00", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 0x000000FF);
        value &= 0x000000FF;

        charOutBig(frameBuffer, x + (FONT_CHAR_W * 0), y, hexTable[value >> 4], fontColor);
        charOutBig(frameBuffer, x + (FONT_CHAR_W * 1), y, hexTable[value & 15], fontColor);
    }
}

void printThreeHex(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 0x00000FFF);
        value &= 0x00000FFF;

        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, hexTable[ value              >> 8], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, hexTable[(value & (15 << 4)) >> 4], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, hexTable[ value              & 15], fontColor);
    }
}

void printFourHex(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "0000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 0x0000FFFF);
        value &= 0x0000FFFF;

        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, hexTable[ value              >> 12], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, hexTable[(value & (15 << 8)) >>  8], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, hexTable[(value & (15 << 4)) >>  4], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 3), y, hexTable[ value               & 15], fontColor);
    }
}

void printFiveHex(uint32_t *frameBuffer, uint32_t x, uint32_t y, uint32_t value, uint32_t fontColor)
{
    if (value == 0)
    {
        textOut(frameBuffer, x, y, "00000", fontColor);
    }
    else
    {
        PT_ASSERT(value <= 0x000FFFFF);
        value &= 0x000FFFFF;

        charOut(frameBuffer, x + (FONT_CHAR_W * 0), y, hexTable[ value               >> 16], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 1), y, hexTable[(value & (15 << 12)) >> 12], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 2), y, hexTable[(value & (15 <<  8)) >>  8], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 3), y, hexTable[(value & (15 <<  4)) >>  4], fontColor);
        charOut(frameBuffer, x + (FONT_CHAR_W * 4), y, hexTable[ value                & 15], fontColor);
    }
}
