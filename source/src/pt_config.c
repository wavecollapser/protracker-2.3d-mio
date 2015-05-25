/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#define _GNU_SOURCE
// Dl_info and dladdr for GNU/Linux needs this define before header including

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#endif
#include "pt_helpers.h"
#include "pt_header.h"
#include "pt_config.h"
#include "pt_tables.h"
#include "pt_audio.h"
#include "pt_diskop.h"

int8_t loadConfig(PTCONFIG *config)
{
    char cfgString[18];
    char *configBuffer;
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t tmp8;

    uint16_t tmp16;

#ifndef WIN32
    Dl_info info;
    uint32_t pathLen;
#endif

    int32_t lineLen;
    uint32_t configFileSize;
    uint32_t i;

    FILE *configFile;

    // set standard config values first
    memset(config->defaultDiskOpDir, 0, PATH_MAX_LEN + 1);

    config->a4000LowPassFilter = false;
    config->soundFrequency     = 44100;
    config->stereoSeparation   = 25;
    config->scale3x            = false;
    config->moddot             = false;
    config->blankzero          = false;
    config->accidental         = 0; // sharp
    config->quantizeValue      = 1;
    config->transdel           = false;

#ifndef WIN32
    // massive hack to set dir to binary dir (thank you SDL, not!)

    // first check for $HOME/.protracker
    sprintf(editor.tempPath, "%s/.protracker", getenv("HOME"));

    configFile = fopen(editor.tempPath, "r");
    if (configFile == NULL)
    {
        // no, try to fetch binary_dir/protracker.ini

        dladdr((void *)(&loadConfig), &info);
        memset(editor.tempPath, 0, PATH_MAX_LEN);
        realpath(info.dli_fname, editor.tempPath);

        pathLen = strlen(editor.tempPath);
        for (i = pathLen; i != 0; --i)
        {
           if (editor.tempPath[i] == '/') // /app_name
           {
               editor.tempPath[i] = '\0';
               break;
           }
        }
 
        chdir(editor.tempPath);
#ifdef __APPLE__
        chdir("../../../"); // package.app/Contents/MacOS
#endif
        configFile = fopen("protracker.ini", "r");
        if (configFile == NULL)
        {
            fprintf(stderr, "WARNING: Could not open $HOME/.protracker or protracker.ini, using default settings\n");
            return (true);
        }
    }
#else
    configFile = fopen("protracker.ini", "r");
    if (configFile == NULL)
    {
        fprintf(stderr, "WARNING: Could not open protracker.ini, using default settings\n");
        return (true);
    }
#endif

    fseek(configFile, 0, SEEK_END);
    configFileSize = ftell(configFile);
    rewind(configFile);

    configBuffer = (char *)(malloc(configFileSize));
    if (configBuffer == NULL)
    {
        fclose(configFile);
        fprintf(stderr, "ERROR: Out of memory!\n");

        return (false);
    }

    fread(configBuffer, 1, configFileSize, configFile);
    fclose(configFile);

    configBuffer = strtok(configBuffer, "\n");
    while (configBuffer != NULL)
    {
        lineLen = strlen(configBuffer);

        // remove CR in CRLF linefeed (if present)
        if (configBuffer[lineLen - 1] == '\r')
        {
            configBuffer[lineLen - 1] = '\0';
            lineLen--;
        }

        // COMMENT OR CATEGORY
        if ((*configBuffer == ';') || (*configBuffer == '['))
        {
            configBuffer = strtok(NULL, "\n");
            continue;
        }

        // BLANKZERO
        else if (strncmp(configBuffer, "BLANKZERO=", 10) == 0)
        {
                 if (strncmp(&configBuffer[10], "TRUE",  4) == 0) config->blankzero = true;
            else if (strncmp(&configBuffer[10], "FALSE", 5) == 0) config->blankzero = false;
        }

        // ACCIDENTAL
        else if (strncmp(configBuffer, "ACCIDENTAL=", 11) == 0)
        {
                 if (strncmp(&configBuffer[11], "SHARP",  4) == 0) config->accidental = 0;
            else if (strncmp(&configBuffer[11], "FLAT",   5) == 0) config->accidental = 1;
        }

        // QUANTIZE
        else if (strncmp(configBuffer, "QUANTIZE=", 9) == 0)
        {
            if (configBuffer[9] != '\0')
                config->quantizeValue = (int16_t)(CLAMP(atoi(&configBuffer[9]), 0, 63));
        }

        // TRANSDEL
        else if (strncmp(configBuffer, "TRANSDEL=", 9) == 0)
        {
                 if (strncmp(&configBuffer[9], "TRUE",  4) == 0) config->transdel = true;
            else if (strncmp(&configBuffer[9], "FALSE", 5) == 0) config->transdel = false;
        }

        // MODDOT
        else if (strncmp(configBuffer, "MODDOT=", 7) == 0)
        {
                 if (strncmp(&configBuffer[7], "TRUE",  4) == 0) config->moddot = true;
            else if (strncmp(&configBuffer[7], "FALSE", 5) == 0) config->moddot = false;
        }

        // SCALE3X
        else if (strncmp(configBuffer, "SCALE3X=", 8) == 0)
        {
                 if (strncmp(&configBuffer[8], "TRUE",  4) == 0) config->scale3x = true;
            else if (strncmp(&configBuffer[8], "FALSE", 5) == 0) config->scale3x = false;
        }

        // DEFAULTDIR
        else if (strncmp(configBuffer, "DEFAULTDIR=", 11) == 0)
        {
            i = 11;
            while (configBuffer[i] == ' ') i++;                 // remove spaces before string (if present)
            while (configBuffer[lineLen - 1] == ' ') lineLen--; // remove spaces after string  (if present)

            lineLen -= i;

            if (lineLen > 0)
                strncpy(config->defaultDiskOpDir, &configBuffer[i], (lineLen > PATH_MAX_LEN) ? PATH_MAX_LEN : lineLen);
        }

        // A4000LOWPASSFILTER
        else if (strncmp(configBuffer, "A4000LOWPASSFILTER=", 19) == 0)
        {
                 if (strncmp(&configBuffer[19], "TRUE",  4) == 0) config->a4000LowPassFilter = true;
            else if (strncmp(&configBuffer[19], "FALSE", 5) == 0) config->a4000LowPassFilter = false;
        }

        // FREQUENCY
        else if (strncmp(configBuffer, "FREQUENCY=", 10) == 0)
        {
            if (configBuffer[10] != '\0')
                config->soundFrequency = (uint32_t)(CLAMP(atoi(&configBuffer[10]), 44100, 48000));
        }

        // STEREOSEPARATION
        else if (strncmp(configBuffer, "STEREOSEPARATION=", 17) == 0)
        {
            if (configBuffer[17] != '\0')
                config->stereoSeparation = (int8_t)(CLAMP(atoi(&configBuffer[17]), 0, 100));
        }

        configBuffer = strtok(NULL, "\n");
    }

    free(configBuffer);

    editor.ui.scale3x    = config->scale3x;
    editor.diskop.moddot = config->moddot;
    editor.ui.blankzero  = config->blankzero;
    editor.Accidental    = config->accidental;
    editor.QuantizeValue = config->quantizeValue;
    editor.TransDelFlag  = config->transdel;
    editor.oldBPM        = editor.initialTempo;

    // Load PT.Config if exists...
#ifndef WIN32
    // massive hack to set dir to binary dir (thank you SDL, not!)

    dladdr((void *)(&loadConfig), &info);
    memset(editor.tempPath, 0, PATH_MAX_LEN);
    realpath(info.dli_fname, editor.tempPath);

    pathLen = strlen(editor.tempPath);
    for (i = pathLen; i != 0; --i)
    {
        if (editor.tempPath[i] == '/') // /app_name
        {
            editor.tempPath[i] = '\0';
            break;
        }
    }

    chdir(editor.tempPath);
#ifdef __APPLE__
    chdir("../../../"); // package.app/Contents/MacOS
#endif
    configFile = fopen("PT.Config", "rb"); // PT didn't read PT.Config with no number, but let's support it
    if (configFile == NULL)
    {
        for (i = 0; i < 99; ++i)
        {
            sprintf(cfgString, "PT.Config-%02d", i);

            configFile = fopen(cfgString, "rb");
            if (configFile != NULL)
                break;
        }

        if (i == 99) return (true);
    }
#else
    configFile = fopen("PT.Config", "rb"); // PT didn't read PT.Config with no number, but let's support it
    if (configFile == NULL)
    {
        for (i = 0; i < 99; ++i)
        {
            sprintf(cfgString, "PT.Config-%02d", i);

            configFile = fopen(cfgString, "rb");
            if (configFile != NULL)
                break;
        }

        if (i == 99) return (true);
    }
#endif

    fseek(configFile, 0, SEEK_END);
    configFileSize = ftell(configFile);

    if (configFileSize != 1024)
    {
        fclose(configFile);
        return (true);
    }

    fseek(configFile, 6, SEEK_SET);
    fread(cfgString, 1, 18, configFile);

    if (strncmp(cfgString, "Configuration File", 18))
    {
        fclose(configFile);
        return (true);
    }

    // most likely a valid PT.Config, let's load settings

    // Palette
    fseek(configFile, 154, SEEK_SET);
    for (i = 0; i < 8; ++i)
    {
        fread(&tmp16, 2, 1, configFile);
        if (!bigEndian) tmp16 = SWAP16(tmp16);

        r = ((tmp16 & 0x0F00) >> 8) * 17;
        g = ((tmp16 & 0x00F0) >> 4) * 17;
        b = ((tmp16 & 0x000F) >> 0) * 17;

        palette[i] = 0xFF000000 | ((r << 16) | (g << 8) | (b));
    }

    // Transpose Delete (delete out of range notes on transposing)
    fseek(configFile, 174, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    config->transdel    = tmp8 ? true : false;
    editor.TransDelFlag = config->transdel;

    // Note Accidental (sharp/flat)
    fseek(configFile, 200, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    config->accidental = tmp8 ? 1 : 0;
    editor.Accidental  = config->accidental;

    // Multi Mode Next
    fseek(configFile, 462, SEEK_SET);
    fread(&editor.MultiModeNext[0], 1, 1, configFile);
    fread(&editor.MultiModeNext[1], 1, 1, configFile);
    fread(&editor.MultiModeNext[2], 1, 1, configFile);
    fread(&editor.MultiModeNext[3], 1, 1, configFile);

    // Effect Macros
    fseek(configFile, 466, SEEK_SET);
    for (i = 0; i < 10; ++i)
    {
        fread(&tmp16, 2, 1, configFile);
        if (!bigEndian) tmp16 = SWAP16(tmp16);

        editor.EffectMacros[i] = tmp16;
    }

    // Timing Mode (CIA/VBLANK)
    fseek(configFile, 487, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    editor.timingMode = tmp8 ? TEMPO_MODE_CIA : TEMPO_MODE_VBLANK;

    // Blank Zeroes
    fseek(configFile, 490, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    config->blankzero   = tmp8 ? true : false;
    editor.ui.blankzero = config->blankzero;

    // Initial Tempo (don't load of timing is set to vblank)
    if (editor.timingMode == TEMPO_MODE_CIA)
    {
        fseek(configFile, 497, SEEK_SET);
        fread(&tmp8, 1, 1, configFile);
        if (tmp8 < 32) tmp8 = 32;
    }

    // Tuning Tone Note
    fseek(configFile, 501, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    if (tmp8 > 35) tmp8 = 35;

    // Tuning Tone Volume
    fseek(configFile, 503, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    if (tmp8 > 64) tmp8 = 64;

    // Initial Speed
    fseek(configFile, 545, SEEK_SET);
    fread(&tmp8, 1, 1, configFile);
    if (editor.timingMode == TEMPO_MODE_VBLANK)
    {
        editor.initialSpeed = tmp8;
    }
    else
    {
        if (tmp8 > 31) tmp8 = 31;
        editor.initialSpeed = tmp8;
    }

    // VU-Meter Colors
    fseek(configFile, 546, SEEK_SET);
    for (i = 0; i < 48; ++i)
    {
        fread(&VUmeterColors[i], 2, 1, configFile);
        if (!bigEndian) VUmeterColors[i] = SWAP16(VUmeterColors[i]);
    }

    // Spectrum Analyzer Colors
    fseek(configFile, 642, SEEK_SET);
    for (i = 0; i < 36; ++i)
    {
        fread(&AnalyzerColors[i], 2, 1, configFile);
        if (!bigEndian) AnalyzerColors[i] = SWAP16(AnalyzerColors[i]);
    }

    fclose(configFile);

    return (true);
}
