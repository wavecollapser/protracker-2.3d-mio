/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_CONFIG_H
#define __PT_CONFIG_H

#include <stdint.h>
#include "pt_header.h"

typedef struct
{
    char defaultDiskOpDir[PATH_MAX_LEN + 1];
    int8_t a4000LowPassFilter;
    int8_t stereoSeparation;
    int8_t scale3x;
    int8_t moddot;
    int8_t blankzero;
    int8_t accidental;
    int8_t transdel;
    int16_t quantizeValue;
    uint32_t soundFrequency;
} PTCONFIG;

int8_t loadConfig(PTCONFIG *config);

#endif
