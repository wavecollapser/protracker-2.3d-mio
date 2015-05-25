/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h> // floorf()
#include "pt_header.h"
#include "pt_helpers.h"
#include "pt_textout.h"
#include "pt_audio.h"
#include "pt_palette.h"
#include "pt_tables.h"
#include "pt_visuals.h"
#include "pt_sampleloader.h" // normalize16bitSigned()

extern uint32_t *tempScreenBuffer; // pt_main.c

static const int8_t *SamOrgStart;
static const int8_t *SamStart;
static int8_t *BlankSample;
static int8_t *CopyBuf;
static int8_t LoopOnOffFlag;

static uint16_t DragStart;
static uint16_t DragEnd;
static uint16_t PenX;
static uint16_t PenY;
static uint16_t DrawX;
static uint16_t DrawY;
static uint16_t LoopStartPos;
static uint16_t LoopEndPos;
static uint16_t SaveMX;
static uint16_t LastSamPos;

static int32_t SamOffset;
static int32_t SamDisplay;
static int32_t SamLength;
static int32_t SamOff;
static int32_t lastMouseX;
static int32_t lastMouseY;
static int32_t tmpLoopStart;
static int32_t tmpLoopLength;
static uint32_t *SamBuf;
static uint32_t CopyBufSize;

static MODULE *tmpModEntry;

typedef struct mix_data
{
    int32_t length;
    int32_t index;
    float frac;
    float delta;
} Mixer;

void RedrawSample(int8_t keepMarking);
void FillSampleBuffer(void);
void SetLoopSprites(void);
void setupSampler(MODULE *modEntry, int8_t keepMarking);

static void drawLine(uint32_t *frameBuffer, uint16_t w, uint16_t line_x1, uint16_t line_y1, uint16_t line_x2, uint16_t line_y2, uint32_t color)
{
    // this function must *NOT* draw outside of the pixel buffer!
    // only feed it parameters that are sane.

    int16_t d;
    int16_t x;
    int16_t y;
    int16_t ax;
    int16_t ay;
    int16_t sx;
    int16_t sy;
    int16_t dx;
    int16_t dy;

    dx = line_x2 - line_x1;
    ax = ABS(dx) * 2;
    sx = SGN(dx);
    dy = line_y2 - line_y1;
    ay = ABS(dy) * 2;
    sy = SGN(dy);
    x  = line_x1;
    y  = line_y1;

    if (ax > ay)
    {
        d = ay - (ax / 2);

        while (true)
        {
            frameBuffer[(y * w) + x] = color;

            if (x == line_x2)
                break;

            if (d >= 0)
            {
                y += sy;
                d -= ax;
            }

            x += sx;
            d += ay;
        }
    }
    else
    {
        d = ax - (ay / 2);

        while (true)
        {
            frameBuffer[(y * w) + x] = color;

            if (y == line_y2)
                break;

            if (d >= 0)
            {
                x += sx;
                d -= ay;
            }

            y += sy;
            d += ax;
        }
    }
}

static void SetDragBar(void)
{
    if ((SamLength <= 0) || (SamDisplay == SamLength))
        return;

    DragStart = (uint16_t)((((             SamOffset) * 311) / SamLength) + 4);
    DragEnd   = (uint16_t)((((SamDisplay + SamOffset) * 311) / SamLength) + 5);
}

static void clampSamplePos(void)
{
    if (tmpModEntry == NULL)
    {
        PT_DEBUGBREAK();
        return;
    }

    if (editor.SamplePos > tmpModEntry->samples[editor.currentSample].length)
        editor.SamplePos = tmpModEntry->samples[editor.currentSample].length;
}

void redoSampleData(MODULE *modEntry, int8_t sample)
{
    MODULE_SAMPLE *s;

    s = &modEntry->samples[sample];

    mixerKillVoiceIfReadingSample(modEntry, sample);

    memset(&modEntry->sampleData[s->offset], 0, 0x0001FFFE);
    if ((editor.sampleRedoBuffer[sample] != NULL) && (editor.sampleRedoLengths[sample] > 0))
        memcpy(&modEntry->sampleData[s->offset], editor.sampleRedoBuffer[sample], editor.sampleRedoLengths[sample]);

    s->fineTune   = editor.sampleRedoFinetunes[sample];
    s->volume     = editor.sampleRedoVolumes[sample];
    s->length     = editor.sampleRedoLengths[sample];
    s->loopStart  = editor.sampleRedoLoopStarts[sample];
    s->loopLength = (editor.sampleRedoLoopLengths[sample] < 2) ? 2 : editor.sampleRedoLoopLengths[sample];

    displayMsg("SAMPLE RESTORED !");
    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);

    editor.SamplePos = 0;

    calcRealModSize(modEntry);
}

void fillRedoBuffer(MODULE *modEntry, int8_t sample)
{
    MODULE_SAMPLE *s;

    if (editor.sampleRedoBuffer[sample] != NULL)
    {
        free(editor.sampleRedoBuffer[sample]);
        editor.sampleRedoBuffer[sample] = NULL;
    }

    s = &modEntry->samples[sample];

    editor.sampleRedoFinetunes[sample]   = s->fineTune;
    editor.sampleRedoVolumes[sample]     = s->volume;
    editor.sampleRedoLengths[sample]     = s->length;
    editor.sampleRedoLoopStarts[sample]  = s->loopStart;
    editor.sampleRedoLoopLengths[sample] = s->loopLength;

    if (s->length > 0)
    {
        editor.sampleRedoBuffer[sample] = (int8_t *)(malloc(s->length));
        if (editor.sampleRedoBuffer[sample] != NULL)
            memcpy(editor.sampleRedoBuffer[sample], &modEntry->sampleData[s->offset], s->length);
    }
}

void samplerClearCopyBuffer(void)
{
    memset(CopyBuf, 0, 0x0001FFFE);
    CopyBufSize = 0;
}

int8_t allocSamplerVars(void)
{
    CopyBuf = (int8_t *)(malloc(0x0001FFFE));
    if (CopyBuf == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    SamBuf = (uint32_t *)(malloc(314 * 64 * sizeof (int32_t)));
    if (SamBuf == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    BlankSample = (int8_t *)(calloc(314, 1));
    if (BlankSample == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    LoopOnOffFlag = 0;
    DragStart     = 0;
    DragEnd       = 0;
    PenX          = 0;
    PenY          = 0;
    DrawX         = 0;
    DrawY         = 0;
    LoopStartPos  = 0;
    LoopEndPos    = 0;
    SaveMX        = 0;
    LastSamPos    = 0;
    SamOffset     = 0;
    SamDisplay    = 1;
    SamLength     = 0;
    SamOff        = 0;
    lastMouseX    = 0;
    lastMouseY    = 0;

    samplerClearCopyBuffer();

    return (true);
}

void deAllocSamplerVars(void)
{
    int8_t i;

    if (CopyBuf != NULL)
    {
        free(CopyBuf);
        CopyBuf = NULL;
    }

    if (SamBuf != NULL)
    {
        free(SamBuf);
        SamBuf = NULL;
    }

    if (BlankSample != NULL)
    {
        free(BlankSample);
        BlankSample = NULL;
    }

    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        if (editor.sampleRedoBuffer[i] != NULL)
        {
            free(editor.sampleRedoBuffer[i]);
            editor.sampleRedoBuffer[i] = NULL;
        }
    }
}

void samplerRemoveDcOffset(MODULE *modEntry)
{
    int8_t *smpDat;

    int32_t i;
    int32_t from;
    int32_t to;
    int32_t offset;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[editor.currentSample];
    if (s->length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    smpDat = &modEntry->sampleData[s->offset];

    from = 0;
    to   = s->length;

    if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
    {
        from = editor.MarkStartOfs;
        to   = editor.MarkEndOfs;
    }

    // calculate offset value
    offset = 0;
    for (i = from; i < to; ++i)
        offset += smpDat[i];
    offset /= to;

    // remove DC offset
    for (i = from; i < to; ++i)
        smpDat[i] = (int8_t)(CLAMP((int16_t)(smpDat[i]) - offset, -128, 127));

    RedrawSample(KEEP_SAMPLE_MARKING);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void samplerInvertSample(MODULE *modEntry)
{
    int8_t *smpDat;

    int32_t i;
    int32_t from;
    int32_t to;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[editor.currentSample];
    if (s->length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    smpDat = &modEntry->sampleData[s->offset];

    from = 0;
    to   = s->length;

    if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
    {
        from = editor.MarkStartOfs;
        to   = editor.MarkEndOfs;
    }

    for (i = from; i < to; ++i)
        smpDat[i] ^= 0xFF;

    RedrawSample(KEEP_SAMPLE_MARKING);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void MixChordSample(MODULE *modEntry)
{
    char oldText[22];
    int8_t *oldSampleData;
    int8_t sameNotes;
    int8_t oldVolume;
    int8_t oldLoopFlag;
    uint8_t oldFinetune;

    int16_t *tmpSample16;

    int32_t i;
    int32_t j;
    int32_t mixPos2;
    int32_t oldLength;
    int32_t oldLoopStart;
    int32_t oldLoopEnd;

    Mixer mixVoice[4];

    MODULE_SAMPLE *s;

    if (editor.Note1 == 36)
    {
        displayErrorMsg("NO BASENOTE!");
        return;
    }

    if (modEntry->samples[editor.currentSample].length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    // check if all notes are the same (illegal)
    sameNotes = true;
    if ((editor.Note2 != 36) && (editor.Note2 != editor.Note1)) sameNotes = false; else editor.Note2 = 36;
    if ((editor.Note3 != 36) && (editor.Note3 != editor.Note1)) sameNotes = false; else editor.Note3 = 36;
    if ((editor.Note4 != 36) && (editor.Note4 != editor.Note1)) sameNotes = false; else editor.Note4 = 36;

    if (sameNotes)
    {
        displayErrorMsg("ONLY ONE NOTE!");
        return;
    }

    // stupid way of rolling/ordering the notes
    for (i = 0; i < 3; ++i)
    {
        if (editor.Note2 == 36)
        {
            editor.Note2 = editor.Note3;
            editor.Note3 = editor.Note4;
            editor.Note4 = 36;
        }
    }

    for (i = 0; i < 3; ++i)
    {
        if (editor.Note3 == 36)
        {
            editor.Note3 = editor.Note4;
            editor.Note4 = 36;
        }
    }

    // remove duplicates
    if (editor.Note4 == editor.Note3) editor.Note4 = 36;
    if (editor.Note4 == editor.Note2) editor.Note4 = 36;
    if (editor.Note3 == editor.Note2) editor.Note3 = 36;

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    oldLength     = modEntry->samples[editor.currentSample].length;
    oldLoopStart  = modEntry->samples[editor.currentSample].loopStart;
    oldLoopEnd    = oldLoopStart + modEntry->samples[editor.currentSample].loopLength;
    oldLoopFlag   = modEntry->samples[editor.currentSample].loopLength > 2;
    oldSampleData = &modEntry->sampleData[modEntry->samples[editor.currentSample].offset];

    if (editor.NewOldFlag == 0)
    {
        // find a free sample slot for the new sample

        for (i = 0; i < MOD_SAMPLES; ++i)
        {
            if (modEntry->samples[i].length == 0)
                break;
        }

        if (i == MOD_SAMPLES)
        {
            displayErrorMsg("NO EMPTY SAMPLE!");
            return;
        }

        oldFinetune = modEntry->samples[editor.currentSample].fineTune;
        oldVolume   = modEntry->samples[editor.currentSample].volume;

        memcpy(oldText, modEntry->samples[editor.currentSample].text, 22);

        s = &modEntry->samples[i];

        s->fineTune = oldFinetune;
        s->volume   = oldVolume;

        memcpy(s->text, oldText, 22);

        editor.currentSample = (int8_t)(i);
    }
    else
    {
        // overwrite current sample
        s = &modEntry->samples[editor.currentSample];
    }

    tmpSample16 = (int16_t *)(calloc(0x0001FFFE, sizeof (int16_t)));
    if (tmpSample16 == NULL)
    {
        displayErrorMsg("OUT OF MEMORY !!!");
        return;
    }

    s->length       = oldLoopFlag ? 0xFFFE : editor.ChordLength; // if old sample loops, make long sample for easier loop making
    s->loopLength   = 2;
    s->loopStart    = 0;
    s->text[21]     = '!'; // chord sample indicator

    // clear mixing datas
    memset(mixVoice, 0, sizeof (mixVoice));

    // setup mixing lengths
    mixVoice[0].length = (oldLength * PeriodTable[(37 * s->fineTune) + editor.Note1]) / PeriodTable[editor.tuningNote];
    mixVoice[1].length = (oldLength * PeriodTable[(37 * s->fineTune) + editor.Note2]) / PeriodTable[editor.tuningNote];
    mixVoice[2].length = (oldLength * PeriodTable[(37 * s->fineTune) + editor.Note3]) / PeriodTable[editor.tuningNote];
    mixVoice[3].length = (oldLength * PeriodTable[(37 * s->fineTune) + editor.Note4]) / PeriodTable[editor.tuningNote];

    // force lengths to zero if empty notes
    if (editor.Note1 == 36) mixVoice[0].length = 0;
    if (editor.Note2 == 36) mixVoice[1].length = 0;
    if (editor.Note3 == 36) mixVoice[2].length = 0;
    if (editor.Note4 == 36) mixVoice[3].length = 0;

    // setup mixing delta values
    if (editor.Note1 < 36) mixVoice[0].delta = (float)(PeriodTable[editor.tuningNote]) / (float)(PeriodTable[(37 * s->fineTune) + editor.Note1]);
    if (editor.Note2 < 36) mixVoice[1].delta = (float)(PeriodTable[editor.tuningNote]) / (float)(PeriodTable[(37 * s->fineTune) + editor.Note2]);
    if (editor.Note3 < 36) mixVoice[2].delta = (float)(PeriodTable[editor.tuningNote]) / (float)(PeriodTable[(37 * s->fineTune) + editor.Note3]);
    if (editor.Note4 < 36) mixVoice[3].delta = (float)(PeriodTable[editor.tuningNote]) / (float)(PeriodTable[(37 * s->fineTune) + editor.Note4]);

    // start mixing

    for (i = 0; i < 4; ++i) // four maximum channels (notes)
    {
        if (mixVoice[i].length > 0)
        {
            for (j = 0; j < 0x0001FFFE; ++j) // max length
            {
                mixPos2 = mixVoice[i].index + 1;
                if (oldLoopFlag) // wrap second sample (used for linear interpolation)
                {
                    if (mixPos2 >= oldLoopEnd)
                        mixPos2  = oldLoopStart;
                }
                else
                {
                    if (mixPos2 >= oldLength)
                        mixPos2  = 0;
                }

                tmpSample16[j] += (int16_t)(LERP(oldSampleData[mixVoice[i].index], oldSampleData[mixPos2], mixVoice[i].frac));

                mixVoice[i].frac += mixVoice[i].delta;
                if (mixVoice[i].frac >= 1.0f)
                {
                    mixVoice[i].index += (int32_t)(mixVoice[i].frac);
                    mixVoice[i].frac  -= (int32_t)(mixVoice[i].frac);

                    if (oldLoopFlag)
                    {
                        while (mixVoice[i].index >= oldLoopEnd)
                               mixVoice[i].index  = oldLoopStart + (mixVoice[i].index - oldLoopEnd);
                    }
                    else if (mixVoice[i].index >= oldLength)
                    {
                        break;
                    }
                }
            }
        }
    }

    if (editor.HalfClipFlag == 0)
    {
        normalize16bitSigned(tmpSample16, s->length, NORMALIZE_MONO);

        memset(&modEntry->sampleData[s->offset], 0, 131070);
        for (i = 0; i < s->length; ++i) // downscale
            modEntry->sampleData[s->offset + i] = (int8_t)(CLAMP(floorf(((float)(tmpSample16[i]) / 256.0f) + 0.5f), -128.0f, 127.0f));
    }
    else
    {
        memset(&modEntry->sampleData[s->offset], 0, 131070);
        for (i = 0; i < s->length; ++i) // clip/clamp
            modEntry->sampleData[s->offset + i] = (int8_t)(CLAMP(tmpSample16[i], -128, 127));
    }

    free(tmpSample16);

    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
    calcRealModSize(modEntry);

    editor.SamplePos = 0;
}

void resampleSample(MODULE *modEntry)
{
    int8_t *tempSmpPtr;

    int16_t tunePeriod;
    int16_t reSmpPeriod;

    int32_t pos;
    int32_t readPos;
    int32_t readPos2;
    int32_t newLength;

    float delta;
    float frac;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[editor.currentSample];
    if (s->length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    // allocate memory for our temp sample data
    tempSmpPtr = (int8_t *)(malloc(s->length));
    if (tempSmpPtr == NULL)
    {
        displayErrorMsg("OUT OF MEMORY !!!");
        return;
    }

    mixerKillVoiceIfReadingSample(tmpModEntry, editor.currentSample);

    // copy old data into the temp buffer and wipe old data
    memcpy(tempSmpPtr, &modEntry->sampleData[s->offset], s->length);
    memset(&modEntry->sampleData[s->offset], 0, 0x0001FFFE);

    // set up resampling variables
    tunePeriod  = PeriodTable[editor.tuningNote];
    reSmpPeriod = PeriodTable[(37 * s->fineTune) + editor.ResampleNote];

    newLength = ((s->length * reSmpPeriod) / tunePeriod) & 0xFFFFFFFE;
    if (newLength > 0x0001FFFE)
        newLength = 0x0001FFFE;

    // resample!

    delta   = (float)(s->length) / (float)(newLength);
    readPos = 0;
    pos     = 0;
    frac    = 0.0f;

    while (pos <= newLength)
    {
        readPos2 = readPos + 1;
        if (readPos2 >= s->length)
            readPos2  = 0;

        modEntry->sampleData[s->offset + pos] = (int8_t)(LERP(tempSmpPtr[readPos], tempSmpPtr[readPos2], frac));

        frac += delta;
        if (frac >= 1.0f)
        {
            readPos += (int32_t)(frac);
            frac    -= (int32_t)(frac);
        }

        pos++;
    }

    free(tempSmpPtr);

    // update sample
    s->length     = newLength;
    s->fineTune   = 0; // yes, newly resampled sample has no finetuning
    s->loopStart  = 0;
    s->loopLength = 2;

    RedrawSample(REMOVE_SAMPLE_MARKING);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
    calcRealModSize(modEntry);

    clampSamplePos();
}

void DoMix(MODULE *modEntry)
{
    int8_t *FromPtr1;
    int8_t *FromPtr2;
    int8_t *MixPtr;
    uint8_t smpFrom1;
    uint8_t smpFrom2;
    uint8_t smpTo;

    int16_t tmpSmp;

    int32_t i;
    int32_t MixLength;

    MODULE_SAMPLE *s1;
    MODULE_SAMPLE *s2;
    MODULE_SAMPLE *s3;

    smpFrom1 = HexToInteger2(&editor.MixText[4]);
    smpFrom2 = HexToInteger2(&editor.MixText[7]);
    smpTo    = HexToInteger2(&editor.MixText[13]);

    if ((smpFrom1 == 0) || (smpFrom1 > 0x1F) ||
        (smpFrom2 == 0) || (smpFrom2 > 0x1F) ||
        (smpTo    == 0) || (smpTo    > 0x1F))
    {
        displayErrorMsg("NOT RANGE 01-1F !");
        return;
    }

    smpFrom1--;
    smpFrom2--;
    smpTo--;

    s1 = &modEntry->samples[smpFrom1];
    s2 = &modEntry->samples[smpFrom2];
    s3 = &modEntry->samples[smpTo];

    if ((s1->length == 0) || (s2->length == 0))
    {
        displayErrorMsg("EMPTY SAMPLES !!!");
        return;
    }

    if (s1->length > s2->length)
    {
        FromPtr1  = &modEntry->sampleData[s1->offset];
        FromPtr2  = &modEntry->sampleData[s2->offset];
        MixLength = s1->length;
    }
    else
    {
        FromPtr1  = &modEntry->sampleData[s2->offset];
        FromPtr2  = &modEntry->sampleData[s1->offset];
        MixLength = s2->length;
    }

    MixPtr = (int8_t *)(calloc(MixLength, 1));
    if (MixPtr == NULL)
    {
        displayErrorMsg("OUT OF MEMORY !!!");
        return;
    }

    mixerKillVoiceIfReadingSample(modEntry, smpTo);

    for (i = 0; i < MixLength; ++i)
    {
        tmpSmp    = (i < s2->length) ? (FromPtr1[i] + FromPtr2[i]) : FromPtr1[i];
        MixPtr[i] = (int8_t)((editor.HalfClipFlag == 0) ? (tmpSmp / 2) : CLAMP(tmpSmp, -128, 127));
    }

    memset(&modEntry->sampleData[s3->offset],      0, 0x0001FFFE);
    memcpy(&modEntry->sampleData[s3->offset], MixPtr, MixLength);

    free(MixPtr);

    s3->length     = MixLength;
    s3->volume     = 64;
    s3->fineTune   = 0;
    s3->loopStart  = 0;
    s3->loopLength = 2;

    editor.currentSample = smpTo;
    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);

    editor.SamplePos = 0;

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
    calcRealModSize(modEntry);
}

// this is actually treble increase
void BoostSample(MODULE *modEntry, uint32_t sample)
{
    int8_t *smpDat;

    int16_t tmpWord;
    int16_t tmp;
    int16_t sub;

    int32_t i;
    int32_t from;
    int32_t to;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[sample];
    if (s->length == 0)
        return; // don't display warning/show warning pointer, it is done elsewhere

    smpDat = &modEntry->sampleData[s->offset];

    from = 0;
    to   = s->length;

    if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
    {
        from = editor.MarkStartOfs;
        to   = editor.MarkEndOfs;
    }

    sub = 0;
    for (i = from; i < to; ++i)
    {
        tmp      = smpDat[i];
        tmpWord  = tmp;
        tmp     -= sub;
        sub      = tmpWord;
        tmpWord += (tmp / 4);

        // this routine doesn't depend on the HalfClip flag for some reason... (FilterSample does)
        smpDat[i] = (int8_t)(CLAMP(tmpWord, -128, 127));
    }

    // don't redraw sample here, it is done elsewhere

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

// this is actually treble decrease
void FilterSample(MODULE *modEntry, uint32_t sample)
{
    int8_t *smpDat;

    int16_t tmpWord;

    int32_t i;
    int32_t from;
    int32_t to;

    MODULE_SAMPLE *s;

    s = &modEntry->samples[sample];
    if (s->length == 0)
        return; // don't display warning/show warning pointer, it is done elsewhere

    smpDat = &modEntry->sampleData[s->offset];

    from = 0;
    to   = s->length;

    if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
    {
        from = editor.MarkStartOfs;
        to   = editor.MarkEndOfs;
    }

    for (i = from; i < (to - 1); ++i)
    {
        tmpWord   = smpDat[i] + smpDat[i + 1];
        smpDat[i] = (int8_t)((editor.HalfClipFlag == 0) ? (tmpWord / 2) : CLAMP(tmpWord, -128, 127));
    }

    // don't redraw sample here, it is done elsewhere

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void toggleTuningTone(MODULE *modEntry)
{
    uint8_t i;

    if ((editor.currentMode == MODE_PLAY) || (editor.currentMode == MODE_RECORD))
        return;

    i = (editor.cursor.channel + 1) & 3;

    editor.tuningFlag ^= 1;
    if (editor.tuningFlag)
    {
        // tuning tone toggled on
        mixerSetChVol(i, editor.tuningVol);
        mixerSetChDelta(i, PeriodTable[editor.tuningNote]);
        mixerSetChSource(i, TToneData, sizeof (TToneData), 0, sizeof (TToneData), 0);
    }
    else
    {
        // tuning tone toggled off
        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            // shutdown scope
            modEntry->channels[i].scopeLoopQuirk = false;
            modEntry->channels[i].scopeEnabled   = false;
            modEntry->channels[i].scopeTrigger   = false;

            // shutdown voice
            mixerKillVoice(i);
        }
    }
}

static void MarkToOffset(void)
{
    if (editor.MarkStart == SamDisplay)
        return;

    editor.MarkStartOfs = (((editor.MarkStart - 3) * SamDisplay) / 314) + SamOffset;

    if (editor.MarkEnd >= 316)
        editor.MarkEndOfs = SamOffset + SamDisplay;
    else
        editor.MarkEndOfs = (((editor.MarkEnd - 3) * SamDisplay) / 314) + SamOffset;
}

static void OffsetToMark(void)
{
    int32_t MarkTmp;

    // This function has a rounding error, but the problem
    // is there on the Amiga as well, so whatever...

    if (editor.MarkStartOfs < 0)
    {
        editor.MarkStart = 0;
        return;
    }

    editor.MarkStart = 3;

    MarkTmp = editor.MarkStartOfs - SamOffset;
    if (MarkTmp >= 0)
    {
        MarkTmp *= 314;

        if (SamDisplay > 0)
        {
            MarkTmp /= SamDisplay;
            if (MarkTmp > 314)
            {
                editor.MarkStart = 0;
                editor.MarkEnd   = 0;

                return;
            }

            editor.MarkStart += MarkTmp;
        }
    }

    editor.MarkEnd = 3;

    MarkTmp = editor.MarkEndOfs - SamOffset;
    if (MarkTmp >= 0)
    {
        MarkTmp *= 314;

        if (SamDisplay > 0)
        {
            MarkTmp /= SamDisplay;
            if (MarkTmp > 313)
                MarkTmp = 313;

            editor.MarkEnd += MarkTmp;
        }
    }
    else
    {
        editor.MarkStart = 0;
    }

    editor.MarkStart = CLAMP(editor.MarkStart, 2, 316);
    editor.MarkEnd   = CLAMP(editor.MarkEnd,   2, 316);
}

static void InvertRange(void)
{
    uint8_t y;

    int32_t pad;
    int32_t x;
    int32_t len;
    uint32_t *p;

    if ((editor.MarkStart <= editor.MarkEnd) && (editor.MarkStart >= 3) && (editor.MarkStart <= 317))
    {
        len = (editor.MarkEnd - editor.MarkStart) + 1;
        pad = 314 - len;
        p   = &SamBuf[editor.MarkStart - 3];

        y = 64;
        while (y--)
        {
            x = len;
            while (x--)
            {
                     if (*p == palette[PAL_QRDSCP])  *p = palette[PAL_BACKGRD];
                else if (*p == palette[PAL_BACKGRD]) *p = palette[PAL_QRDSCP];

                p++;
            }

            p += pad;
        }
    }
}

void sampleMarkerToBeg(void)
{
    InvertRange();

    if (input.keyb.shiftKeyDown && (editor.MarkStartOfs >= 0))
    {
        editor.MarkStart = 3;
    }
    else
    {
        editor.MarkStart = 3;
        editor.MarkEnd   = 3;
    }

    MarkToOffset();
    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void sampleMarkerToCenter(void)
{
    InvertRange();

    if (input.keyb.shiftKeyDown && (editor.MarkStartOfs >= 0))
    {
        if (!((editor.MarkStart == 3) && (editor.MarkEnd == 316)))
        {
                 if (editor.MarkStart < ((316 / 2) + 1)) editor.MarkEnd   = (316 / 2) + 1;
            else if (editor.MarkStart > ((316 / 2) + 1)) editor.MarkStart = (316 / 2) + 1;
        }
    }
    else
    {
        editor.MarkStart = (316 / 2) + 1;
        editor.MarkEnd   = (316 / 2) + 1;
    }

    MarkToOffset();
    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void sampleMarkerToEnd(void)
{
    InvertRange();

    if (input.keyb.shiftKeyDown && (editor.MarkStartOfs >= 0))
    {
        editor.MarkEnd = 316;
    }
    else
    {
        editor.MarkStart = 316;
        editor.MarkEnd   = 316;
    }

    MarkToOffset();
    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void moveSampleMarkerRight(uint8_t extend, uint8_t toEnd)
{
    if (extend && (editor.MarkStartOfs < 0))
        return;

    InvertRange();

    if (!extend)
    {
        if ((editor.MarkEnd - editor.MarkStart) > 0)
            editor.MarkStart = editor.MarkEnd;

        if (++editor.MarkStart > 316)
              editor.MarkStart = 316;
    }

    if (toEnd)
    {
        if (!extend)
        {
            editor.MarkStart    = 316;
            editor.MarkStartOfs = SamLength;
        }

        editor.MarkEnd    = 316;
        editor.MarkEndOfs = SamLength;
        SamOffset         = SamLength - SamDisplay;

        FillSampleBuffer();
        SetDragBar();
        SetLoopSprites();

        OffsetToMark();
        InvertRange();

        editor.SamplePos = editor.MarkEndOfs;
        clampSamplePos();

        return;
    }
    else
    {
        if ((editor.MarkEnd >= 316) && (SamDisplay != SamLength))
        {
            if (++editor.MarkEndOfs > SamLength)
                  editor.MarkEndOfs = SamLength;

            if (!extend)
                editor.MarkStartOfs = editor.MarkEndOfs;

            if (++SamOffset > (SamLength - SamDisplay))
                  SamOffset =  SamLength - SamDisplay;

            FillSampleBuffer();
            SetDragBar();
            SetLoopSprites();

            OffsetToMark();
            InvertRange();

            editor.SamplePos = editor.MarkEndOfs;
            clampSamplePos();

            return;
        }
        else
        {
            if (++editor.MarkEnd > 316)
                  editor.MarkEnd = 316;
        }
    }

    MarkToOffset();
    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void moveSampleMarkerLeft(uint8_t extend, uint8_t toBeg)
{
    if (extend && (editor.MarkStartOfs < 0))
        return;

    InvertRange();

    if (!extend)
    {
        if ((editor.MarkEnd - editor.MarkStart) > 0)
             editor.MarkEnd = editor.MarkStart;

        if (--editor.MarkEnd < 3)
              editor.MarkEnd = 3;
    }

    if (toBeg)
    {
        if (!extend)
        {
            editor.MarkEnd    = 3;
            editor.MarkEndOfs = 0;
        }

        editor.MarkStart    = 3;
        editor.MarkStartOfs = 0;
        SamOffset           = 0;

        FillSampleBuffer();
        SetDragBar();
        SetLoopSprites();

        OffsetToMark();
        InvertRange();

        editor.SamplePos = editor.MarkEndOfs;
        clampSamplePos();

        return;
    }
    else
    {
        if ((editor.MarkStart <= 3) && (SamDisplay != SamLength))
        {
            if (--editor.MarkStartOfs < 0)
                  editor.MarkStartOfs = 0;

            if (!extend)
                editor.MarkEndOfs = editor.MarkStartOfs;

            if (--SamOffset < 0)
                  SamOffset = 0;

            FillSampleBuffer();
            SetDragBar();
            SetLoopSprites();

            OffsetToMark();
            InvertRange();

            editor.SamplePos = editor.MarkEndOfs;
            clampSamplePos();

            return;
        }
        else
        {
            if (--editor.MarkStart < 3)
                  editor.MarkStart = 3;
        }
    }

    MarkToOffset();
    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void samplerSamCopy(MODULE *modEntry)
{
    MODULE_SAMPLE *s;

    if (editor.MarkStartOfs < 0)
    {
        displayErrorMsg("NO RANGE SELECTED");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    s = &modEntry->samples[editor.currentSample];
    if (s->length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    if (editor.MarkEndOfs >= s->length)
        CopyBufSize = ((s->length - 1)   - editor.MarkStartOfs) + 1;
    else
        CopyBufSize = (editor.MarkEndOfs - editor.MarkStartOfs) + 1;

    memcpy(CopyBuf, &modEntry->sampleData[s->offset + editor.MarkStartOfs], CopyBufSize);

    InvertRange();
    InvertRange();
}

void samplerSamDelete(MODULE *modEntry, uint8_t cut)
{
    int8_t *tmpBuf;

    int32_t sampleLength;
    int32_t copyLength;
    int32_t markEnd;
    int32_t markStart;

    MODULE_SAMPLE *s;

    if (editor.MarkStartOfs < 0)
    {
        displayErrorMsg("NO RANGE SELECTED");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    if (cut)
        samplerSamCopy(modEntry);

    s = &modEntry->samples[editor.currentSample];

    sampleLength = s->length;
    if (sampleLength == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    // if whole sample is marked, nuke it
    if ((editor.MarkEndOfs - editor.MarkStartOfs) >= sampleLength)
    {
        memset(&modEntry->sampleData[s->offset], 0, 0x0001FFFE);

        InvertRange();
        InvertRange();

        editor.MarkStartOfs = -1;
        editor.MarkEndOfs   =  0;
        editor.MarkStart    =  2;
        editor.MarkEnd      =  2;

        InvertRange();

        SamStart      = BlankSample;
        SamDisplay    = 314;
        SamLength     = 314;
        s->length     = 0;
        s->loopStart  = 0;
        s->loopLength = 2;

        FillSampleBuffer();
        SetDragBar();
        SetLoopSprites();

        calcRealModSize(modEntry);
        updateWindowTitle(modEntry, MOD_IS_MODIFIED);

        return;
    }

    markEnd   = (editor.MarkEndOfs >= sampleLength) ? sampleLength : editor.MarkEndOfs;
    markStart = editor.MarkStartOfs;

    copyLength = (editor.MarkStartOfs + sampleLength) - markEnd;
    if (copyLength > 0)
    {
        tmpBuf = (int8_t *)(malloc(copyLength));
        if (tmpBuf == NULL)
        {
            displayErrorMsg("OUT OF MEMORY !!!");
            return;
        }

        // copy start part
        memcpy(&tmpBuf[0], &modEntry->sampleData[s->offset], editor.MarkStartOfs);

        // copy end part
        memcpy(&tmpBuf[editor.MarkStartOfs], &modEntry->sampleData[s->offset + markEnd], sampleLength - markEnd);

        // nuke sample data and copy over the result
        memset(&modEntry->sampleData[s->offset],      0, 0x0001FFFE);
        memcpy(&modEntry->sampleData[s->offset], tmpBuf, copyLength);

        free(tmpBuf);
    }

    SamLength = copyLength;
    if ((SamOffset + SamDisplay) >= SamLength)
    {
        if (SamDisplay < SamLength)
        {
            if ((SamLength - SamDisplay) < 0)
            {
                SamOffset  = 0;
                SamDisplay = SamLength;
            }
            else
            {
                SamOffset = SamLength - SamDisplay;
            }
        }
        else
        {
            SamOffset  = 0;
            SamDisplay = SamLength;
        }
    }

    if (s->loopLength > 2) // loop enabled?
    {
        if (markEnd > s->loopStart)
        {
            if (markStart <= (s->loopStart + s->loopLength))
            {
                // we cut data inside the loop, disable loop
                s->loopStart  = 0;
                s->loopLength = 2;
            }

            // we cut data after the loop, don't modify loop points
        }
        else
        {
            // we cut data before the loop, adjust loop start point
            s->loopStart = (s->loopStart - (markEnd - editor.MarkStartOfs)) & 0xFFFFFFFE;
            if (s->loopStart < 0)
            {
                s->loopStart  = 0;
                s->loopLength = 2;
            }
        }
    }

    s->length = copyLength & 0xFFFFFFFE;
    if (s->length > 0x0001FFFE)
        s->length = 0x0001FFFE;

    if (SamDisplay <= 1)
    {
        SamStart   = BlankSample;
        SamDisplay = 314;
        SamLength  = 314;
    }

    FillSampleBuffer();
    SetDragBar();
    SetLoopSprites();

    InvertRange();
    InvertRange();

    if ((SamDisplay == 0) || (editor.MarkStart > 316) || (editor.MarkEnd > 316))
    {
        editor.MarkStartOfs = -1;
        editor.MarkEndOfs   =  0;
        editor.MarkStart    =  2;
        editor.MarkEnd      =  2;
    }
    else
    {
        editor.MarkEndOfs = editor.MarkStartOfs;
        OffsetToMark();
    }

    InvertRange();

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();

    UpdateVoiceParameters(modEntry);
    calcRealModSize(modEntry);

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void samplerSamPaste(MODULE *modEntry)
{
    int8_t *tmpBuf;
    int8_t wasZooming;

    int32_t tmpMarkStart;
    uint32_t readPos;

    MODULE_SAMPLE *s;

    if ((CopyBuf == NULL) || (CopyBufSize == 0))
    {
        displayErrorMsg("BUFFER IS EMPTY");
        return;
    }

    s = &modEntry->samples[editor.currentSample];

    wasZooming = (SamDisplay != SamLength) ? true : false;

    if (s->length == 0)
    {
        InvertRange();
        editor.MarkStart    = 3;
        editor.MarkEnd      = 3;
        editor.MarkStartOfs = SamOffset;
        editor.MarkEndOfs   = SamOffset;
        InvertRange();
    }
    else if (editor.MarkStartOfs < 0)
    {
        displayErrorMsg("SET CURSOR POS");
        return;
    }

    if ((s->length + CopyBufSize) > 0x0001FFFE)
    {
        displayErrorMsg("NOT ENOUGH ROOM");
        return;
    }

    tmpBuf = (int8_t *)(malloc(0x0001FFFE));
    if (tmpBuf == NULL)
    {
        displayErrorMsg("OUT OF MEMORY !!!");
        return;
    }

    readPos = 0;

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    // copy start part
    if (editor.MarkStartOfs >= 0)
    {
        memcpy(&tmpBuf[readPos], &modEntry->sampleData[s->offset], editor.MarkStartOfs);
        readPos += editor.MarkStartOfs;
    }

    // copy buffer
    memcpy(&tmpBuf[readPos], CopyBuf, CopyBufSize);

    // copy end part
    if (editor.MarkStartOfs >= 0)
    {
        readPos += CopyBufSize;

        if ((s->length - editor.MarkStartOfs) > 0)
            memcpy(&tmpBuf[readPos], &modEntry->sampleData[s->offset + editor.MarkStartOfs], s->length - editor.MarkStartOfs);
    }

    s->length = (s->length + CopyBufSize) & 0xFFFFFFFE;
    if (s->length > 0x0001FFFE)
        s->length = 0x0001FFFE;

    if (s->loopLength > 2) // loop enabled?
    {
        if (editor.MarkStartOfs > s->loopStart)
        {
            if (editor.MarkStartOfs <= (s->loopStart + s->loopLength))
            {
                // we pasted data inside the loop, disable loop
                s->loopStart  = 0;
                s->loopLength = 2;
            }

            // we pasted data after the loop, don't modify loop points
        }
        else
        {
            // we pasted data before the loop, adjust loop start point
            s->loopStart = (s->loopStart + CopyBufSize) & 0xFFFFFFFE;
            if ((s->loopStart + s->loopLength) > s->length)
            {
                s->loopStart  = 0;
                s->loopLength = 2;
            }
        }
    }

    memset(&modEntry->sampleData[s->offset],      0, 0x0001FFFE);
    memcpy(&modEntry->sampleData[s->offset], tmpBuf, s->length);

    free(tmpBuf);

    SamLength = s->length;
    if (wasZooming) // was zooming
        FillSampleBuffer();

    SetDragBar();
    SetLoopSprites();

    tmpMarkStart = editor.MarkStartOfs;
    InvertRange();

    InvertRange();
    editor.MarkStartOfs = tmpMarkStart;
    editor.MarkEndOfs   = editor.MarkStartOfs + CopyBufSize;
    OffsetToMark();
    InvertRange();

    UpdateVoiceParameters(modEntry);
    calcRealModSize(modEntry);

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);

    if (!wasZooming)
        setupSampler(modEntry, KEEP_SAMPLE_MARKING);

    editor.SamplePos = editor.MarkEndOfs;
    clampSamplePos();
}

void samplerPlayWaveform(MODULE *modEntry)
{
    int16_t tmpPeriod;

    MODULE_SAMPLE *s;

    mod_channel *chn;

    s   = &modEntry->samples[editor.currentSample];
    chn = &modEntry->channels[editor.cursor.channel];

    if (s->length == 0)
    {
        // shutdown scope
        chn->scopeLoopQuirk = false;
        chn->scopeEnabled   = false;
        chn->scopeTrigger   = false;

        // shutdown voice
        mixerKillVoice(editor.cursor.channel);
    }
    else
    {
        tmpPeriod = PeriodTable[(37 * s->fineTune) + editor.currentPlayNote];

        chn->sample          = editor.currentSample + 1;
        chn->tempPeriod      = tmpPeriod;
        chn->period          = tmpPeriod;
        chn->volume          = s->volume;
        chn->scopeLoopQuirk  = false;
        chn->scopeEnabled    = true;
        chn->scopeKeepPeriod = false;
        chn->scopeKeepVolume = false;
        chn->scopeVolume     = 0 - (s->volume / 2);
        chn->scopePeriod     = tmpPeriod / 2;
        chn->scopeLoopFlag   = (s->loopStart + s->loopLength) > 2;
        chn->scopePos        = s->offset;
        chn->scopeEnd        = s->offset + s->length;
        chn->scopeLoopBegin  = s->offset + s->loopStart;
        chn->scopeLoopEnd    = s->offset + s->loopStart + s->loopLength;

        // one-shot loop simulation (real PT didn't show this in the scopes...)
        chn->scopeLoopQuirk = false;
        if ((s->loopLength > 2) && (s->loopStart == 0))
        {
            chn->scopeLoopQuirk = chn->scopeLoopEnd;
            chn->scopeLoopEnd   = chn->scopeEnd;
        }

        mixerSetChSource(editor.cursor.channel, modEntry->sampleData + s->offset, s->length, s->loopStart, s->loopLength, 0);
        mixerSetChDelta(editor.cursor.channel, tmpPeriod);
        mixerSetChVol(editor.cursor.channel, s->volume);
    }
}

void samplerPlayDisplay(MODULE *modEntry)
{
    int16_t tmpPeriod;

    int32_t tmp;

    MODULE_SAMPLE *s;

    mod_channel *chn;

    s   = &modEntry->samples[editor.currentSample];
    chn = &modEntry->channels[editor.cursor.channel];

    if (s->length == 0)
    {
        // shutdown scope
        chn->scopeLoopQuirk = false;
        chn->scopeEnabled   = false;
        chn->scopeTrigger   = false;

        // shutdown voice
        mixerKillVoice(editor.cursor.channel);
    }
    else
    {
        tmpPeriod = PeriodTable[(37 * s->fineTune) + editor.currentPlayNote];
        tmp       = SamOff + SamOffset + (SamStart - SamOrgStart);

        chn->sample          = editor.currentSample + 1;
        chn->tempPeriod      = tmpPeriod;
        chn->period          = tmpPeriod;
        chn->volume          = s->volume;
        chn->scopeLoopQuirk  = false;
        chn->scopeEnabled    = true;
        chn->scopeKeepPeriod = false;
        chn->scopeKeepVolume = false;
        chn->scopeVolume     = 0 - (s->volume / 2);
        chn->scopePeriod     = tmpPeriod / 2;
        chn->scopeLoopFlag   = false;
        chn->scopePos        = tmp;
        chn->scopeEnd        = tmp + SamDisplay;

        mixerSetChSource(editor.cursor.channel, modEntry->sampleData + tmp, SamDisplay, 0, 0, 0);
        mixerSetChDelta(editor.cursor.channel, tmpPeriod);
        mixerSetChVol(editor.cursor.channel, s->volume);
    }
}

void samplerPlayRange(MODULE *modEntry)
{
    int16_t tmpPeriod;

    int32_t tmp;

    MODULE_SAMPLE *s;

    mod_channel *chn;

    if (editor.MarkStartOfs < 0)
    {
        displayErrorMsg("NO RANGE SELECTED");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    s   = &modEntry->samples[editor.currentSample];
    chn = &modEntry->channels[editor.cursor.channel];

    if (s->length == 0)
    {
        // shutdown scope
        chn->scopeLoopQuirk = false;
        chn->scopeEnabled   = false;
        chn->scopeTrigger   = false;

        // shutdown voice
        mixerKillVoice(editor.cursor.channel);
    }
    else
    {
        tmpPeriod = PeriodTable[(37 * s->fineTune) + editor.currentPlayNote];
        tmp       = SamOff + (SamStart - SamOrgStart) + editor.MarkStartOfs;

        chn->sample          = editor.currentSample + 1;
        chn->tempPeriod      = tmpPeriod;
        chn->period          = tmpPeriod;
        chn->volume          = s->volume;
        chn->scopeLoopQuirk  = false;
        chn->scopeEnabled    = true;
        chn->scopeKeepPeriod = false;
        chn->scopeKeepVolume = false;
        chn->scopeVolume     = 0 - (s->volume / 2);
        chn->scopePeriod     = tmpPeriod / 2;
        chn->scopeLoopFlag   = false;
        chn->scopePos        = tmp;
        chn->scopeEnd        = tmp + editor.MarkEndOfs - editor.MarkStartOfs;

        mixerSetChSource(editor.cursor.channel, modEntry->sampleData + tmp, editor.MarkEndOfs - editor.MarkStartOfs, 0, 0, 0);
        mixerSetChDelta(editor.cursor.channel, tmpPeriod);
        mixerSetChVol(editor.cursor.channel, s->volume);
    }
}

void FillSampleBuffer(void)
{
    uint16_t y;
    uint16_t i;

    uint32_t step;

    // clear sample buffer
    for (i = 0; i < (314 * 64); ++i)
        SamBuf[i] = palette[PAL_BACKGRD];

    // render sample buffer
    step = SamOffset;
    for (i = 0; i < 314; ++i)
    {
        y = (127 - SamStart[step]) / 4;

        if (i == 0)
        {
            PenX = i;
            PenY = y;
        }
        else
        {
            DrawX = PenX;
            DrawY = PenY;
            PenX  = i;
            PenY  = y;

            drawLine(SamBuf, 314, PenX, PenY, DrawX, DrawY, palette[PAL_QRDSCP]);
        }

        step = (((uint32_t)(i + 1) * SamDisplay) / 314) + SamOffset;
    }
}

void SetLoopSprites(void)
{
    MODULE_SAMPLE *s;

    if (editor.ui.samplerScreenShown)
    {
        if (tmpModEntry == NULL)
        {
            PT_DEBUGBREAK();
            return;
        }

        s = &tmpModEntry->samples[editor.currentSample];
        if ((s->loopStart + s->loopLength) > 2)
        {
            LoopOnOffFlag = 1;

            if (SamDisplay > 0)
            {
                LoopStartPos = (uint16_t)((( s->loopStart                  - SamOffset) * 314) / SamDisplay);
                LoopEndPos   = (uint16_t)((((s->loopStart + s->loopLength) - SamOffset) * 314) / SamDisplay);
            }
        }
        else
        {
            LoopOnOffFlag = 0;
            LoopStartPos  = 0;
            LoopEndPos    = 0;
        }
    }
}

void DisplaySample(void)
{
    FillSampleBuffer();

    if (editor.MarkStartOfs >= 0)
    {
        OffsetToMark();
        InvertRange();
    }

    SetDragBar();
    SetLoopSprites();
}

void samplerShowAll(void)
{
    SamOffset           = 0;
    SamDisplay          = SamLength;
    editor.MarkStartOfs = -1;
    editor.MarkEndOfs   =  0;
    editor.MarkStart    =  2;
    editor.MarkEnd      =  2;

    DisplaySample();
}

void samplerZoomOut(void)
{
    int32_t tmp;
    int32_t tmpDisplay;
    int32_t tmpOffset;

    tmpDisplay = SamDisplay;
    tmpOffset  = SamOffset;

    tmp = SamDisplay * 2;
    if (tmp > SamLength)
    {
        SamOffset  = 0;
        SamDisplay = SamLength;
    }
    else
    {
        tmpDisplay /= 2;
        if (tmpDisplay < SamOffset)
            tmpOffset -= tmpDisplay;

        tmpDisplay = tmpOffset + tmp;
        if (tmpDisplay > SamLength)
            tmpOffset  = SamLength - tmp;

        SamOffset  = tmpOffset;
        SamDisplay = tmp;
    }

    InvertRange();
    OffsetToMark();
    InvertRange();

    DisplaySample();
}

void samplerRangeAll(void)
{
    InvertRange();

    editor.MarkStart = 3;
    editor.MarkEnd   = 316;

    MarkToOffset();

    InvertRange();
}

void samplerShowRange(void)
{
    MODULE_SAMPLE *s;

    s = &tmpModEntry->samples[editor.currentSample];
    if (s->length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return;
    }

    if (editor.MarkStartOfs < 0)
    {
        displayErrorMsg("NO RANGE SELECTED");
        return;
    }

    if (editor.MarkStartOfs == editor.MarkEndOfs)
    {
        displayErrorMsg("SET LARGER RANGE");
        return;
    }

    SamDisplay          = (editor.MarkStartOfs >= editor.MarkEndOfs) ? 1 : (editor.MarkEndOfs - editor.MarkStartOfs);
    SamOffset           =  editor.MarkStartOfs;
    editor.MarkStartOfs = -1;
    editor.MarkEndOfs   =  0;
    editor.MarkStart    =  2;
    editor.MarkEnd      =  2;

    DisplaySample();
}

void volBoxBarPressed(int8_t mouseButtonHeld)
{
    int32_t mx;

    if (!mouseButtonHeld)
    {
        if ((input.mouse.x >= 72) && (input.mouse.x <= 173))
        {
            if ((input.mouse.y >= 154) && (input.mouse.y <= 174)) editor.ui.forceVolDrag = 1;
            if ((input.mouse.y >= 165) && (input.mouse.y <= 175)) editor.ui.forceVolDrag = 2;
        }
    }
    else
    {
        mx = CLAMP(input.mouse.x - 107, 0, 60);

             if (editor.ui.forceVolDrag == 1) editor.Vol1 = (uint16_t)((mx * 200) / 60);
        else if (editor.ui.forceVolDrag == 2) editor.Vol2 = (uint16_t)((mx * 200) / 60);
    }
}

void samplerBarPressed(int8_t mouseButtonHeld)
{
    int32_t tmp;

    if (!mouseButtonHeld)
    {
        if ((input.mouse.x >= 4) && (input.mouse.x <= 315))
        {
            if (input.mouse.x < DragStart)
            {
                tmp = SamOffset - SamDisplay;
                if (tmp < 0)
                    tmp = 0;

                if (tmp == SamOffset)
                    return;

                SamOffset = tmp;

                DisplaySample();

                return;
            }

            if (input.mouse.x > DragEnd)
            {
                tmp = SamOffset + SamDisplay;
                if ((tmp + SamDisplay) <= SamLength)
                {
                    if (tmp == SamOffset)
                        return;

                    SamOffset = tmp;
                }
                else
                {
                    tmp = SamLength - SamDisplay;
                    if (tmp == SamOffset)
                        return;

                    SamOffset = tmp;
                }

                DisplaySample();

                return;
            }

            SaveMX     = (uint16_t)(input.mouse.x) - DragStart;
            LastSamPos = SaveMX;

            editor.ui.forceSampleDrag = true;
        }
    }

    if (input.mouse.x != LastSamPos)
    {
        LastSamPos = (uint16_t)(input.mouse.x);

        tmp = LastSamPos - SaveMX - 4;
        if (tmp < 0)
            tmp = 0;

        tmp = (tmp * SamLength) / 311;
        if ((tmp + SamDisplay) <= SamLength)
        {
            if (tmp == SamOffset)
                return;

            SamOffset = tmp;
        }
        else
        {
            tmp = SamLength - SamDisplay;
            if (tmp == SamOffset)
                return;

            SamOffset = tmp;
        }

        DisplaySample();
    }
}

void samplerEditSample(int8_t mouseButtonHeld)
{
    int8_t y;

    int32_t x;
    int32_t smp_x0;
    int32_t smp_x1;
    int32_t xDistance;
    int32_t smp_y0;
    int32_t smp_y1;
    int32_t yDistance;

    MODULE_SAMPLE *s;

    if (tmpModEntry == NULL)
    {
        PT_DEBUGBREAK();
        return;
    }

    s = (MODULE_SAMPLE *)(&tmpModEntry->samples[editor.currentSample]);

    // EDIT SAMPLE ROUTINE (non-PT feature inspired by FT2)
    if (!mouseButtonHeld)
    {
        if ((input.mouse.x >= 3) && (input.mouse.x <= 316) && (input.mouse.y >= 138) && (input.mouse.y <= 201))
        {
            if (s->length == 0)
            {
                displayErrorMsg("SAMPLE LENGTH = 0");
                return;
            }

            lastMouseX = input.mouse.x;
            lastMouseY = input.mouse.y;

            editor.ui.forceSampleEdit = true;
        }

        return;
    }

    x = CLAMP((((input.mouse.x - 3) * SamDisplay) / 314) + SamOffset, 0, s->length - 1);

    y = (int8_t)(CLAMP(-(input.mouse.y - 169) * 4, -128, 127));
    if (y >= 124)
        y  = 127;

    if (!input.keyb.shiftKeyDown)
        tmpModEntry->sampleData[s->offset + x] = y;

    // interpolate x gaps
    if (input.mouse.x != lastMouseX)
    {
        smp_y0 = CLAMP(-(lastMouseY - 169) * 4, -128, 127);
        if (smp_y0 >= 124)
            smp_y0  = 127;

        smp_y1    = y;
        yDistance = smp_y1 - smp_y0;

        if (input.mouse.x > lastMouseX)
        {
            smp_x0    = CLAMP((((lastMouseX - 3) * SamDisplay) / 314) + SamOffset, 0, s->length - 1);
            smp_x1    = x;
            xDistance = smp_x1 - smp_x0;

            if (xDistance != 0)
            {
                for (x = smp_x0; x < smp_x1; ++x)
                    tmpModEntry->sampleData[s->offset + x] = (int8_t)(smp_y0 + ((x - smp_x0) / xDistance) * yDistance);
            }
        }
        else if (input.mouse.x < lastMouseX)
        {
            smp_x0    = x;
            smp_x1    = CLAMP((((lastMouseX - 3) * SamDisplay) / 314) + SamOffset, 0, s->length - 1);
            xDistance = smp_x1 - smp_x0;

            if (xDistance != 0)
            {
                for (x = smp_x0; x < smp_x1; ++x)
                    tmpModEntry->sampleData[s->offset + x] = (int8_t)(smp_y0 + ((x - smp_x0) / xDistance) * yDistance);
            }
        }

        lastMouseX = input.mouse.x;

        if (!input.keyb.shiftKeyDown)
            lastMouseY = input.mouse.y;
    }

    editor.MarkStartOfs = -1;
    editor.MarkEndOfs   =  0;
    editor.MarkStart    =  2;
    editor.MarkEnd      =  2;

    FillSampleBuffer();

    updateWindowTitle(tmpModEntry, MOD_IS_MODIFIED);
}

void samplerSamplePressed(int8_t mouseButtonHeld)
{
    int16_t mouseX;

    int32_t tmp;

    MODULE_SAMPLE *s;

    if (tmpModEntry == NULL)
    {
        PT_DEBUGBREAK();
        return;
    }

    if (!mouseButtonHeld)
    {
        if (input.mouse.y < 142)
        {
            if ((input.mouse.x >= LoopStartPos) && (input.mouse.x <= (LoopStartPos + 3)))
            {
                editor.ui.leftLoopPinMoving  = true;
                editor.ui.rightLoopPinMoving = false;
                editor.ui.sampleMarkingPos   = 1;
            }
            else if ((input.mouse.x >= (LoopEndPos + 3)) && (input.mouse.x <= (LoopEndPos + 6)))
            {
                editor.ui.rightLoopPinMoving = true;
                editor.ui.leftLoopPinMoving  = false;
                editor.ui.sampleMarkingPos   = 1;
            }
        }
    }

    mouseX = (int16_t)(input.mouse.x);

    // TODO: When moving the left loop pin and dragging it quickly to the right,
    //       the right loop pin moves towards the left... Fix this!
    if (editor.ui.leftLoopPinMoving)
    {
        s = (MODULE_SAMPLE *)(&tmpModEntry->samples[editor.currentSample]);

        if (mouseX > 314)
            mouseX = 314;

        tmp = s->loopStart;

        s->loopStart = (((mouseX * SamDisplay) / 314) + SamOffset) & 0xFFFFFFFE;

        if (s->loopStart > tmp)
        {
            s->loopLength -= (s->loopStart - tmp);
            if (s->loopLength < 2)
            {
                s->loopStart  = tmp;
                s->loopLength = 2;
            }
        }
        else if (s->loopStart < tmp)
        {
            s->loopLength += (tmp - s->loopStart);
        }

        UpdateVoiceParameters(tmpModEntry);
        SetLoopSprites();

        updateWindowTitle(tmpModEntry, MOD_IS_MODIFIED);

        return;
    }

    if (editor.ui.rightLoopPinMoving)
    {
        s = (MODULE_SAMPLE *)(&tmpModEntry->samples[editor.currentSample]);

        mouseX = CLAMP(mouseX - 3, 0, 314);

        s->loopLength = ((((mouseX * SamDisplay) / 314) + SamOffset) & 0xFFFFFFFE) - s->loopStart;
        if (s->loopLength < 2)
            s->loopLength = 2;

        UpdateVoiceParameters(tmpModEntry);
        SetLoopSprites();

        updateWindowTitle(tmpModEntry, MOD_IS_MODIFIED);

        return;
    }

    if (!mouseButtonHeld)
    {
        if ((mouseX < 3) || (mouseX > 316))
            return;

        editor.ui.sampleMarkingPos = (int16_t)(mouseX);
        LastSamPos                 = editor.ui.sampleMarkingPos;

        InvertRange();
        editor.MarkStart = LastSamPos;
        editor.MarkEnd   = LastSamPos;
        InvertRange();

        MarkToOffset();

        if (tmpModEntry->samples[editor.currentSample].length == 0)
            editor.SamplePos = 0;
        else
            editor.SamplePos = (((mouseX - 3) * SamDisplay) / 314) + SamOffset;

        return;
    }

    mouseX = CLAMP(mouseX, 3, 316);

    if (mouseX != LastSamPos)
    {
        LastSamPos = (uint16_t)(mouseX);

        InvertRange();

        if (LastSamPos > editor.ui.sampleMarkingPos)
        {
            editor.MarkStart = editor.ui.sampleMarkingPos;
            editor.MarkEnd   = LastSamPos;
        }
        else if (LastSamPos <= editor.ui.sampleMarkingPos)
        {
            editor.MarkStart = LastSamPos;
            editor.MarkEnd   = editor.ui.sampleMarkingPos;
        }

        InvertRange();
    }

    MarkToOffset();

    if (tmpModEntry->samples[editor.currentSample].length == 0)
        editor.SamplePos = 0;
    else
        editor.SamplePos = (((mouseX - 3) * SamDisplay) / 314) + SamOffset;
}

void removeTempLoopPoints(void)
{
    tmpLoopStart  = 0;
    tmpLoopLength = 2;
}

void removeSampleDataMarking(void)
{
    editor.MarkStartOfs = -1;
    editor.MarkEndOfs   =  0;
    editor.MarkStart    =  2;
    editor.MarkEnd      =  2;
}

// This one is *ONLY* used for the volume tool box!
void UpdateSample(void)
{
    FillSampleBuffer();
    OffsetToMark();
    InvertRange();
}

void RedrawSample(int8_t keepMarking)
{
    MODULE_SAMPLE *s;

    if (tmpModEntry == NULL)
    {
        PT_DEBUGBREAK();
        return;
    }

    if (!keepMarking)
    {
        editor.MarkStartOfs = -1;
        editor.MarkEndOfs   =  0;
        editor.MarkStart    =  2;
        editor.MarkEnd      =  2;
    }

    s = (MODULE_SAMPLE *)(&tmpModEntry->samples[editor.currentSample]);

    if (s->length > 0)
    {
        SamOff      = s->offset;
        SamOrgStart = &tmpModEntry->sampleData[s->offset];
        SamStart    = &tmpModEntry->sampleData[s->offset];
        SamDisplay  = s->length;
        SamLength   = s->length;
    }
    else
    {
        SamStart   = BlankSample;
        SamDisplay = 314;
        SamLength  = 314;
    }

    SamOffset = 0;

    FillSampleBuffer();
    SetDragBar();
    SetLoopSprites();

    if (keepMarking)
    {
        OffsetToMark();
        InvertRange();
    }

    recalcChordLength(tmpModEntry);
}

void samplerLoopToggle(void)
{
    MODULE_SAMPLE *s;

    if (tmpModEntry == NULL)
    {
        PT_DEBUGBREAK();
        return;
    }

    s = (MODULE_SAMPLE *)(&tmpModEntry->samples[editor.currentSample]);
    if (s->length == 0)
        return;

    mixerKillVoiceIfReadingSample(tmpModEntry, editor.currentSample);

    if (LoopOnOffFlag)
    {
        tmpLoopStart  = s->loopStart;
        tmpLoopLength = s->loopLength;
        s->loopStart  = 0;
        s->loopLength = 2;
    }
    else
    {
        s->loopStart  = tmpLoopStart;
        s->loopLength = tmpLoopLength;

        if (s->loopLength <= 2)
        {
            s->loopLength = s->length;
            if (s->loopLength < 2)
                s->loopLength = 2;
        }

        if (s->loopStart >= s->length)
            s->loopStart  = s->length - 1;

        while ((s->loopStart + s->loopLength) > s->length)
            s->loopLength--;

        if (s->loopLength <= 2)
        {
            s->loopStart  = 0;
            s->loopLength = s->length;
        }
    }

    UpdateVoiceParameters(tmpModEntry);
    DisplaySample();

    updateWindowTitle(tmpModEntry, MOD_IS_MODIFIED);
}

void setupSampler(MODULE *modEntry, int8_t keepMarking)
{
    MODULE_SAMPLE *s;

    editor.ui.lastSampleOffset = 0x900;

    tmpModEntry = (MODULE *)(modEntry);

    s = (MODULE_SAMPLE *)(&modEntry->samples[editor.currentSample]);
    if (editor.SamplePos > s->length)
        editor.SamplePos = s->length;

    if (editor.ui.samplerScreenShown)
        RedrawSample(keepMarking);

    recalcChordLength(modEntry);
}

void drawSampler(uint32_t *frameBuffer, MODULE *modEntry)
{
    uint8_t i;

    uint16_t y;
    uint16_t len;
    uint16_t x;

    int32_t sampleReadPos;
    int32_t putX;
    const uint32_t *ptr32_src;
    uint32_t *ptr32_dst;
    uint32_t pix;

    // copy waveform buffer to screen
    ptr32_src = SamBuf;
    ptr32_dst = frameBuffer + ((138 * SCREEN_W) + 3);

    y = 64;
    while (y--)
    {
        memcpy(ptr32_dst, ptr32_src, 314 * sizeof (int32_t));

        ptr32_src += 314;
        ptr32_dst += SCREEN_W;
    }

    printSixDecimals(frameBuffer, 264, 214, (SamStart == BlankSample) ? 0 : SamDisplay, palette[PAL_GENTXT]);

    textOut(frameBuffer, 288, 225, LoopOnOffFlag ? "ON" : "OFF", palette[PAL_GENTXT]);

    if (editor.ui.changingSmpResample)
        textOut(frameBuffer, 288, 236, "---", palette[PAL_GENTXT]);
    else
        textOut(frameBuffer, 288, 236, editor.Accidental ? NoteNames2[editor.ResampleNote] : NoteNames1[editor.ResampleNote], palette[PAL_GENTXT]);

    // DRAW LOOP POINT PINS

    if (LoopOnOffFlag)
    {
        pix = palette[PAL_COLORKEY];

        // right loop pin (continue source buffer)
        if (LoopEndPos <= 314)
        {
            ptr32_src = loopPinsBitmap + (64 * 4);
            ptr32_dst = &frameBuffer[(138 * SCREEN_W) + (LoopEndPos + 3)];
            y         = 64;

            while (y--)
            {
                if (*(ptr32_src + 0) != pix) *(ptr32_dst + 0) = *(ptr32_src + 0);
                if (*(ptr32_src + 1) != pix) *(ptr32_dst + 1) = *(ptr32_src + 1);
                if (*(ptr32_src + 2) != pix) *(ptr32_dst + 2) = *(ptr32_src + 2);
                if (*(ptr32_src + 3) != pix) *(ptr32_dst + 3) = *(ptr32_src + 3);

                ptr32_src += 4;
                ptr32_dst += SCREEN_W;
            }

            // dirty hack to clear SCREEN_W overrun from right loop pin
            if (LoopEndPos == 314)
            {
                frameBuffer[139 * SCREEN_W] = palette[PAL_BORDER];
                frameBuffer[140 * SCREEN_W] = palette[PAL_BORDER];
                frameBuffer[141 * SCREEN_W] = palette[PAL_BORDER];
                frameBuffer[142 * SCREEN_W] = palette[PAL_BORDER];
            }
        }

        // left loop pin
        if (LoopStartPos <= 314)
        {
            ptr32_src = loopPinsBitmap;
            ptr32_dst = &frameBuffer[(138 * SCREEN_W) + LoopStartPos];
            y         = 64;

            while (y--)
            {
                if (*(ptr32_src + 0) != pix) *(ptr32_dst + 0) = *(ptr32_src + 0);
                if (*(ptr32_src + 1) != pix) *(ptr32_dst + 1) = *(ptr32_src + 1);
                if (*(ptr32_src + 2) != pix) *(ptr32_dst + 2) = *(ptr32_src + 2);
                if (*(ptr32_src + 3) != pix) *(ptr32_dst + 3) = *(ptr32_src + 3);

                ptr32_src += 4;
                ptr32_dst += SCREEN_W;
            }
        }
    }

    // DRAW SAMPLE POSITION LINE
    if (SamDisplay > 0) // prevent division by zero
    {
        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            if (modEntry->channels[i].scopeEnabled)
            {
                if (modEntry->channels[i].sample == (editor.currentSample + 1))
                {
                    sampleReadPos = modEntry->channels[i].scopePos - modEntry->samples[editor.currentSample].offset;

                    putX = (((sampleReadPos - SamOffset) * 314) / SamDisplay) + 3;
                    if ((putX >= 3) && (putX < 317))
                    {
                        ptr32_dst = &frameBuffer[(138 * SCREEN_W) + putX];

                        y = 64;
                        while (y--)
                        {
                            *ptr32_dst  = 0xFF00FFFF; // this color is not in the palette
                             ptr32_dst += SCREEN_W;
                        }
                    }
                }
            }
        }
    }

    // SAMPLE DRAG BAR
    if ((SamLength > 0) && (SamDisplay != SamLength))
    {
        ptr32_dst = &frameBuffer[(206 * SCREEN_W) + DragStart];
        len       = DragEnd - DragStart;
        pix       = palette[PAL_QRDSCP];

        x = len;
        while (x--)
        {
            *(ptr32_dst + (SCREEN_W * 0)) = pix;
            *(ptr32_dst + (SCREEN_W * 1)) = pix;
            *(ptr32_dst + (SCREEN_W * 2)) = pix;
            *(ptr32_dst + (SCREEN_W * 3)) = pix;

            ptr32_dst++;
        }
    }

    if (editor.ui.samplerVolBoxShown || (modEntry->samples[editor.currentSample].length > 0x0000FFFE))
    {
        // 9xx on >64kB samples makes no sound (PT replayer bug), so display no sample offset
        textOut(frameBuffer, 288, 247, "---" , palette[PAL_GENTXT]);
        return;
    }

    // update sample offset
    if ((input.mouse.x >= 3) && (input.mouse.x <= 316) && (input.mouse.y >= 138) && (input.mouse.y <= 201))
    {
        if (modEntry->samples[editor.currentSample].length > 0)
            editor.ui.lastSampleOffset = 0x0900 | (uint16_t)(((((input.mouse.x - 3) * SamDisplay) / 314) + SamOffset) / 256);
    }

    // display sample offset
    if (editor.ui.lastSampleOffset <= 0x0900)
        textOut(frameBuffer, 288, 247, "---", palette[PAL_GENTXT]);
    else if (editor.ui.lastSampleOffset <= 0x09FF)
        printThreeHex(frameBuffer, 288, 247, editor.ui.lastSampleOffset, palette[PAL_GENTXT]);
}
