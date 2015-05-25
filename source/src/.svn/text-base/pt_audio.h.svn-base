/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_AUDIO_H
#define __PT_AUDIO_H

#include <stdint.h>
#include "pt_header.h"

void setLEDFilter(uint8_t state);
void toggleLEDFilter(void);
int8_t renderToWav(char *filename, int8_t checkIfFileExist);
void toggleAmigaPanMode(void);
void toggleLowPassFilter(void);
void UpdateVoiceParameters(MODULE *modEntry);
void mixerSwapChSource(uint8_t ch, const int8_t *src, int32_t length, int32_t loopStart, int32_t loopLength, int32_t newSampleOffset);
void mixerSetChSource(uint8_t ch, const int8_t *src, int32_t length, int32_t loopStart, int32_t loopLength, int32_t offset);
void mixerSetChVol(uint8_t ch, int8_t vol);
void mixerSetChDelta(uint8_t ch, uint16_t period);
void mixerKillVoice(uint8_t ch);
void mixerKillAllVoices(void);
void mixerKillVoiceIfReadingSample(MODULE *modEntry, uint8_t sample);
void mixerCalcChPans(uint8_t stereoSeparation);
void mixerSetSamplesPerTick(int32_t val);

#endif
