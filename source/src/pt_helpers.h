/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_HELPERS_H
#define __PT_HELPERS_H

#include <stdint.h>
#include "pt_header.h"

#ifdef _MSC_VER
#define inline __forceinline
#endif

#if defined (WIN32) && defined (_DEBUG)
#define PT_DEBUGBREAK() __debugbreak()
#else
#define PT_DEBUGBREAK()
#endif

#if defined (WIN32) && defined (_DEBUG)
#define PT_ASSERT(X) if (!(X)) __debugbreak()
#else
#define PT_ASSERT(X)
#endif

#ifndef true
#define true  1
#define false 0
#endif

#define SWAP16(value) \
( \
    (((uint16_t)((value) & 0x00FF)) << 8) | \
    (((uint16_t)((value) & 0xFF00)) >> 8)   \
)

#define SWAP32(value) \
( \
    (((uint32_t)((value) & 0x000000FF)) << 24) | \
    (((uint32_t)((value) & 0x0000FF00)) <<  8) | \
    (((uint32_t)((value) & 0x00FF0000)) >>  8) | \
    (((uint32_t)((value) & 0xFF000000)) >> 24)   \
)

#define SGN(x) (((x) >= 0) ? 1 : -1)
#define ABS(a) (((a) < 0) ? -(a) : (a))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define LERP(x, y, z) ((x) + ((y) - (x)) * (z))

void updateWindowTitle(MODULE *modEntry, int8_t modified);
void recalcChordLength(MODULE *modEntry);
uint8_t HexToInteger2(char *ptr);

void displayMsg(const char *msg);
void displayErrorMsg(const char *msg);
void handleAskNo(MODULE *modEntry);
void handleAskYes(MODULE *modEntry);
void setPrevStatusMessage(void);
void setStatusMessage(const char *message, uint8_t carry);
void calcRealModSize(MODULE *modEntry);

#endif
