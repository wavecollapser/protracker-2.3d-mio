/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_TABLES_H
#define __PT_TABLES_H

#include <stdint.h>

// TABLES
extern const char NoteNames1[36][4];
extern const char NoteNames2[36][4];
extern const int8_t TToneData[32];
extern const uint16_t CursorPosTable[24];
extern const int8_t VUmeterHeights[65];
extern const uint8_t FunkTable[16];
extern const uint8_t VibratoTable[32];
extern int16_t PeriodTable[606];
extern float *deltaTable;
extern float *deltaTable22kHz;
extern float *deltaTable28kHz;
extern const char hexTable[16];
extern const char UnshiftedKeymap[64];
extern const int8_t RawKeyScaleTable[41];
extern const int8_t KbdTransTable[2][41];
extern int8_t pnotetable[32];

// GFX
extern const uint32_t iconBitmap[1024];
extern const uint32_t loopPinsBitmap[512];
extern const uint8_t mousePointerBitmap[256];
extern const uint8_t fontBitmap[5120];
extern const uint8_t arrowPaletteBitmap[30];

// PACKED GFX
extern const uint8_t aboutScreenPackedBitmap[1515];
extern const uint8_t clearDialogPackedBitmap[525];
extern const uint8_t diskOpScreenPackedBitmap[1898];
extern const uint8_t editOpModeCharsPackedBitmap[88];
extern const uint8_t editOpScreen1PackedBitmap[1481];
extern const uint8_t editOpScreen2PackedBitmap[1502];
extern const uint8_t editOpScreen3PackedBitmap[1717];
extern const uint8_t editOpScreen4PackedBitmap[1701];
extern const uint8_t introScreenPackedBitmap[1877];
extern const uint8_t mod2wavPackedBitmap[607];
extern const uint8_t muteButtonsPackedBitmap[46];
extern const uint8_t posEdPackedBitmap[1367];
extern const uint8_t samplerVolumePackedBitmap[706];
extern const uint8_t samplerScreenPackedBitmap[3042];
extern const uint8_t spectrumVisualsPackedBitmap[2217];
extern const uint8_t trackerFramePackedBitmap[8446];
extern const uint8_t yesNoDialogPackedBitmap[476];

// changable by config file
extern uint16_t AnalyzerColors[36];
extern uint16_t VUmeterColors[48];

// these are filled on init, so no const
extern uint32_t vuMeterBitmap[480];
extern uint32_t patternCursorBitmap[154];
extern uint32_t spectrumAnaBitmap[36];
extern uint32_t *editOpScreen1Bitmap;
extern uint32_t *editOpScreen2Bitmap;
extern uint32_t *editOpScreen3Bitmap;
extern uint32_t *editOpScreen4Bitmap;
extern uint32_t *yesNoDialogBitmap;
extern uint32_t *spectrumVisualsBitmap;
extern uint32_t *posEdBitmap;
extern uint32_t *mod2wavBitmap;
extern uint32_t *diskOpScreenBitmap;
extern uint32_t *clearDialogBitmap;
extern uint32_t *samplerScreenBitmap;
extern uint32_t *samplerVolumeBitmap;
extern uint32_t *trackerFrameBitmap;
extern uint32_t *introScreenBitmap;
extern uint32_t *aboutScreenBitmap;
extern uint32_t *muteButtonsBitmap;
extern uint32_t *arrowBitmap;
extern uint32_t *editOpModeCharsBitmap;

// PALETTE
extern uint32_t palette[13];

// easter egg stuff :)
extern uint32_t lem_fall[480];
extern uint32_t lem_fall_umbrella[2304];
extern uint32_t lem_walk[960];
extern uint32_t lem_climb[1584];

#define EDOP_MODE_BMP_A_OFS ((7 * 6) * 0)
#define EDOP_MODE_BMP_C_OFS ((7 * 6) * 1)
#define EDOP_MODE_BMP_H_OFS ((7 * 6) * 2)
#define EDOP_MODE_BMP_N_OFS ((7 * 6) * 3)
#define EDOP_MODE_BMP_O_OFS ((7 * 6) * 4)
#define EDOP_MODE_BMP_P_OFS ((7 * 6) * 5)
#define EDOP_MODE_BMP_S_OFS ((7 * 6) * 6)
#define EDOP_MODE_BMP_T_OFS ((7 * 6) * 7)

#endif
