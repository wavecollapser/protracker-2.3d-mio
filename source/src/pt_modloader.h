/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_MODLOADER_H
#define __PT_MODLOADER_H

#include <stdint.h>
#include "pt_header.h"

MODULE *createNewMod(void);
int8_t saveModule(MODULE *modEntry, int8_t checkIfFileExist, int8_t giveNewFreeFilename);
int8_t modSave(MODULE *modEntry, char *fileName);
MODULE *modLoad(const char *fileName);

void diskOpLoadFile(uint32_t fileEntryRow, MODULE *modEntry); // pt_mouse.c (don't ask!)

#endif
