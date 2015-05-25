/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_EDIT_H
#define __PT_EDIT_H

#include <SDL/SDL.h>
#include <stdint.h>
#include "pt_header.h"

void SaveUndo(MODULE *modEntry);
void UndoLastChange(MODULE *modEntry);
void CopySampleTrack(MODULE *modEntry);
void DelSampleTrack(MODULE *modEntry);
void ExchSampleTrack(MODULE *modEntry);
void TrackNoteUp(uint8_t sampleFlag, uint8_t from, uint8_t to, MODULE *modEntry);
void TrackNoteDown(uint8_t sampleFlag, uint8_t from, uint8_t to, MODULE *modEntry);
void TrackOctaUp(uint8_t sampleFlag, uint8_t from, uint8_t to, MODULE *modEntry);
void TrackOctaDown(uint8_t sampleFlag, uint8_t from, uint8_t to, MODULE *modEntry);
void PattNoteUp(uint8_t sampleFlag, MODULE *modEntry);
void PattNoteDown(uint8_t sampleFlag, MODULE *modEntry);
void PattOctaUp(uint8_t sampleFlag, MODULE *modEntry);
void PattOctaDown(uint8_t sampleFlag, MODULE *modEntry);

void exitGetTextLine(MODULE *modEntry, uint8_t updateValue);
void getTextLine(void);
void getNumLine(uint8_t type);
void handleEditKeys(SDLKey keyEntry, MODULE *modEntry, int8_t normalMode);
int8_t keyToNote(SDLKey keyEntry);

#endif
