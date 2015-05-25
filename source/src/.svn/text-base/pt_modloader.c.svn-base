/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h> // for toupper()/tolower()
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pt_palette.h"
#include "pt_header.h"
#include "pt_sampler.h"
#include "pt_helpers.h"
#include "pt_audio.h"

typedef struct
{
    int8_t _eof;
    uint8_t *_ptr;
    uint8_t *_base;
    uint32_t _cnt;
    uint32_t _bufsiz;
} MEM;

static MEM *mopen(const uint8_t *src, uint32_t length);
static void mclose(MEM **buf);
static int32_t mgetc(MEM *buf);
static size_t mread(void *buffer, size_t size, size_t count, MEM *buf);
static void mseek(MEM *buf, int32_t offset, int32_t whence);

uint8_t ppdecrunch(uint8_t *src, uint8_t *dest, uint8_t *offset_lens, uint32_t src_len, uint32_t dest_len, uint8_t skip_bits);

MODULE *createNewMod(void)
{
    uint8_t i;

    MODULE *newMod;

    newMod = (MODULE *)(calloc(1, sizeof (MODULE)));
    if (newMod == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    for (i = 0; i < MAX_PATTERNS; ++i)
    {
        newMod->patterns[i] = (modnote_t *)(calloc(MOD_ROWS * PAULA_CHANNELS, sizeof (modnote_t)));
        if (newMod->patterns[i] == NULL)
        {
            fprintf(stderr, "ERROR: Out of memory!\n");
            return (false);
        }
    }

    newMod->sampleData = (int8_t *)(calloc(MOD_SAMPLES, 0x0001FFFE));
    if (newMod->sampleData == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    newMod->head.orderCount   = 1;
    newMod->head.patternCount = 1;

    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        newMod->samples[i].offset     = 0x0001FFFE * i;
        newMod->samples[i].loopLength = 2;

        // set display pointers (very important)
        newMod->samples[i].volumeDisp     = &newMod->samples[i].volume;
        newMod->samples[i].lengthDisp     = &newMod->samples[i].length;
        newMod->samples[i].loopStartDisp  = &newMod->samples[i].loopStart;
        newMod->samples[i].loopLengthDisp = &newMod->samples[i].loopLength;
    }

    // set display pointers (very important)
    editor.currentEditPatternDisp = &newMod->currPattern;
    editor.currentPosDisp         = &newMod->currOrder;
    editor.currentPatternDisp     = &newMod->head.order[0];
    editor.currentPosEdPattDisp   = &newMod->head.order[0];
    editor.currentLengthDisp      = &newMod->head.orderCount;

    calcRealModSize(newMod);

    return (newMod);
}

int8_t modSave(MODULE *modEntry, char *fileName)
{
    int16_t tempPatternCount;

    uint32_t tempLoopLength;
    uint32_t tempLoopStart;
    uint32_t j;
    uint32_t k;
    uint32_t i;

    FILE *fmodule;

    tempPatternCount = 0;

    fmodule = fopen(fileName, "wb");
    if (fmodule == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    for (i = 0; i < 20; ++i)
        fputc((char)(tolower(modEntry->head.moduleTitle[i])), fmodule);

    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        fwrite(modEntry->samples[i].text, 1, 22, fmodule);
        fputc((uint32_t)(modEntry->samples[i].length) >> 9, fmodule);
        fputc((uint32_t)(modEntry->samples[i].length) >> 1, fmodule);
        fputc(modEntry->samples[i].fineTune & 0x0F, fmodule);
        fputc((modEntry->samples[i].volume > 64) ? 64 : modEntry->samples[i].volume, fmodule);

        tempLoopLength = (uint32_t)(modEntry->samples[i].loopLength);
        if (tempLoopLength < 2)
            tempLoopLength = 2;

        tempLoopStart = (uint32_t)(modEntry->samples[i].loopStart);
        if (tempLoopLength == 2)
            tempLoopStart = 0;

        fputc(tempLoopStart  >> 9, fmodule);
        fputc(tempLoopStart  >> 1, fmodule);
        fputc(tempLoopLength >> 9, fmodule);
        fputc(tempLoopLength >> 1, fmodule);
    }

    fputc(modEntry->head.orderCount & 0x00FF, fmodule);
    fputc(0x7F, fmodule); // ProTracker puts 0x7F at this place (restart pos)

    for (i = 0; i < MOD_ORDERS; ++i)
        fputc(modEntry->head.order[i] & 0x00FF, fmodule);

    tempPatternCount = 0;
    for (i = 0; i < MOD_ORDERS; ++i)
    {
        if (tempPatternCount < modEntry->head.order[i])
            tempPatternCount = modEntry->head.order[i];
    }

    if (++tempPatternCount > MAX_PATTERNS)
          tempPatternCount = MAX_PATTERNS;

    fwrite((tempPatternCount <= 64) ? "M.K." : "M!K!", 1, 4, fmodule);

    for (i = 0; i < (uint32_t)(tempPatternCount); ++i)
    {
        for (j = 0; j < MOD_ROWS; ++j)
        {
            for (k = 0; k < PAULA_CHANNELS; ++k)
            {
                modnote_t tmp;

                tmp = modEntry->patterns[i][(j * PAULA_CHANNELS) + k];

                fputc(((tmp.period >> 8) & 0x0F) | (tmp.sample & 0x10), fmodule);
                fputc(tmp.period & 0xFF, fmodule);
                fputc((tmp.sample << 4) | (tmp.command & 0x0F), fmodule);
                fputc(tmp.param, fmodule);
            }
        }
    }

    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        // Amiga ProTracker "BEEEEEEEEP" fix
        if ((modEntry->samples[i].length >= 2) && (modEntry->samples[i].loopLength == 2))
        {
            fputc(0, fmodule);
            fputc(0, fmodule);

            k = modEntry->samples[i].length;
            for (j = 2; j < k; ++j)
                fputc(modEntry->sampleData[modEntry->samples[i].offset + j], fmodule);
        }
        else
        {
            fwrite(modEntry->sampleData + (0x0001FFFE * i), 1, modEntry->samples[i].length, fmodule);
        }
    }

    editor.currentSample = 0;

    fclose(fmodule);

    displayMsg("MODULE SAVED !");
    setMsgPointer();

    editor.diskop.cached = false;
    updateWindowTitle(modEntry, MOD_NOT_MODIFIED);

    return (true);
}

static int8_t checkModType(const char *buf)
{
         if (!strncmp(buf, "M.K.", 4)) return (FORMAT_MK);   // ProTracker v1.x, handled as ProTracker v2.x
    else if (!strncmp(buf, "M!K!", 4)) return (FORMAT_MK2);  // ProTracker v2.x (if >64 patterns)
    else if (!strncmp(buf, "FLT4", 4)) return (FORMAT_FLT4); // StarTrekker (4ch), handled as ProTracker v2.x
    else if (!strncmp(buf, "4CHN", 4)) return (FORMAT_4CHN); // FastTracker II (4ch), handled as ProTracker v2.x
    else if (!strncmp(buf, "N.T.", 4)) return (FORMAT_MK);   // NoiseTracker 1.0, handled as ProTracker v2.x

    return (FORMAT_UNKNOWN); // may be The Ultimate SoundTracker, 15 samples
}

MODULE *modLoad(const char *fileName)
{
    char modSig[4];
    char tmpChar;
    int8_t mightBeSTK;
    int8_t numSamples;
    int8_t lateVerSTKFlag;
    uint8_t ppCrunchData[4];
    uint8_t bytes[4];
    uint8_t *ppBuffer;
    uint8_t *modBuffer;
    uint8_t ch;
    uint8_t row;
    uint8_t i;
    uint8_t pattern;

    int32_t tmp;
    uint32_t j;
    uint32_t PP20;
    uint32_t ppPackLen;
    uint32_t ppUnpackLen;

    FILE *fmodule;

    MODULE *newModule;

    MODULE_SAMPLE *s;

    modnote_t *note;

    MEM *mod;

    lateVerSTKFlag = false;
    mightBeSTK     = false;

    newModule = (MODULE *)(calloc(1, sizeof (MODULE)));
    if (newModule == NULL)
    {
        displayErrorMsg("OUT OF MEMORY !!!");
        return (NULL);
    }

    fmodule = fopen(fileName, "rb");
    if (fmodule == NULL)
    {
        free(newModule);
        newModule = NULL;

        displayErrorMsg("FILE I/O ERROR !");

        return (NULL);
    }

    fseek(fmodule, 0, SEEK_END);
    newModule->head.moduleSize = ftell(fmodule);
    fseek(fmodule, 0, SEEK_SET);

    // check if mod is a powerpacker mod
    fread(&PP20, 4, 1, fmodule); if (bigEndian) PP20 = SWAP32(PP20);
    if (PP20 == 0x30325850) // "PX20"
    {
        free(newModule);
        fclose(fmodule);

        displayErrorMsg("ENCRYPTED PPACK !");

        return (NULL);
    }
    else if (PP20 == 0x30325050) // "PP20"
    {
        ppPackLen = newModule->head.moduleSize;
        if (ppPackLen & 3)
        {
            free(newModule);
            fclose(fmodule);

            displayErrorMsg("POWERPACKER ERROR");

            return (NULL);
        }

        fseek(fmodule, ppPackLen - 4, SEEK_SET);

        ppCrunchData[0] = (uint8_t)(fgetc(fmodule));
        ppCrunchData[1] = (uint8_t)(fgetc(fmodule));
        ppCrunchData[2] = (uint8_t)(fgetc(fmodule));
        ppCrunchData[3] = (uint8_t)(fgetc(fmodule));

        ppUnpackLen = (ppCrunchData[0] << 16) | (ppCrunchData[1] << 8) | ppCrunchData[2];

        // smallest and biggest possible .MOD
        if ((ppUnpackLen < 2108) || (ppUnpackLen > 4195326))
        {
            free(newModule);
            fclose(fmodule);

            displayErrorMsg("NOT A MOD FILE !");

            return (NULL);
        }

        ppBuffer = (uint8_t *)(malloc(ppPackLen));
        if (ppBuffer == NULL)
        {
            free(newModule);
            fclose(fmodule);

            displayErrorMsg("OUT OF MEMORY !!!");

            return (NULL);
        }

        modBuffer = (uint8_t *)(malloc(ppUnpackLen));
        if (modBuffer == NULL)
        {
            free(newModule);
            free(ppBuffer);

            fclose(fmodule);

            displayErrorMsg("OUT OF MEMORY !!!");

            return (NULL);
        }

        fseek(fmodule, 0, SEEK_SET);
        fread(ppBuffer, 1, ppPackLen, fmodule);
        fclose(fmodule);

        ppdecrunch(ppBuffer + 8, modBuffer, ppBuffer + 4, ppPackLen - 12, ppUnpackLen, ppCrunchData[3]);
        free(ppBuffer);

        newModule->head.moduleSize = ppUnpackLen;
    }
    else
    {
        // smallest and biggest possible .MOD
        if ((newModule->head.moduleSize < 2108) || (newModule->head.moduleSize > 4195326))
        {
            free(newModule);
            fclose(fmodule);

            displayErrorMsg("NOT A MOD FILE !");

            return (NULL);
        }

        modBuffer = (uint8_t *)(malloc(newModule->head.moduleSize));
        if (modBuffer == NULL)
        {
            free(newModule);
            fclose(fmodule);

            displayErrorMsg("OUT OF MEMORY !!!");
            return (NULL);
        }

        fseek(fmodule, 0, SEEK_SET);
        fread(modBuffer, 1, newModule->head.moduleSize, fmodule);
        fclose(fmodule);
    }

    mod = mopen(modBuffer, newModule->head.moduleSize);
    if (mod == NULL)
    {
        free(modBuffer);
        free(newModule);

        displayErrorMsg("FILE I/O ERROR !");

        return (NULL);
    }

    // check module tag
    mseek(mod, 0x0438, SEEK_SET);
    mread(modSig, 1, 4, mod);

    newModule->head.format = checkModType(modSig);
    if (newModule->head.format == FORMAT_UNKNOWN)
        mightBeSTK = true;

    mseek(mod, 0, SEEK_SET);

    mread(newModule->head.moduleTitle, 1, 20, mod);

    for (i = 0; i < 20; ++i)
    {
        tmpChar = newModule->head.moduleTitle[i];
        if (((tmpChar < ' ') || (tmpChar > '~')) && (tmpChar != '\0'))
            tmpChar = ' ';

        newModule->head.moduleTitle[i] = (char)(tolower(tmpChar));
    }

    // read sample information
    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        s = &newModule->samples[i];

        if (mightBeSTK && (i > 14))
        {
            s->loopLength = 2;
        }
        else
        {
            mread(s->text, 1, 22, mod);

            for (j = 0; j < 22; ++j)
            {
                tmpChar = s->text[j];
                if (((tmpChar < ' ') || (tmpChar > '~')) && (tmpChar != '\0'))
                    tmpChar = ' ';

                s->text[j] = (char)(tolower(tmpChar));
            }

            s->length = (int32_t)((mgetc(mod) << 8) | mgetc(mod)) * 2;
            if (s->length > 9999)
                lateVerSTKFlag = true; // Only used if mightBeSTK is set

            s->fineTune = (uint8_t)(mgetc(mod)) & 0x0F;

            s->volume = (uint8_t)(mgetc(mod));
            if (s->volume > 64)
                s->volume = 64;

            s->loopStart = (int32_t)((mgetc(mod) << 8) | mgetc(mod)) * 2;
            if (mightBeSTK)
                s->loopStart /= 2;

            s->loopLength = (int32_t)((mgetc(mod) << 8) | mgetc(mod)) * 2;
            if (s->loopLength < 2)
                s->loopLength = 2;

            // fix for poorly converted STK->PTMOD modules.
            if (!mightBeSTK && ((s->loopStart + s->loopLength) > s->length))
            {
                if (((s->loopStart / 2) + s->loopLength) <= s->length)
                {
                    s->loopStart /= 2;
                }
                else
                {
                    s->loopStart  = 0;
                    s->loopLength = 2;
                }
            }

            if (mightBeSTK)
            {
                if (s->loopLength > 2)
                {
                    tmp = s->loopStart;

                    s->length      -= s->loopStart;
                    s->loopStart    = 0;
                    s->tmpLoopStart = tmp;
                }

                // No finetune in STK/UST
                s->fineTune = 0;
            }
        }
    }

    // STK 2.5 had loopStart in words, not bytes. Convert if late version STK.
    for (i = 0; i < 15; ++i)
    {
        if (mightBeSTK && lateVerSTKFlag)
        {
            s = &newModule->samples[i];
            if (s->loopStart > 2)
            {
                s->length       -= s->tmpLoopStart;
                s->tmpLoopStart *= 2;
            }
        }
    }

    newModule->head.orderCount = (int16_t)(mgetc(mod));

    // fixes beatwave.mod (129 orders) and other weird MODs
    if (newModule->head.orderCount > 127)
    {
        if (newModule->head.orderCount > 129)
        {
            mclose(&mod);

            free(modBuffer);
            free(newModule);

            displayErrorMsg("NOT A MOD FILE !");

            return (NULL);
        }

        newModule->head.orderCount = 127;
    }

    if (newModule->head.orderCount == 0)
    {
        mclose(&mod);

        free(modBuffer);
        free(newModule);

        displayErrorMsg("NOT A MOD FILE !");

        return (NULL);
    }

    newModule->head.restartPos = (uint8_t)(mgetc(mod));
    if (mightBeSTK && ((newModule->head.restartPos == 0) || (newModule->head.restartPos > 220)))
    {
        mclose(&mod);

        free(modBuffer);
        free(newModule);

        displayErrorMsg("NOT A MOD FILE !");

        return (NULL);
    }

    if (mightBeSTK)
    {
        // If we're still here at this point and the mightBeSTK flag is set,
        // then it's definitely a proper The Ultimate SoundTracker (STK) module.

        newModule->head.format = FORMAT_STK;

        if (newModule->head.restartPos == 120)
        {
            newModule->head.restartPos = PT_STD_BPM;
        }
        else
        {
            if (newModule->head.restartPos > 239)
                newModule->head.restartPos = 239;

            // max BPM: 14536 (there was no clamping originally, sick!)
            newModule->head.initBPM = (uint16_t)(1773447 / ((240 - newModule->head.restartPos) * 122));
        }
    }

    for (i = 0; i < MOD_ORDERS; ++i)
    {
        newModule->head.order[i] = (int16_t)(mgetc(mod));
        if (newModule->head.order[i] > newModule->head.patternCount)
            newModule->head.patternCount = newModule->head.order[i];
    }

    if (++newModule->head.patternCount > MAX_PATTERNS)
    {
        mclose(&mod);

        free(modBuffer);
        free(newModule);

        displayErrorMsg("NOT A MOD FILE !");

        return (NULL);
    }

    if (newModule->head.format != FORMAT_STK) // The Ultimate SoundTracker MODs doesn't have this tag
        mseek(mod, 4, SEEK_CUR); // We already read/tested the tag earlier, skip it

    // init 100 patterns and load patternCount of patterns
    for (pattern = 0; pattern < MAX_PATTERNS; ++pattern)
    {
        newModule->patterns[pattern] = (modnote_t *)(calloc(MOD_ROWS * PAULA_CHANNELS, sizeof (modnote_t)));
        if (newModule->patterns[pattern] == NULL)
        {
            mclose(&mod);

            free(modBuffer);

            for (i = 0; i < pattern; ++i)
                free(newModule->patterns[i]);

            free(newModule);

            displayErrorMsg("OUT OF MEMORY !!!");

            return (NULL);
        }
    }

    for (pattern = 0; pattern < newModule->head.patternCount; ++pattern)
    {
        note = newModule->patterns[pattern];
        for (row = 0; row < MOD_ROWS; ++row)
        {
            for (ch = 0; ch < PAULA_CHANNELS; ++ch)
            {
                bytes[0] = (uint8_t)(mgetc(mod));
                bytes[1] = (uint8_t)(mgetc(mod));
                bytes[2] = (uint8_t)(mgetc(mod));
                bytes[3] = (uint8_t)(mgetc(mod));

                note->period  = ((bytes[0] & 0x0F) << 8) | bytes[1];
                note->sample  =  (bytes[0] & 0xF0) | (bytes[2] >> 4); // Don't (!) clamp, the player checks for invalid sample data
                note->command =   bytes[2] & 0x0F;
                note->param   =   bytes[3];

                if (mightBeSTK)
                {
                    // Convert STK effects to PT effects
                    if (!lateVerSTKFlag)
                    {
                        if (note->command == 0x01)
                        {
                            // Arpeggio
                            note->command = 0x00;
                        }
                        else if (note->command == 0x02)
                        {
                            // Pitch slide
                            if (note->param & 0xF0)
                            {
                                note->command = 0x02;
                                note->param >>= 4;
                            }
                            else if (note->param & 0x0F)
                            {
                                note->command = 0x01;
                            }
                        }
                    }

                    // Volume slide/pattern break
                    if (note->command == 0x0D)
                    {
                        if (note->param == 0)
                            note->command = 0x0D;
                        else
                            note->command = 0x0A;
                    }
                }
                else if (newModule->head.format == FORMAT_4CHN) // 4CHN != PT MOD
                {
                    // Remove FastTracker II 8xx/E8x panning commands if present
                    if (note->command == 0x08)
                    {
                        // 8xx
                        note->command = 0;
                        note->param   = 0;
                    }
                    else if ((note->command == 0x0E) && ((note->param >> 4) == 0x08))
                    {
                        // E8x
                        note->command = 0;
                        note->param   = 0;
                    }

                    // Remove F00, FastTracker II didn't use F00 as STOP in .MOD
                    if ((note->command == 0x0F) && (note->param == 0x00))
                    {
                        note->command = 0;
                        note->param   = 0;
                    }
                }

                note++;
            }
        }
    }

    numSamples = (newModule->head.format == FORMAT_STK) ? 15 : 31;
    for (i = 0; i < numSamples; ++i)
        newModule->samples[i].offset = 0x0001FFFE * i;

    // init 31*0x0001FFFE worth of sample data so that you can
    // easily load bigger samples if you're editing the song.
    // That's ~4MB. We have loads of RAM nowadays... No problem!
    newModule->sampleData = (int8_t *)(calloc(MOD_SAMPLES, 0x0001FFFE));
    if (newModule->sampleData == NULL)
    {
        mclose(&mod);

        free(modBuffer);

        for (i = 0; i < MAX_PATTERNS; ++i)
        {
            if (newModule->patterns[i] != NULL)
                free(newModule->patterns[i]);
        }

        free(newModule);

        displayErrorMsg("OUT OF MEMORY !!!");

        return (NULL);
    }

    // load sample data
    numSamples = (newModule->head.format == FORMAT_STK) ? 15 : 31;
    for (i = 0; i < numSamples; ++i)
    {
        s = &newModule->samples[i];

        if (mightBeSTK && (newModule->samples[i].loopLength > 2))
        {
            for (j = 0; j < (uint32_t)(newModule->samples[i].tmpLoopStart); ++j)
                mgetc(mod); // skip

            mread(&newModule->sampleData[s->offset], 1, newModule->samples[i].length - newModule->samples[i].loopStart, mod);
        }
        else
        {
            mread(&newModule->sampleData[s->offset], 1, newModule->samples[i].length, mod);
        }

        fillRedoBuffer(newModule, i);
    }

    mclose(&mod);

    // set up display pointers (very important)
    for (i = 0; i < MOD_SAMPLES; ++i)
    {
        newModule->samples[i].volumeDisp     = &newModule->samples[i].volume;
        newModule->samples[i].lengthDisp     = &newModule->samples[i].length;
        newModule->samples[i].loopStartDisp  = &newModule->samples[i].loopStart;
        newModule->samples[i].loopLengthDisp = &newModule->samples[i].loopLength;
    }

    editor.currentEditPatternDisp = &newModule->currPattern;
    editor.currentPosDisp         = &newModule->currOrder;
    editor.currentPatternDisp     = &newModule->head.order[0];
    editor.currentPosEdPattDisp   = &newModule->head.order[0];
    editor.currentLengthDisp      = &newModule->head.orderCount;

    // calculate MOD size
    calcRealModSize(newModule);

    editor.muted[0]             = false;
    editor.muted[1]             = false;
    editor.muted[2]             = false;
    editor.muted[3]             = false;
    editor.EditMoveAdd          = 1;
    editor.currentSample        = 0;
    editor.playTime             = 0;
    editor.modLoaded            = true;
    editor.BlockMarkFlag        = false;
    editor.keypadSampleOffset   = 0;
    editor.SampleZero           = false;
    editor.ui.aboutScreenShown  = false;
    editor.ui.editOpScreenShown = false;

    setLEDFilter(false);  // real PT doesn't do this there, but that's insane

    samplerClearCopyBuffer();
    setupSampler(newModule, REMOVE_SAMPLE_MARKING);

    editor.SamplePos = 0;

    memset(editor.ui.pattNames, 0, MAX_PATTERNS * 16);

    modSetPos(0, 0);

    updateWindowTitle(newModule, MOD_NOT_MODIFIED);

    return (newModule);
}

int8_t saveModule(MODULE *modEntry, int8_t checkIfFileExist, int8_t giveNewFreeFilename)
{
    char fileName[48];

    uint16_t i;
    uint16_t j;

    struct stat statBuffer;

    memset(fileName, 0, sizeof (fileName));

    if (editor.diskop.moddot)
    {
        // extension.filename

        if (*modEntry->head.moduleTitle == '\0')
        {
            strcat(fileName, "mod.untitled");
        }
        else
        {
            strcat(fileName, "mod.");

            for (i = 4; i < (20 + 4); ++i)
            {
                fileName[i] = modEntry->head.moduleTitle[i - 4];
                if (fileName[i] == '\0')
                    break;

                // A..Z -> a..z
                if ((fileName[i] >= 'A') && (fileName[i] <= 'Z'))
                    fileName[i] += 32;

                // convert illegal filename characters to spaces
                     if (fileName[i] ==  '<') fileName[i] = ' ';
                else if (fileName[i] ==  '>') fileName[i] = ' ';
                else if (fileName[i] ==  ':') fileName[i] = ' ';
                else if (fileName[i] ==  '"') fileName[i] = ' ';
                else if (fileName[i] ==  '/') fileName[i] = ' ';
                else if (fileName[i] == '\\') fileName[i] = ' ';
                else if (fileName[i] ==  '|') fileName[i] = ' ';
                else if (fileName[i] ==  '?') fileName[i] = ' ';
                else if (fileName[i] ==  '*') fileName[i] = ' ';
            }
        }
    }
    else
    {
        // filename.extension

        if (*modEntry->head.moduleTitle == '\0')
        {
            strcat(fileName, "untitled.mod");
        }
        else
        {
            for (i = 0; i < 20; ++i)
            {
                fileName[i] = modEntry->head.moduleTitle[i];
                if (fileName[i] == '\0')
                    break;

                // A..Z -> a..z
                if ((fileName[i] >= 'A') && (fileName[i] <= 'Z'))
                    fileName[i] += 32;

                // convert illegal filename characters to spaces
                     if (fileName[i] ==  '<') fileName[i] = ' ';
                else if (fileName[i] ==  '>') fileName[i] = ' ';
                else if (fileName[i] ==  ':') fileName[i] = ' ';
                else if (fileName[i] ==  '"') fileName[i] = ' ';
                else if (fileName[i] ==  '/') fileName[i] = ' ';
                else if (fileName[i] == '\\') fileName[i] = ' ';
                else if (fileName[i] ==  '|') fileName[i] = ' ';
                else if (fileName[i] ==  '?') fileName[i] = ' ';
                else if (fileName[i] ==  '*') fileName[i] = ' ';
            }

            strcat(fileName, ".mod");
        }
    }

    if (giveNewFreeFilename)
    {
        if (stat(fileName, &statBuffer) == 0)
        {
            for (j = 1; j <= 9999; ++j) // This number should satisfy all! ;)
            {
                memset(fileName, 0, sizeof (fileName));

                if (editor.diskop.moddot)
                {
                    // extension.filename

                    if (*modEntry->head.moduleTitle == '\0')
                    {
                        sprintf(fileName, "mod.untitled-%d", j);
                    }
                    else
                    {
                        strcat(fileName, "mod.");

                        for (i = 4; i < 20 + 4; ++i)
                        {
                            fileName[i] = modEntry->head.moduleTitle[i - 4];
                            if (fileName[i] == '\0')
                                break;

                            // A..Z -> a..z
                            if ((fileName[i] >= 'A') && (fileName[i] <= 'Z'))
                                fileName[i] += 32;

                            // convert illegal filename characters to spaces
                                 if (fileName[i] ==  '<') fileName[i] = ' ';
                            else if (fileName[i] ==  '>') fileName[i] = ' ';
                            else if (fileName[i] ==  ':') fileName[i] = ' ';
                            else if (fileName[i] ==  '"') fileName[i] = ' ';
                            else if (fileName[i] ==  '/') fileName[i] = ' ';
                            else if (fileName[i] == '\\') fileName[i] = ' ';
                            else if (fileName[i] ==  '|') fileName[i] = ' ';
                            else if (fileName[i] ==  '?') fileName[i] = ' ';
                            else if (fileName[i] ==  '*') fileName[i] = ' ';
                        }

                        sprintf(fileName, "%s-%d", fileName, j);
                    }
                }
                else
                {
                    // filename.extension

                    if (*modEntry->head.moduleTitle == '\0')
                    {
                        sprintf(fileName, "untitled-%d.mod", j);
                    }
                    else
                    {
                        for (i = 0; i < 20; ++i)
                        {
                            fileName[i] = modEntry->head.moduleTitle[i];
                            if (fileName[i] == '\0')
                                break;

                            // A..Z -> a..z
                            if ((fileName[i] >= 'A') && (fileName[i] <= 'Z'))
                                fileName[i] += 32;

                            // convert illegal filename characters to spaces
                                 if (fileName[i] ==  '<') fileName[i] = ' ';
                            else if (fileName[i] ==  '>') fileName[i] = ' ';
                            else if (fileName[i] ==  ':') fileName[i] = ' ';
                            else if (fileName[i] ==  '"') fileName[i] = ' ';
                            else if (fileName[i] ==  '/') fileName[i] = ' ';
                            else if (fileName[i] == '\\') fileName[i] = ' ';
                            else if (fileName[i] ==  '|') fileName[i] = ' ';
                            else if (fileName[i] ==  '?') fileName[i] = ' ';
                            else if (fileName[i] ==  '*') fileName[i] = ' ';
                        }

                        sprintf(fileName, "%s-%d", fileName, j);
                        strcat(fileName, ".mod");
                    }
                }

                if (stat(fileName, &statBuffer) != 0)
                    break;
            }
        }
    }

    if (checkIfFileExist)
    {
        if (stat(fileName, &statBuffer) == 0)
        {
            editor.ui.askScreenShown = true;
            editor.ui.askScreenType  = ASK_SAVEMOD_OVERWRITE;

            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
            setStatusMessage("OVERWRITE FILE ?", NO_CARRY);

            return (-1);
        }
    }

    if (editor.ui.askScreenShown)
    {
        editor.ui.answerNo       = false;
        editor.ui.answerYes      = false;
        editor.ui.askScreenShown = false;
    }

    return (modSave(modEntry, fileName));
}

static MEM *mopen(const uint8_t *src, uint32_t length)
{
    MEM *b;

    if ((src == NULL) || (length == 0)) return (NULL);

    b = (MEM *)(malloc(sizeof (MEM)));
    if (b == NULL) return (NULL);

    b->_base   = (uint8_t *)(src);
    b->_ptr    = (uint8_t *)(src);
    b->_cnt    = length;
    b->_bufsiz = length;
    b->_eof    = false;

    return (b);
}

static void mclose(MEM **buf)
{
    if (*buf != NULL)
    {
        free(*buf);
        *buf = NULL;
    }
}

static int32_t mgetc(MEM *buf)
{
    int32_t b;

    b = *buf->_ptr;

    buf->_cnt--;
    buf->_ptr++;

    if (buf->_cnt <= 0)
    {
        buf->_ptr = buf->_base + buf->_bufsiz;
        buf->_cnt = 0;
        buf->_eof = true;
    }

    return (int32_t)(b);
}

static size_t mread(void *buffer, size_t size, size_t count, MEM *buf)
{
    int32_t pcnt;

    size_t wrcnt;

    if (buf       == NULL) return (0);
    if (buf->_ptr == NULL) return (0);

    wrcnt = size * count;
    if ((size == 0) || buf->_eof) return (0);

    pcnt = ((uint32_t)(buf->_cnt) > wrcnt) ? wrcnt : buf->_cnt;
    memcpy(buffer, buf->_ptr, pcnt);

    buf->_cnt -= pcnt;
    buf->_ptr += pcnt;

    if (buf->_cnt <= 0)
    {
        buf->_ptr = buf->_base + buf->_bufsiz;
        buf->_cnt = 0;
        buf->_eof = true;
    }

    return (pcnt / size);
}

static void mseek(MEM *buf, int32_t offset, int32_t whence)
{
    if (buf == NULL) return;

    if (buf->_base)
    {
        switch (whence)
        {
            case SEEK_SET: buf->_ptr  = buf->_base + offset;                break;
            case SEEK_CUR: buf->_ptr += offset;                             break;
            case SEEK_END: buf->_ptr  = buf->_base + buf->_bufsiz + offset; break;
            default: break;
        }

        buf->_eof = false;
        if (buf->_ptr >= (buf->_base + buf->_bufsiz))
        {
            buf->_ptr = buf->_base + buf->_bufsiz;
            buf->_eof = true;
        }

        buf->_cnt = (buf->_base + buf->_bufsiz) - buf->_ptr;
    }
}

/*
** Code taken from Heikki Orsila's amigadepack.
** Modified by 8bitbubsy
**/

#define PP_READ_BITS(nbits, var) do {        \
  bit_cnt = (nbits);                         \
  while (bits_left < bit_cnt) {              \
    if (buf_src < src) return (false);       \
    bit_buffer |= (*--buf_src << bits_left); \
    bits_left += 8;                          \
  }                                          \
  (var) = 0;                                 \
  bits_left -= bit_cnt;                      \
  while (bit_cnt--) {                        \
    (var) = ((var) << 1) | (bit_buffer & 1); \
    bit_buffer >>= 1;                        \
  }                                          \
} while (0);

uint8_t ppdecrunch(uint8_t *src, uint8_t *dest, uint8_t *offset_lens, uint32_t src_len, uint32_t dest_len, uint8_t skip_bits)
{
    uint8_t *buf_src;
    uint8_t *dest_end;
    uint8_t *out;
    uint8_t bits_left;
    uint8_t bit_cnt;

    uint32_t x;
    uint32_t todo;
    uint32_t offbits;
    uint32_t offset;
    uint32_t written;
    uint32_t bit_buffer;

    if ((src == NULL) || (dest == NULL) || (offset_lens == NULL)) return (false);

    bits_left  = 0;
    bit_buffer = 0;
    written    = 0;
    buf_src    = src  + src_len;
    out        = dest + dest_len;
    dest_end   = out;

    PP_READ_BITS(skip_bits, x);
    while (written < dest_len)
    {
        PP_READ_BITS(1, x);
        if (x == 0)
        {
            todo = 1;

            do
            {
                PP_READ_BITS(2, x);
                todo += x;
            }
            while (x == 3);

            while (todo--)
            {
                PP_READ_BITS(8, x);

                if (out <= dest)
                    return (false);

                *--out = (uint8_t)(x);
                written++;
            }

            if (written == dest_len)
                break;
        }

        PP_READ_BITS(2, x);

        offbits = offset_lens[x];
        todo    = x + 2;

        if (x == 3)
        {
            PP_READ_BITS(1, x);
            if (x == 0) offbits = 7;

            PP_READ_BITS((uint8_t)(offbits), offset);
            do
            {
                PP_READ_BITS(3, x);
                todo += x;
            }
            while (x == 7);
        }
        else
        {
            PP_READ_BITS((uint8_t)(offbits), offset);
        }

        if ((out + offset) >= dest_end)
            return (false);

        while (todo--)
        {
            x = out[offset];

            if (out <= dest)
                return (false);

            *--out = (uint8_t)(x);
            written++;
        }
    }

    return (true);
}
