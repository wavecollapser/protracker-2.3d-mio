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
#include <SDL/SDL.h>
#ifdef USE_OPENGL
#include <SDL/SDL_opengl.h>
#endif
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include "pt_header.h"
#include "pt_palette.h"
#include "pt_buttons.h"
#include "pt_helpers.h"
#include "pt_tables.h"
#include "pt_mouse.h"
#include "pt_diskop.h"
#include "pt_sampler.h"
#include "pt_config.h"
#include "pt_visuals.h"
#include "pt_edit.h"
#include "pt_modloader.h"

MODULE *mod = NULL; // globalized
uint8_t bigEndian;  // globalized

// accessed by pt_visuals.c
uint32_t *tempScreenBuffer = NULL;
SDL_Surface *screen        = NULL;
SDL_Surface *iconSurface   = NULL;
SDL_VideoInfo tmpVidInfo;

#ifdef USE_OPENGL
    GLuint texture;
#else
    uint32_t *frameBuffer          = NULL;
    SDL_Surface *tempScreenSurface = NULL;
#endif
// -----------------------------

#ifdef WIN32
    // for taking control over windows key on keyboard when program is in focus
    uint8_t windowsKeyIsDown;
    HHOOK g_hKeyboardHook;
#endif

static PTCONFIG config;
static SDL_TimerID timer50Hz;

int8_t initializeVars(void);

void cleanUp(void) // never call this inside the main loop!
{
    audioClose();

    SDL_RemoveTimer(timer50Hz);

    modFree(mod);

    if (editor.rowVisitTable != NULL) free(editor.rowVisitTable);
    if (editor.ui.pattNames  != NULL) free(editor.ui.pattNames);

    deAllocSamplerVars();
    deAllocDiskOpVars();
    freeDiskOpFileMem();
    freeBitmaps();

    freeDisplayBuffers();

    SDL_FreeSurface(screen);
    SDL_FreeSurface(iconSurface);

#ifdef WIN32
    UnhookWindowsHookEx(g_hKeyboardHook);
#endif
}

int main(int argc, char *argv[])
{
    int32_t ticker60HzDelay;
    uint32_t ticker60HzNow;
    uint32_t ticker60HzNext;
    uint32_t ticker60HzInterval;

    SDL_Event inputEvent;

    MODULE *newMod;

    // very first thing to do is to set a big endian flag using a well-known hack
    // DO *NOT* run this test later in the code, as some macros depend on the flag!
    union
    {
        uint32_t a;
        uint8_t b[4];
    } endianTest;

    endianTest.a = 1;
    bigEndian    = endianTest.b[3];
    // ----------------------------

#ifdef WIN32
    SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);

    // for taking control over windows key on keyboard if app has focus
    windowsKeyIsDown = 0;
    g_hKeyboardHook  = SetWindowsHookEx(WH_KEYBOARD_LL, lowLevelKeyboardProc, GetModuleHandle(NULL), 0);
#endif

    // error messages are written to stderr

    if (!initializeVars())
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    if (!loadConfig(&config)) // returns false on mem alloc failure
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    if (!setupVideo())
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    if (!setupAudio(&config))
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    if (!unpackBitmaps())
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    diskOpSetInitPath(&config);

    mod = createNewMod();
    if (mod == NULL)
    {
        cleanUp();
        SDL_Quit();

        return (1);
    }

    setPlayerModSource(mod);
    modSetTempo(editor.initialTempo);
    modSetSpeed(editor.initialSpeed);

    updateWindowTitle(mod, false);

    pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
    setStatusMessage("ALL RIGHT", DO_CARRY);
    setStatusMessage("PROTRACKER V2.3D", NO_CARRY);

    // load a .MOD from the command arguments if passed
    if ((argc >= 2) && (strlen(argv[1]) > 0))
    {
        editor.ui.introScreenShown = false;
        setStatusMessage("ALL RIGHT", DO_CARRY);

        newMod = modLoad(argv[1]);
        if (newMod != NULL)
        {
            mod->moduleLoaded = false;
            modFree(mod);

            setPlayerModSource(newMod);
            setMainModSource(newMod);

            modSetSpeed(6);

            if (mod->head.initBPM > 0)
                modSetTempo(mod->head.initBPM);
            else
                modSetTempo(125);

            modSetPos(0, 0);

            mod->moduleLoaded = true;
        }
        else
        {
            input.mouse.buttonErrorMsgActive = true;
            input.mouse.buttonErrorMsgBlock  = true;
            input.mouse.errorMsgCounter      = 0;

            // status/error message is set in the mod loader
            pointerErrorMode();
        }
    }

    if ((argc == 3) && (!strcmp(argv[2], "/autoplay")))
    {
        editor.playMode = PLAY_MODE_NORMAL;

        modPlay(DONT_SET_PATTERN, 0, DONT_SET_ROW);

        editor.currentMode = MODE_PLAY;
        pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
        setStatusMessage("ALL RIGHT", DO_CARRY);
    }

    // set up timer stuff
    ticker60HzInterval = 1 * 1000 / 60; // 60 as in 60Hz
    ticker60HzNext     = SDL_GetTicks() + ticker60HzInterval;
    timer50Hz          = SDL_AddTimer(1000 / 50, _50HzCallBack, NULL);

    editor.programRunning = true;
    while (editor.programRunning)
    {
        SDL_GetMouseState(&input.mouse.x, &input.mouse.y);

        if (editor.fullScreenFlag)
        {
            input.mouse.x = (int32_t)((float)(input.mouse.x) / input.mouse.scale);
            input.mouse.y = (int32_t)((float)(input.mouse.y) / input.mouse.scale);

            if (input.mouse.x >= SCREEN_W)
            {
                input.mouse.x = SCREEN_W - 1;
                SDL_WarpMouse((uint16_t)((float)(input.mouse.x) * input.mouse.scale), (uint16_t)((float)(input.mouse.y) * input.mouse.scale));
            }

            if (input.mouse.y >= SCREEN_H)
            {
                input.mouse.y = SCREEN_H - 1;
                SDL_WarpMouse((uint16_t)((float)(input.mouse.x) * input.mouse.scale), (uint16_t)((float)(input.mouse.y) * input.mouse.scale));
            }
        }
        else
        {
            input.mouse.x = CLAMP(input.mouse.x, 0, (SCREEN_W * (editor.ui.scale3x ? 3 : 2)) - 1) / (editor.ui.scale3x ? 3 : 2);
            input.mouse.y = CLAMP(input.mouse.y, 0, (SCREEN_H * (editor.ui.scale3x ? 3 : 2)) - 1) / (editor.ui.scale3x ? 3 : 2);
        }

        while (SDL_PollEvent(&inputEvent))
        {
            updateKeyModifiers(); // set/clear CTRL/ALT/SHIFT/AMIGA states

            if (inputEvent.type == SDL_QUIT)
            {
                editor.MixFlag                = false;
                editor.swapChannelFlag        = false;
                editor.ui.changingChordNote   = false;
                editor.ui.samplerVolBoxShown  = false;
                editor.ui.changingSmpResample = false;

                if (editor.ui.getLineFlag)
                    exitGetTextLine(mod, EDIT_TEXT_NO_UPDATE);

                if (mod->modified)
                {
                    editor.ui.askScreenShown = true;
                    editor.ui.askScreenType = ASK_QUIT;

                    pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                    setStatusMessage("REALLY QUIT ?", NO_CARRY);
                }
                else
                {
                    editor.ui.throwExit = true;
                }
            }
            else if (inputEvent.type == SDL_KEYUP)
            {
                keyUpHandler(inputEvent.key.keysym.sym);
            }
            else if (inputEvent.type == SDL_KEYDOWN)
            {
                keyDownHandler(inputEvent.key.keysym.sym, mod);
            }
            else if (inputEvent.type == SDL_MOUSEBUTTONUP)
            {
                mouseButtonUpHandler(inputEvent.button.button, mod);
            }
            else if (inputEvent.type == SDL_MOUSEBUTTONDOWN)
            {
                if (!editor.ui.askScreenShown && editor.ui.introScreenShown)
                {
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                    editor.ui.introScreenShown = false;
                }

                if ((editor.ui.sampleMarkingPos == -1) && !editor.ui.forceSampleDrag && !editor.ui.forceVolDrag && !editor.ui.forceSampleEdit)
                    mouseButtonDownHandler(inputEvent.button.button, mod);
            }

            if (editor.ui.throwExit)
            {
                editor.programRunning       = false;
                editor.ui.diskOpScreenShown = false;

                if (editor.diskop.isFilling)
                {
                    editor.diskop.isFilling = false;

                    editor.diskop.forceStopReading = true;
                    SDL_WaitThread(editor.diskop.fillThread, NULL);
                }

                if (editor.isWAVRendering)
                {
                    editor.isWAVRendering = false;
                    SDL_WaitThread(editor.mod2WavThread, NULL);
                }

                setStatusMessage("FREE'ING MEM...", NO_CARRY);
            }
        }

        renderFrame();

        if (editor.ui.sampleMarkingPos >= 0) samplerSamplePressed(MOUSE_BUTTON_HELD);
        if (editor.ui.forceSampleDrag)       samplerBarPressed(MOUSE_BUTTON_HELD);
        if (editor.ui.forceSampleEdit)       samplerEditSample(MOUSE_BUTTON_HELD);
        if (editor.ui.forceVolDrag)          volBoxBarPressed(MOUSE_BUTTON_HELD);

        blitFrame();

        // Crude but fast 60Hz frame limiter
        ticker60HzNow   = SDL_GetTicks();
        ticker60HzDelay = ticker60HzNext - ticker60HzNow;

        if (ticker60HzDelay > 0)
            SDL_Delay(ticker60HzDelay);

        ticker60HzNext += ticker60HzInterval;
        // --------------------------------

        // AFTER-FRAME ROUTINES
        if (input.mouse.buttonWaiting)
        {
            if (++input.mouse.buttonWaitCounter > 8)
            {
                input.mouse.buttonWaitCounter = 0;
                input.mouse.buttonWaiting     = false;
            }
        }

        if (input.mouse.buttonErrorMsgActive)
        {
            if (++input.mouse.errorMsgCounter >= 60)
            {
                input.mouse.errorMsgCounter = 0;

                pointerSetPreviousMode();
                setPrevStatusMessage();

                input.mouse.buttonErrorMsgBlock  = false;
                input.mouse.buttonErrorMsgActive = false;
            }
        }

        handleKeyRepeat(input.keyb.lastKey, mod);

        if (!input.mouse.buttonWaiting && (editor.ui.sampleMarkingPos == -1) && !editor.ui.forceSampleDrag && !editor.ui.forceVolDrag && !editor.ui.forceSampleEdit)
            handleMouseButtons(mod);
        // --------------------------------
    }

    cleanUp();

    SDL_Quit();

    return (0);
}

int8_t initializeVars(void)
{
    memset(&input,  0, sizeof (input));
    memset(&editor, 0, sizeof (editor));
    memset(&audio,  0, sizeof (audio));

    if (!allocSamplerVars()) return (false);
    if (!allocDiskOpVars())  return (false);

    editor.rowVisitTable = (uint8_t *)(malloc(MOD_ORDERS * MOD_ROWS));
    if (editor.rowVisitTable == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    editor.ui.pattNames = (char *)(calloc(MAX_PATTERNS, 16));
    if (editor.ui.pattNames == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    input.mouse.x = SCREEN_W / 2;
    input.mouse.y = SCREEN_H / 2;

    editor.F6pos                  = 0;
    editor.F7pos                  = 16;
    editor.F8pos                  = 32;
    editor.F9pos                  = 48;
    editor.F10pos                 = 63;
    editor.MarkEnd                = 2;
    editor.tuningVol              = 32;
    editor.SampleVol              = 100; // percentage
    editor.MarkStart              = 2;
    editor.tuningNote             = 24; // C-3
    editor.MetroSpeed             = 4;
    editor.Accidental             = 0;
    editor.MarkEndOfs             = 0;
    editor.MarkStartOfs           = -1;
    editor.initialTempo           = 125;
    editor.initialSpeed           = 6;
    editor.ResampleNote           = 24; // C-3
    editor.QuantizeValue          = 1;
    editor.EditMoveAdd            = 1;
    editor.currentPlayNote        = 24; // C-3
    editor.MultiModeNext[0]       = 2;
    editor.MultiModeNext[1]       = 3;
    editor.MultiModeNext[2]       = 4;
    editor.MultiModeNext[3]       = 1;
    editor.ui.visualizerMode      = VISUAL_QUADRASCOPE;
    editor.ui.introScreenShown    = true;
    editor.ui.sampleMarkingPos    = -1;
    editor.ui.previousPointerMode = editor.ui.pointerMode;

    editor.EffectMacros[0] = 0x0102;
    editor.EffectMacros[1] = 0x0202;
    editor.EffectMacros[2] = 0x0037;
    editor.EffectMacros[3] = 0x0047;
    editor.EffectMacros[4] = 0x0304;
    editor.EffectMacros[5] = 0x0F06;
    editor.EffectMacros[6] = 0x0C10;
    editor.EffectMacros[7] = 0x0C20;
    editor.EffectMacros[8] = 0x0E93;
    editor.EffectMacros[9] = 0x0A0F;

    // chord maker (Edit Op. Screen #4)
    editor.Note1    = 36;
    editor.Note2    = 36;
    editor.Note3    = 36;
    editor.Note4    = 36;
    editor.OldNote1 = 36;
    editor.OldNote2 = 36;
    editor.OldNote3 = 36;
    editor.OldNote4 = 36;

    // sampler screen (volume box)
    editor.Vol1 = 100;
    editor.Vol2 = 100;

    strcpy(editor.MixText, "MIX 01+02 TO 03");

    // set up certain display pointers
    editor.currentSampleDisp = &editor.currentSample;
    editor.QuantizeValueDisp = &editor.QuantizeValue;
    editor.MetroSpeedDisp    = &editor.MetroSpeed;
    editor.MetroChannelDisp  = &editor.MetroChannel;
    editor.SampleFromDisp    = &editor.SampleFrom;
    editor.SampleToDisp      = &editor.SampleTo;
    editor.SamplePosDisp     = &editor.SamplePos;
    editor.SampleVolDisp     = &editor.SampleVol;
    editor.ChordLengthDisp   = &editor.ChordLength;
    editor.Vol1Disp          = &editor.Vol1;
    editor.Vol2Disp          = &editor.Vol2;

    initEasterEgg();

    return (true);
}

void setMainModSource(MODULE *modEntry)
{
    mod = modEntry;
}
