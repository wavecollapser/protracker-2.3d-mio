/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_DISKOP_H
#define __PT_DISKOP_H

#include <SDL/SDL.h>
#include <stdint.h>
#include "pt_config.h"

enum
{
    DISKOP_DIR      = 0,
    DISKOP_FILE     = 1,
    DISKOP_NO_CACHE = 0,
    DISKOP_CACHE    = 1
};

#define DISKOP_LIST_SIZE 10

void handleEntryJumping(SDLKey keyEntry);
int8_t diskOpEntryIsDir(int32_t fileIndex);
char *diskOpGetEntry(int32_t fileIndex);
char *diskOpGetEntryPath(int32_t fileIndex);
uint8_t dirCanBeOpened(const char *path);
int8_t diskOpSetPath(const char *path, uint8_t cache);
void diskOpSetInitPath(PTCONFIG *config);
void diskOpGoParent(void);
void diskOpDraw(uint32_t *frameBuffer);
int8_t allocDiskOpVars(void);
void deAllocDiskOpVars(void);
void freeDiskOpFileMem(void);

#endif
