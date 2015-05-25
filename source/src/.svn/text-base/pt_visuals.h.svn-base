/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_VISUALS_H
#define __PT_VISUALS_H

#include <stdint.h>
#include "pt_header.h"

void freeBitmaps(void);

int8_t setupVideo(void);
void renderFrame(void);
void blitFrame(void);
void updateSpectrumAnalyzer(int16_t period, int8_t volume);
void updateQuadrascope(MODULE *modEntry);
void toggleFullscreen(void);
uint32_t _50HzCallBack(uint32_t interval, void *param);
void freeDisplayBuffers(void);
void startGfxUpdate(void);
int8_t unpackBitmaps(void);
void createGraphics(void);

// easter egg stuff :)
void initEasterEgg(void);
void spawnLemming(void);

#endif
