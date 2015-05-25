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
#include <math.h>
#include <ctype.h> // for toupper()/tolower()
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pt_header.h"
#include "pt_textout.h"
#include "pt_palette.h"
#include "pt_sampler.h"
#include "pt_audio.h"
#include "pt_sampleloader.h"
#include "pt_visuals.h"
#include "pt_helpers.h"

int8_t loadWAVSample(MODULE *modEntry, const char *filename, char *entryname, int8_t forceDownSampling);
int8_t loadIFFSample(MODULE *modEntry, const char *filename, char *entryname);
int8_t loadRAWSample(MODULE *modEntry, const char *filename, char *entryname);

void normalize16bitSigned(int16_t *sampleData, uint32_t sampleLength, int8_t stereo)
{
    int32_t sampleL; // -32768..32768 (thus 32-bit)
    int32_t sampleR; // -32768..32768 (thus 32-bit)
    int32_t sampleVolL;
    int32_t sampleVolR;
    uint32_t i;

    sampleVolL = 0;
    sampleVolR = 0;

    for (i = 0; i < sampleLength; ++i)
    {
        if (stereo)
        {
            sampleL = (int32_t)(sampleData[(i * 2)    ]);
            sampleR = (int32_t)(sampleData[(i * 2) + 1]);

            if (sampleR < 0)
                sampleR = 0 - sampleR; // abs()

            if (sampleVolR < sampleR)
                sampleVolR = sampleR;
        }
        else
        {
            sampleL = (int32_t)(sampleData[i]);
        }

        if (sampleL < 0)
            sampleL = 0 - sampleL; // abs()

        if (sampleVolL < sampleL)
            sampleVolL = sampleL;
    }

    // prevent division by zero!
    if (sampleVolL <= 0) sampleVolL = 1;
    if (sampleVolR <= 0) sampleVolR = 1;

    sampleVolL = (32767 * 32768) / sampleVolL;
    sampleVolR = (32767 * 32768) / sampleVolR;

    for (i = 0; i < sampleLength; ++i)
    {
        if (stereo)
        {
            sampleData[(i * 2)    ] = (int16_t)((sampleData[(i * 2)    ] * sampleVolL) / 32768);
            sampleData[(i * 2) + 1] = (int16_t)((sampleData[(i * 2) + 1] * sampleVolR) / 32768);
        }
        else
        {
            sampleData[i] = (int16_t)((sampleData[i] * sampleVolL) / 32768);
        }
    }
}

void normalize8bitUnsigned(uint8_t *sampleData, uint32_t sampleLength, int8_t stereo)
{
    int16_t sampleL; // -128..128 (thus 16-bit)
    int16_t sampleR; // -128..128 (thus 16-bit)

    int32_t sampleVolL;
    int32_t sampleVolR;
    uint32_t i;

    sampleVolL = 0;
    sampleVolR = 0;

    for (i = 0; i < sampleLength; ++i)
    {
        if (stereo)
        {
            sampleL = (int16_t)(sampleData[(i * 2)    ] - 128);
            sampleR = (int16_t)(sampleData[(i * 2) + 1] - 128);

            if (sampleR < 0)
                sampleR = 0 - sampleR; // abs()

            if (sampleVolR < sampleR)
                sampleVolR = sampleR;
        }
        else
        {
            sampleL = (int16_t)(sampleData[i] - 128);
        }

        if (sampleL < 0)
            sampleL = 0 - sampleL; // abs()

        if (sampleVolL < sampleL)
            sampleVolL = sampleL;
    }

    // prevent division by zero!
    if (sampleVolL <= 0) sampleVolL = 1;
    if (sampleVolR <= 0) sampleVolR = 1;

    sampleVolL = (127 * 32768) / sampleVolL;
    sampleVolR = (127 * 32768) / sampleVolR;

    for (i = 0; i < sampleLength; ++i)
    {
        if (stereo)
        {
            sampleL = (int16_t)(sampleData[(i * 2)    ] - 128);
            sampleR = (int16_t)(sampleData[(i * 2) + 1] - 128);

            sampleData[(i * 2)    ] = (uint8_t)(((sampleL * sampleVolL) / 32768) + 128);
            sampleData[(i * 2) + 1] = (uint8_t)(((sampleR * sampleVolR) / 32768) + 128);
        }
        else
        {
            sampleL       = (int16_t)(sampleData[i] - 128);
            sampleData[i] = (uint8_t)(((sampleL * sampleVolL) / 32768) + 128);
        }
    }
}

void extLoadWAVSampleCallback(MODULE *modEntry, int8_t downsample)
{
    loadWAVSample(modEntry, editor.filenameTmp, editor.entrynameTmp, downsample);
}

int8_t loadWAVSample(MODULE *modEntry, const char *filename, char *entryname, int8_t forceDownSampling)
{
    // Stereo is downmixed to mono (combine channels)
    // 16-bit is downmixed to 8-bit
    // 2x downsampling (if wanted by the user)
    //
    // Oops look out for spaghetti code

    int8_t smpL8;
    int8_t smpR8;
    int8_t skippingChunks;
    uint8_t *audioData8;
    uint8_t WAVVolumeFound;
    uint8_t WAVSampleNameFound;

    int16_t *audioData16;
    int16_t tempVol;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint16_t bitsPerSample;

    uint32_t i;
    uint32_t nameLen;
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t sampleLength;
    uint32_t fileLength;
    uint32_t loopFlags;
    uint32_t loopStart;
    uint32_t loopEnd;

    FILE *f;

    WAVVolumeFound     = false;
    WAVSampleNameFound = false;

    if (forceDownSampling == -1)
    {
        // these two *must* be fully wiped, for outputting reasons
        memset(editor.filenameTmp,  0, PATH_MAX_LEN);
        memset(editor.entrynameTmp, 0, PATH_MAX_LEN);

        strcpy(editor.filenameTmp,  filename);
        strcpy(editor.entrynameTmp, entryname);
    }

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    fseek(f, 0, SEEK_END);
    fileLength = ftell(f);
    fseek(f, 0, SEEK_SET);

    fread(&chunkID, 4, 1, f); if (bigEndian) chunkID = SWAP32(chunkID);
    if (chunkID != 0x46464952)
    {
        displayErrorMsg("NOT A WAV !");
        fclose(f);

        return (false);
    }

    fseek(f, 8, SEEK_CUR); // unneeded stuff

    fread(&chunkID,     4, 1, f); if (bigEndian) chunkID     = SWAP32(chunkID);
    fread(&chunkSize,   4, 1, f); if (bigEndian) chunkSize   = SWAP32(chunkSize);
    fread(&audioFormat, 2, 1, f); if (bigEndian) audioFormat = SWAP16(audioFormat);
    fread(&numChannels, 2, 1, f); if (bigEndian) numChannels = SWAP16(numChannels);

    fseek(f, 10, SEEK_CUR); // unneeded stuff

    fread(&bitsPerSample, 2, 1, f); if (bigEndian) bitsPerSample = SWAP16(bitsPerSample);

    if (chunkID != 0x20746D66)
    {
        displayErrorMsg("NOT A WAV !");
        fclose(f);

        return (false);
    }

    if ((chunkSize > 16) && (ftell(f) + chunkSize < fileLength))
        fseek(f, chunkSize - 16, SEEK_CUR);

    skippingChunks = true;
    while (skippingChunks && !feof(f))
    {
        fread(&chunkID,   4, 1, f); if (bigEndian) chunkID   = SWAP32(chunkID);
        fread(&chunkSize, 4, 1, f); if (bigEndian) chunkSize = SWAP32(chunkSize);

        switch (chunkID)
        {
            case 0x61746164: // "data"
                skippingChunks = false;
            break;

            default:
            {
                // uninteresting chunk, skip its content

                if ((ftell(f) + chunkSize) >= fileLength)
                {
                    fclose(f);

                    displayErrorMsg("WAV IS CORRUPT !");
                    return (false);
                }

                if (chunkSize > 0)
                    fseek(f, chunkSize, SEEK_CUR);
            }
            break;
        }
    }

    sampleLength = chunkSize;

    if ((chunkID != 0x61746164) || (sampleLength >= fileLength))
    {
        displayErrorMsg("NOT A WAV !");
        fclose(f);

        return (false);
    }

    if ((audioFormat != 1) || (numChannels == 0) || (numChannels > 2))
    {
        displayErrorMsg("WAV UNSUPPORTED !");
        fclose(f);

        return (false);
    }

    if ((bitsPerSample != 8) && (bitsPerSample != 16))
    {
        displayErrorMsg("WAV UNSUPPORTED !");
        fclose(f);

        return (false);
    }

    if (forceDownSampling == -1)
    {
        editor.ui.askScreenShown = true;
        editor.ui.askScreenType  = ASK_DOWNSAMPLING;

        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
        setStatusMessage("2X DOWNSAMPLING ?", NO_CARRY);

        fclose(f);

        return (true);
    }

    if (bitsPerSample == 8)
    {
        audioData8 = (uint8_t *)(malloc(sampleLength));
        if (audioData8 == NULL)
        {
            fclose(f);
            displayErrorMsg("OUT OF MEMORY !!!");

            return (false);
        }

        fread(audioData8, 1, sampleLength, f);

        mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

        if (forceDownSampling)
        {
            sampleLength /= 2;

            if (numChannels == 2)
            {
                sampleLength /= 2;

                normalize8bitUnsigned(audioData8, MIN(sampleLength * 2, 0x0001FFFE * 2), NORMALIZE_STEREO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                    {
                        smpL8 = audioData8[(i * 4)    ] - 128;
                        smpR8 = audioData8[(i * 4) + 1] - 128;

                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (smpL8 + smpR8) / 2;
                    }
                    else
                    {
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                    }
                }
            }
            else
            {
                normalize8bitUnsigned(audioData8, MIN(sampleLength * 2, 0x0001FFFE * 2), NORMALIZE_MONO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = audioData8[i * 2] - 128;
                    else
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                }
            }
        }
        else
        {
            if (numChannels == 2)
            {
                sampleLength /= 2;

                normalize8bitUnsigned(audioData8, MIN(sampleLength, 0x0001FFFE), NORMALIZE_STEREO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                    {
                        smpL8 = audioData8[(i * 2)    ] - 128;
                        smpR8 = audioData8[(i * 2) + 1] - 128;

                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (smpL8 + smpR8) / 2;
                    }
                    else
                    {
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                    }
                }
            }
            else
            {
                normalize8bitUnsigned(audioData8, MIN(sampleLength, 0x0001FFFE), NORMALIZE_MONO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = audioData8[i] - 128;
                    else
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                }
            }
        }

        if (sampleLength > 0x0001FFFE)
            sampleLength = 0x0001FFFE;

        free(audioData8);
    }
    else
    {
        sampleLength /= 2;

        audioData16 = (int16_t *)(malloc(sampleLength * sizeof (int16_t)));
        if (audioData16 == NULL)
        {
            fclose(f);
            displayErrorMsg("OUT OF MEMORY !!!");

            return (false);
        }

        fread(audioData16, 2, sampleLength, f);
        if (bigEndian)
        {
            for (i = 0; i < sampleLength; ++i)
                audioData16[i] = SWAP16(audioData16[i]);
        }

        mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

        if (forceDownSampling)
        {
            sampleLength /= 2;

            if (numChannels == 2)
            {
                sampleLength /= 2;

                normalize16bitSigned(audioData16, MIN(sampleLength * 2, 0x0001FFFE * 2), NORMALIZE_STEREO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                    {
                        smpL8 = (audioData16[(i * 4)    ] / 256) & 0xFF;
                        smpR8 = (audioData16[(i * 4) + 1] / 256) & 0xFF;

                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (smpL8 + smpR8) / 2;
                    }
                    else
                    {
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                    }
                }
            }
            else
            {
                normalize16bitSigned(audioData16, MIN(sampleLength * 2, 0x0001FFFE * 2), NORMALIZE_MONO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (int8_t)(audioData16[i * 2] / 256);
                    else
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                }
            }
        }
        else
        {
            if (numChannels == 2)
            {
                sampleLength /= 2;

                normalize16bitSigned(audioData16, MIN(sampleLength, 0x0001FFFE), NORMALIZE_STEREO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                    {
                        smpL8 = (audioData16[(i * 2)    ] / 256) & 0xFF;
                        smpR8 = (audioData16[(i * 2) + 1] / 256) & 0xFF;

                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (smpL8 + smpR8) / 2;
                    }
                    else
                    {
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                    }
                }
            }
            else
            {
                normalize16bitSigned(audioData16, MIN(sampleLength, 0x0001FFFE), NORMALIZE_MONO);

                for (i = 0; i < 0x0001FFFE; ++i)
                {
                    if (i <= sampleLength)
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = (int8_t)(audioData16[i] / 256);
                    else
                        modEntry->sampleData[(editor.currentSample * 0x0001FFFE) + i] = 0;
                }
            }
        }

        if (sampleLength > 0x0001FFFE)
            sampleLength = 0x0001FFFE;

        free(audioData16);
    }

    modEntry->samples[editor.currentSample].loopStart  = 0;
    modEntry->samples[editor.currentSample].loopLength = 2;

    // This is a silly way of checking chunk IDs.
    // I assume the other chunks won't contain another chunk's ID,
    // so it should be safe in *most* situations to just look for
    // a new chunk ID from the middle of a previous one.

    // let's see if we can find an "smpl" chunk
    while (!feof(f))
    {
        fread(&chunkID, 4, 1, f); if (bigEndian) chunkID = SWAP32(chunkID);
        if (chunkID == 0x6C706D73) // "smpl"
        {
            fseek(f, 32, SEEK_CUR);
            fread(&loopFlags, 4, 1, f); if (bigEndian) loopFlags = SWAP32(loopFlags);
            fseek(f, 12, SEEK_CUR);
            fread(&loopStart, 4, 1, f); if (bigEndian) loopStart = SWAP32(loopStart);
            fread(&loopEnd,   4, 1, f); if (bigEndian) loopEnd   = SWAP32(loopEnd);

            if (loopFlags)
            {
                if (forceDownSampling)
                {
                    // we already downsampled 2x, so we're half the original length
                    loopStart /= 2;
                    loopEnd   /= 2;
                }

                // even'ify loop points
                loopStart &= 0xFFFFFFFE;
                loopEnd   &= 0xFFFFFFFE;

                if (loopEnd <= sampleLength)
                {
                    modEntry->samples[editor.currentSample].loopStart  = loopStart;
                    modEntry->samples[editor.currentSample].loopLength = loopEnd - loopStart;
                }
            }

            break;
        }

        if (feof(f))
            break;

        fseek(f, -3, SEEK_CUR);
    }

    // let's see if we can find an "xtra" chunk
    while (!feof(f))
    {
        fread(&chunkID, 4, 1, f); if (bigEndian) chunkID = SWAP32(chunkID);
        if (chunkID == 0x61727478) // "xtra"
        {
            fread(&chunkSize, 4, 1, f); if (bigEndian) chunkSize = SWAP32(chunkSize);
            if (chunkSize >= 16)
            {
                WAVVolumeFound = true;

                fseek(f, 6, SEEK_CUR);
                fread(&tempVol, 2, 1, f);

                tempVol = CLAMP(tempVol, 0, 256);
                modEntry->samples[editor.currentSample].volume = (int8_t)(floorf(((float)(tempVol) * (64.0f / 256.0f)) + 0.5f));
            }

            break;
        }

        if (feof(f))
            break;

        fseek(f, -3, SEEK_CUR);
    }

    // let's see if we can find an "INAM" (sample name) chunk
    while (!feof(f))
    {
        fread(&chunkID, 4, 1, f); if (bigEndian) chunkID = SWAP32(chunkID);
        if (chunkID == 0x4D414E49) // "INAM"
        {
            WAVSampleNameFound = true;

            fread(&chunkSize, 4, 1, f); if (bigEndian) chunkSize = SWAP32(chunkSize);
            if (chunkSize > 0)
            {
                for (i = 0; i < 21; ++i)
                {
                    if (i < chunkSize)
                        modEntry->samples[editor.currentSample].text[i] = (char)(tolower(fgetc(f)));
                    else
                        modEntry->samples[editor.currentSample].text[i] = '\0';
                }

                modEntry->samples[editor.currentSample].text[21] = '\0';
            }

            break;
        }

        if (feof(f))
            break;

        fseek(f, -3, SEEK_CUR);
    }

    fclose(f);

    // set sample attributes
    if (!WAVVolumeFound)
        modEntry->samples[editor.currentSample].volume = 64;

    modEntry->samples[editor.currentSample].fineTune = 0;
    modEntry->samples[editor.currentSample].length   = sampleLength & 0xFFFFFFFE;

    // copy over sample name
    if (!WAVSampleNameFound)
    {
        nameLen = strlen(entryname);
        for (i = 0; i < 21; ++i)
            modEntry->samples[editor.currentSample].text[i] = (i < nameLen) ? (char)(tolower(entryname[i])) : '\0';

        modEntry->samples[editor.currentSample].text[21] = '\0';
    }

    // remove .wav from end of sample name (if present)
    nameLen = strlen(modEntry->samples[editor.currentSample].text);
    if (!strncmp(&modEntry->samples[editor.currentSample].text[nameLen - 4], ".wav", 4))
            memset(&modEntry->samples[editor.currentSample].text[nameLen - 4], 0, 4);

    UpdateVoiceParameters(modEntry);

    removeTempLoopPoints();
    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    fillRedoBuffer(modEntry, editor.currentSample);

    editor.SampleZero = false;
    editor.SamplePos  = 0;

    calcRealModSize(modEntry);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);

    return (true);
}

int8_t loadIFFSample(MODULE *modEntry, const char *filename, char *entryname)
{
    uint8_t i;
    uint8_t skippingBlocks;

    uint32_t nameLen;
    uint32_t sampleLength;
    uint32_t sampleLoopStart;
    uint32_t sampleLoopLength;
    uint32_t blockName;
    uint32_t blockSize;
    uint32_t fileLength;

    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    fseek(f, 0, SEEK_END);
    fileLength = ftell(f);
    fseek(f, 0, SEEK_SET);

    sampleLength   = 0;
    skippingBlocks = true;

    fseek(f, 20, SEEK_SET);

    fread(&sampleLoopStart,  4, 1, f); if (!bigEndian) sampleLoopStart  = SWAP32(sampleLoopStart);
    fread(&sampleLoopLength, 4, 1, f); if (!bigEndian) sampleLoopLength = SWAP32(sampleLoopLength);

    sampleLoopStart  &= 0xFFFFFFFE;
    sampleLoopLength &= 0xFFFFFFFE;

    fseek(f, 12, SEEK_SET);
    while (skippingBlocks && !feof(f))
    {
        // blockName is little-endian, blockSize is big-endian
        fread(&blockName, 4, 1, f); if ( bigEndian) blockName = SWAP32(blockName);
        fread(&blockSize, 4, 1, f); if (!bigEndian) blockSize = SWAP32(blockSize);

        switch (blockName)
        {
            case 0x59444F42: // BODY
            {
                skippingBlocks = false;
                sampleLength   = blockSize & 0xFFFFFFFE;
            }
            break;

            default:
            {
                // uninteresting block, skip its content

                if ((ftell(f) + blockSize) >= fileLength)
                {
                    fclose(f);

                    displayErrorMsg("IFF IS CORRUPT !");
                    return (false);
                }

                if (blockSize > 0)
                    fseek(f, blockSize, SEEK_CUR);
            }
            break;
        }
    }

    if ((sampleLength == 0) || skippingBlocks || feof(f))
    {
        fclose(f);

        displayErrorMsg("NOT A VALID IFF !");
        return (false);
    }

    if (sampleLength > 0x0001FFFE)
        sampleLength = 0x0001FFFE;

    if (sampleLoopLength < 2)
    {
        sampleLoopStart  = 0;
        sampleLoopLength = 2;
    }

    if ((sampleLoopStart >= 0x0001FFFE) || (sampleLoopLength > 0x0001FFFE))
    {
        sampleLoopStart  = 0;
        sampleLoopLength = 2;
    }

    if ((sampleLoopStart + sampleLoopLength) > sampleLength)
    {
        sampleLoopStart  = 0;
        sampleLoopLength = 2;
    }

    if (sampleLoopStart > (sampleLength - 2))
    {
        sampleLoopStart  = 0;
        sampleLoopLength = 2;
    }

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    memset(modEntry->sampleData + modEntry->samples[editor.currentSample].offset, 0, 0x0001FFFE);
    fread(modEntry->sampleData + modEntry->samples[editor.currentSample].offset, 1, sampleLength, f);
    fclose(f);

    // set sample attributes
    memset(modEntry->samples[editor.currentSample].text, 0, sizeof (modEntry->samples[editor.currentSample].text));
    modEntry->samples[editor.currentSample].volume     = 64;
    modEntry->samples[editor.currentSample].fineTune   = 0;
    modEntry->samples[editor.currentSample].length     = sampleLength;
    modEntry->samples[editor.currentSample].loopStart  = sampleLoopStart;
    modEntry->samples[editor.currentSample].loopLength = sampleLoopLength;

    // copy over sample name
    nameLen = strlen(entryname);
    for (i = 0; i < 21; ++i)
        modEntry->samples[editor.currentSample].text[i] = (i < nameLen) ? (char)(tolower(entryname[i])) : '\0';

    modEntry->samples[editor.currentSample].text[21] = '\0';

    // remove .iff from end of sample name (if present)
    nameLen = strlen(modEntry->samples[editor.currentSample].text);
    if (!strncmp(&modEntry->samples[editor.currentSample].text[nameLen - 4], ".iff", 4))
          memset(&modEntry->samples[editor.currentSample].text[nameLen - 4], 0, 4);

    UpdateVoiceParameters(modEntry);

    removeTempLoopPoints();
    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    fillRedoBuffer(modEntry, editor.currentSample);

    editor.SampleZero = false;
    editor.SamplePos  = 0;

    calcRealModSize(modEntry);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);

    return (false);
}

int8_t loadRAWSample(MODULE *modEntry, const char *filename, char *entryname)
{
    uint8_t i;

    uint32_t nameLen;
    uint32_t fileSize;

    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    if (fileSize > 0x0001FFFE)
        fileSize = 0x0001FFFE;

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    memset(modEntry->sampleData + modEntry->samples[editor.currentSample].offset, 0, 0x0001FFFE);
    fread(modEntry->sampleData  + modEntry->samples[editor.currentSample].offset, 1, fileSize, f);
    fclose(f);

    // set sample attributes
    memset(modEntry->samples[editor.currentSample].text, 0, sizeof (modEntry->samples[editor.currentSample].text));
    modEntry->samples[editor.currentSample].volume     = 64;
    modEntry->samples[editor.currentSample].fineTune   = 0;
    modEntry->samples[editor.currentSample].length     = fileSize;
    modEntry->samples[editor.currentSample].loopStart  = 0;
    modEntry->samples[editor.currentSample].loopLength = 2;

    // copy over sample name
    nameLen = strlen(entryname);
    for (i = 0; i < 21; ++i)
        modEntry->samples[editor.currentSample].text[i] = (i < nameLen) ? (char)(tolower(entryname[i])) : '\0';

    modEntry->samples[editor.currentSample].text[21] = '\0';

    UpdateVoiceParameters(modEntry);

    removeTempLoopPoints();
    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    fillRedoBuffer(modEntry, editor.currentSample);

    editor.SampleZero = false;
    editor.SamplePos  = 0;

    calcRealModSize(modEntry);
    updateWindowTitle(modEntry, MOD_IS_MODIFIED);

    return (true);
}

int8_t loadSample(MODULE *modEntry, const char *filename, char *entryname)
{
    uint32_t fileSize;
    uint32_t ID;

    FILE *f;

    f = fopen(filename, "rb");
    if (f == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    fseek(f, 0, SEEK_END);
    fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    // first, check heades before we eventually load as RAW
    if (fileSize > 16)
    {
        fread(&ID, 4, 1, f); if (bigEndian) ID = SWAP32(ID);

        // check if it's actually a WAV sample
        if (ID == 0x46464952) // "RIFF"
        {
            fseek(f, 4, SEEK_CUR);
            fread(&ID, 4, 1, f); if (bigEndian) ID = SWAP32(ID);

            if (ID == 0x45564157) // "WAVE"
            {
                fread(&ID, 4, 1, f); if (bigEndian) ID = SWAP32(ID);

                if (ID == 0x20746D66) // "fmt "
                {
                    fclose(f);
                    return (loadWAVSample(modEntry, filename, entryname, -1));
                }
            }
        }

        // check if it's an Amiga IFF sample
        else if (ID == 0x4D524F46) // "FORM"
        {
            fseek(f, 4, SEEK_CUR);
            fread(&ID, 4, 1, f); if (bigEndian) ID = SWAP32(ID);

            if (ID == 0x58565338) // "8SVX"
            {
                fread(&ID, 4, 1, f); if (bigEndian) ID = SWAP32(ID);

                if (ID == 0x52444856) // "VHDR"
                {
                    fclose(f);
                    return (loadIFFSample(modEntry, filename, entryname));
                }
            }
        }
    }

    // nope, continue loading as RAW
    fclose(f);

    return (loadRAWSample(modEntry, filename, entryname));
}

int8_t saveSample(MODULE *modEntry, int8_t checkIfFileExist, int8_t giveNewFreeFilename)
{
    char fileName[48];
    uint8_t smp;

    uint16_t j;

    uint32_t i;
    uint32_t sampleSize;
    uint32_t iffSize;
    uint32_t iffSampleSize;
    uint32_t loopStart;
    uint32_t loopLen;

    FILE *f;

    struct stat statBuffer;

    WAV_HEADER wave;

    if (modEntry->samples[editor.currentSample].length == 0)
    {
        displayErrorMsg("SAMPLE IS EMPTY");
        return (false);
    }

    memset(fileName, 0, sizeof (fileName));

    if (*modEntry->samples[editor.currentSample].text == '\0')
    {
        strcpy(fileName, "untitled");
    }
    else
    {
        for (i = 0; i < 22; ++i)
        {
            fileName[i] = modEntry->samples[editor.currentSample].text[i];
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

    // remove .wav/.iff from end of sample name (if present)
    if (!strncmp(fileName + (strlen(fileName) - 4), ".wav", 4) || !strncmp(fileName + (strlen(fileName) - 4), ".iff", 4))
        fileName[strlen(fileName) - 4] = '\0';

    switch (editor.diskop.smpSaveType)
    {
        case DISKOP_SMP_WAV: strcat(fileName, ".wav"); break;
        case DISKOP_SMP_IFF: strcat(fileName, ".iff"); break;
        case DISKOP_SMP_RAW:                           break;

        default: return (false); // make compiler happy
    }

    if (giveNewFreeFilename)
    {
        if (stat(fileName, &statBuffer) == 0)
        {
            for (j = 1; j <= 9999; ++j) // This number should satisfy all! ;)
            {
                memset(fileName, 0, sizeof (fileName));

                if (*modEntry->samples[editor.currentSample].text == '\0')
                {
                    sprintf(fileName, "untitled-%d", j);
                }
                else
                {
                    for (i = 0; i < 22; ++i)
                    {
                        fileName[i] = modEntry->samples[editor.currentSample].text[i];
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

                    // remove .wav/.iff from end of sample name (if present)
                    if (!strncmp(fileName + (strlen(fileName) - 4), ".wav", 4) || !strncmp(fileName + (strlen(fileName) - 4), ".iff", 4))
                        fileName[strlen(fileName) - 4] = '\0';

                    sprintf(fileName, "%s-%d", fileName, j);
                }

                switch (editor.diskop.smpSaveType)
                {
                    case DISKOP_SMP_WAV: strcat(fileName, ".wav"); break;
                    case DISKOP_SMP_IFF: strcat(fileName, ".iff"); break;
                    case DISKOP_SMP_RAW:                           break;

                    default: return (false);  // make compiler happy
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
            editor.ui.askScreenType  = ASK_SAVESMP_OVERWRITE;

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

    f = fopen(fileName, "wb");
    if (f == NULL)
    {
        displayErrorMsg("FILE I/O ERROR !");
        return (false);
    }

    sampleSize = modEntry->samples[editor.currentSample].length;

    switch (editor.diskop.smpSaveType)
    {
        case DISKOP_SMP_WAV:
        {
            wave.Format        = bigEndian ? SWAP32(0x45564157) : 0x45564157; // "WAVE"
            wave.ChunkID       = bigEndian ? SWAP32(0x46464952) : 0x46464952; // "RIFF"
            wave.Subchunk1ID   = bigEndian ? SWAP32(0x20746D66) : 0x20746D66; // "fmt "
            wave.Subchunk2ID   = bigEndian ? SWAP32(0x61746164) : 0x61746164; // "data"
            wave.Subchunk1Size = bigEndian ? SWAP32(16) : 16;
            wave.Subchunk2Size = bigEndian ? SWAP32(sampleSize) : sampleSize;
            wave.ChunkSize     = bigEndian ? SWAP32(wave.Subchunk2Size + 36) : (wave.Subchunk2Size + 36);
            wave.AudioFormat   = bigEndian ? SWAP16(1) : 1;
            wave.NumChannels   = bigEndian ? SWAP16(1) : 1;
            wave.BitsPerSample = bigEndian ? SWAP16(8) : 8;
            wave.SampleRate    = bigEndian ? SWAP32(16726) : 16726;
            wave.ByteRate      = bigEndian ? SWAP32(wave.SampleRate * wave.NumChannels * wave.BitsPerSample / 8)
                                    : (wave.SampleRate * wave.NumChannels * wave.BitsPerSample / 8);
            wave.BlockAlign    = bigEndian ? SWAP16(wave.NumChannels * wave.BitsPerSample / 8)
                                    : (wave.NumChannels * wave.BitsPerSample / 8);

            fwrite(&wave, sizeof (WAV_HEADER), 1, f);

            for (i = 0; i < sampleSize; ++i)
            {
                smp = (uint8_t)(modEntry->sampleData[modEntry->samples[editor.currentSample].offset + i]) + 128;
                fputc(smp, f);
            }

            // TODO on a boring day: Add special chunks here for loop points, volume and sample name.
        }
        break;

        case DISKOP_SMP_IFF:
        {
            // dwords are big-endian in IFF
            loopStart     = modEntry->samples[editor.currentSample].loopStart  & 0xFFFFFFFE;
            loopLen       = modEntry->samples[editor.currentSample].loopLength & 0xFFFFFFFE;

            if (!bigEndian) loopStart = SWAP32(loopStart);
            if (!bigEndian) loopLen   = SWAP32(loopLen);

            iffSize       = bigEndian ? (sampleSize + 96) : SWAP32(sampleSize + 96);
            iffSampleSize = bigEndian ? (sampleSize     ) : SWAP32(sampleSize     );

            fputc(0x46, f);fputc(0x4F, f);fputc(0x52, f);fputc(0x4D, f);    // "FORM"

            fwrite(&iffSize, 4, 1, f);

            fputc(0x38, f);fputc(0x53, f);fputc(0x56, f);fputc(0x58, f);    // "8SVX"
            fputc(0x56, f);fputc(0x48, f);fputc(0x44, f);fputc(0x52, f);    // "VHDR"
            fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);fputc(0x14, f);    // 0x00000014

            if (modEntry->samples[editor.currentSample].loopLength > 2)
            {
                fwrite(&loopStart, 4, 1, f);
                fwrite(&loopLen,   4, 1, f);
            }
            else
            {
                fwrite(&iffSampleSize, 4, 1, f);
                fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);// 0x00000000
            }

            fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);fputc(0x20, f);    // 0x00000020
            fputc(0x41, f);fputc(0x56, f);fputc(0x01, f);fputc(0x00, f);    // 0x41560100
            fputc(0x00, f);fputc(0x01, f);fputc(0x00, f);fputc(0x00, f);    // 0x00010000
            fputc(0x4E, f);fputc(0x41, f);fputc(0x4D, f);fputc(0x45, f);    // "NAME"
            fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);fputc(0x18, f);    // 0x00000018

            fwrite(modEntry->samples[editor.currentSample].text, 1, 21, f);

            fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);                   // pad to 24 bytes
            fputc(0x41, f);fputc(0x4E, f);fputc(0x4E, f);fputc(0x4F, f);    // "ANNO"
            fputc(0x00, f);fputc(0x00, f);fputc(0x00, f);fputc(0x11, f);    // 0x00000011

            fprintf(f, "ProTracker clone");fputc(0x00, f);

            fputc(0x42, f);fputc(0x4F, f);fputc(0x44, f);fputc(0x59, f);    // "BODY"

            fwrite(&iffSampleSize, 4, 1, f);

            fwrite(modEntry->sampleData + modEntry->samples[editor.currentSample].offset, 1, sampleSize, f);
        }
        break;

        case DISKOP_SMP_RAW:
        {
            fwrite(modEntry->sampleData + modEntry->samples[editor.currentSample].offset, 1, sampleSize, f);
        }
        break;

        default: return (false); break;  // make compiler happy
    }

    fclose(f);

    editor.diskop.cached = false;

    displayMsg("SAMPLE SAVED !");
    setMsgPointer();

    return (true);
}
