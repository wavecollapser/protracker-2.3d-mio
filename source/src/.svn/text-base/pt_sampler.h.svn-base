/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_SAMPLER_H
#define __PT_SAMPLER_H

#include <stdint.h>
#include "pt_header.h"

void redoSampleData(MODULE *modEntry, int8_t sample);
void fillRedoBuffer(MODULE *modEntry, int8_t sample);
void samplerRemoveDcOffset(MODULE *modEntry);
void samplerInvertSample(MODULE *modEntry);
void MixChordSample(MODULE *modEntry);
void resampleSample(MODULE *modEntry);
void DoMix(MODULE *modEntry);
void BoostSample(MODULE *modEntry, uint32_t sample);
void FilterSample(MODULE *modEntry, uint32_t sample);
void moveSampleMarkerRight(uint8_t extend, uint8_t toEnd);
void moveSampleMarkerLeft(uint8_t extend, uint8_t toBeg);
void checkSampleAttributes(MODULE_SAMPLE *s);
void toggleTuningTone(MODULE *modEntry);
void SetLoopSprites(void);
void samplerClearCopyBuffer(void);
void samplerSamDelete(MODULE *modEntry, uint8_t cut);
void samplerSamPaste(MODULE *modEntry);
void samplerSamCopy(MODULE *modEntry);
void samplerLoopToggle(void);
void samplerBarPressed(int8_t mouseButtonHeld);
void samplerEditSample(int8_t mouseButtonHeld);
void samplerSamplePressed(int8_t mouseButtonHeld);
void volBoxBarPressed(int8_t mouseButtonHeld);
void samplerZoomOut(void);
void sampleMarkerToBeg(void);
void sampleMarkerToCenter(void);
void sampleMarkerToEnd(void);
void samplerPlayWaveform(MODULE *modEntry);
void samplerPlayDisplay(MODULE *modEntry);
void samplerPlayRange(MODULE *modEntry);
void samplerRangeAll(void);
void samplerShowRange(void);
void samplerShowAll(void);
int32_t getSamplerSmpBeg(void);
int32_t getSamplerSmpLen(void);
void setupSampler(MODULE *modEntry, int8_t keepMarking);
void drawSampler(uint32_t *frameBuffer, MODULE *modEntry);
void UpdateSample(void);
void removeTempLoopPoints(void);
void removeSampleDataMarking(void);
int8_t allocSamplerVars(void);
void deAllocSamplerVars(void);

#endif
