/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_SAMPLELOADER_H
#define __PT_SAMPLELOADER_H

#include <stdint.h>
#include "pt_header.h"

void extLoadWAVSampleCallback(MODULE *modEntry, int8_t downSample);
int8_t saveSample(MODULE *modEntry, int8_t checkIfFileExist, int8_t giveNewFreeFilename);
int8_t loadSample(MODULE *modEntry, const char *filename, char *entryname);
void normalize16bitSigned(int16_t *sampleData, uint32_t sampleLength, int8_t stereo);
void normalize8bitUnsigned(uint8_t *sampleData, uint32_t sampleLength, int8_t stereo);

#endif
