/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <stdio.h>
#include <stdint.h>
#include <SDL/SDL.h>
#ifdef WIN32
#include <windows.h>
#endif
#include "pt_header.h"
#include "pt_helpers.h"
#include "pt_visuals.h"
#include "pt_palette.h"
#include "pt_diskop.h"
#include "pt_edit.h"
#include "pt_sampler.h"
#include "pt_audio.h"
#include "pt_buttons.h"
#include "pt_tables.h"
#include "pt_modloader.h"

void sampleUpButton(MODULE *modEntry);   // pt_mouse.c
void sampleDownButton(MODULE *modEntry); // pt_mouse.c

// button tables taken from the ptplay project + modified

static const pt_button_s bAsk[] =
{
    {171, 71,196, 81, PTB_SUREY},
    {234, 71,252, 81, PTB_SUREN}
};

static const pt_button_s bClear[] =
{
    {166, 57,198, 67, PTB_CLEARSONG},
    {204, 57,257, 67, PTB_CLEARSAMPLES},
    {166, 73,198, 83, PTB_CLEARALL},
    {204, 73,257, 83, PTB_CLEARCANCEL}
};

static const pt_button_s bTopScreen[] =
{
    {120,  0,181, 10, PTB_PLAY},
    {120, 11,181, 21, PTB_PATTERN},
    {120, 21,181, 32, PTB_EDIT},
    {120, 33,181, 43, PTB_RECORD},
    {182,  0,243, 10, PTB_STOP},
    {182, 11,243, 21, PTB_CLEAR},
    {182, 21,243, 32, PTB_EDITOP},
    {182, 33,243, 43, PTB_DISKOP},
    {244,  0,305, 10, PTB_MOD2WAV},
    {244, 21,305, 32, PTB_POSED},
    {244, 11,305, 21, PTB_PAT2SMP},
    {244, 33,305, 43, PTB_SAMPLER},
    {306,  0,319, 10, PTB_CHAN1},
    {306, 11,319, 21, PTB_CHAN2},
    {306, 22,319, 32, PTB_CHAN3},
    {306, 33,319, 43, PTB_CHAN4},
    {306, 44,319, 54, PTB_ABOUT},
    {  0,  0, 39, 10, PTB_POSED},
    { 40,  0, 50, 10, PTB_POSINS},
    { 51,  0, 61, 10, PTB_POSDEL},
    { 62,  0, 97, 10, PTB_POSS},
    { 98,  0,108, 10, PTB_POSU},
    {109,  0,119, 10, PTB_POSD},
    { 62, 11, 97, 21, PTB_PATTERNS},
    { 98, 11,108, 21, PTB_PATTERNU},
    {109, 11,119, 21, PTB_PATTERND},
    { 62, 22, 97, 32, PTB_LENGTHS},
    { 98, 22,108, 32, PTB_LENGTHU},
    {109, 22,119, 32, PTB_LENGTHD},
    { 98, 33,108, 43, PTB_FTUNEU},
    {109, 33,119, 43, PTB_FTUNED},
    { 62, 44,97, 54,  PTB_SAMPLES},
    { 98, 44,108, 54, PTB_SAMPLEU},
    {109, 44,119, 54, PTB_SAMPLED},
    { 62, 55, 97, 65, PTB_SVOLUMES},
    { 98, 55,108, 65, PTB_SVOLUMEU},
    {109, 55,119, 65, PTB_SVOLUMED},
    { 54, 66, 97, 76, PTB_SLENGTHS},
    { 98, 66,108, 76, PTB_SLENGTHU},
    {109, 66,119, 76, PTB_SLENGTHD},
    { 54, 77, 97, 87, PTB_SREPEATS},
    { 98, 77,108, 87, PTB_SREPEATU},
    {109, 77,119, 87, PTB_SREPEATD},
    { 54, 88, 97, 98, PTB_SREPLENS},
    { 98, 88,108, 98, PTB_SREPLENU},
    {109, 88,119, 98, PTB_SREPLEND},
    {120, 44,319, 98, PTB_VISUALS}
};

static const pt_button_s bMidScreen[] =
{
    {  0, 99,319,109, PTB_SONGNAME},
    {  0,110,286,120, PTB_SAMPLENAME},
    {287,110,319,120, PTB_LOADSAMPLE}
};

static const pt_button_s bBotScreen[] =
{
    { 26,121, 43,137, PTB_TEMPOU},
    { 43,121, 59,137, PTB_TEMPOD},
    {  0,121, 25,137, PTB_PATTBOX},
    {  0,138,319,254, PTB_PATTDATA}
};

static const pt_button_s bDiskOp[] =
{
    {  0,  0, 79, 21, PTB_DO_BADGE},
    { 80,  0,145, 10, PTB_DO_PACKMOD},
    {146,  0,155, 10, PTB_DO_MODARROW},
    {156,  0,237, 10, PTB_DO_LOADMODULE},
    {238,  0,319, 10, PTB_DO_SAVEMODULE},
    { 80, 11,145, 21, PTB_DO_SAMPLEFORMAT},
    {146, 11,155, 21, PTB_DO_SAMPLEARROW},
    {156, 11,237, 21, PTB_DO_LOADSAMPLE},
    {238, 11,319, 21, PTB_DO_SAVESAMPLE},
    {  0, 22,237, 32, PTB_DO_DATAPATH},
    {238, 22,272, 32, PTB_DO_PARENT},
    {273, 22,307, 32, PTB_DO_REFRESH},
    {308, 22,319, 30, PTB_DO_SCROLLTOP},
    {308, 31,319, 39, PTB_DO_SCROLLUP},
    {308, 40,319, 81, PTB_DO_EXIT},
    {308, 82,319, 89, PTB_DO_SCROLLDOWN},
    {308, 90,319, 99, PTB_DO_SCROLLBOT},
    {  2, 33,304, 93, PTB_DO_FILEAREA}
};

static const pt_button_s bPosEd[] =
{
    { 120,  0,171, 10, PTB_POSINS},
    { 120, 11,171, 21, PTB_POSDEL},
    { 120, 22, 177,98, PTB_PE_PATT},
    { 178, 22, 307,98, PTB_PE_PATTNAME},
    { 308, 22, 319,32, PTB_PE_SCROLLTOP},
    { 308, 33, 319,43, PTB_PE_SCROLLUP},
    { 308, 44, 319,76, PTB_PE_EXIT},
    { 308, 77, 319,87, PTB_PE_SCROLLDOWN},
    { 308, 88, 319,98, PTB_PE_SCROLLBOT},
    { 172,  0,267, 21, PTB_STOP},
    { 268,  0,319, 10, PTB_PLAY},
    { 268, 11,319, 21, PTB_PATTERN}
};

static const pt_button_s bEditOp1[] =
{
    {120, 44,305, 54, PTB_EO_TITLEBAR},
    {306, 44,319, 54, PTB_DUMMY},
    {306, 55,319, 65, PTB_DUMMY},
    {306, 66,319, 76, PTB_EO_2},
    {306, 77,319, 87, PTB_EO_3},
    {306, 88,319, 98, PTB_EO_EXIT},
    {120, 55,212, 65, PTB_EO_TRACK_NOTE_UP},
    {213, 55,305, 65, PTB_EO_PATT_NOTE_UP},
    {120, 66,212, 76, PTB_EO_TRACK_NOTE_DOWN},
    {213, 66,305, 76, PTB_EO_PATT_NOTE_DOWN},
    {120, 77,212, 87, PTB_EO_TRACK_OCTA_UP},
    {213, 77,305, 87, PTB_EO_PATT_OCTA_UP},
    {120, 88,212, 98, PTB_EO_TRACK_OCTA_DOWN},
    {213, 88,305, 98, PTB_EO_PATT_OCTA_DOWN}
};

static const pt_button_s bEditOp2[] =
{
    {120, 44,305, 54, PTB_EO_TITLEBAR},
    {306, 44,319, 54, PTB_DUMMY},
    {306, 55,319, 65, PTB_EO_1},
    {306, 66,319, 76, PTB_DUMMY},
    {306, 77,319, 87, PTB_EO_3},
    {306, 88,319, 98, PTB_EO_EXIT},
    {120, 55,212, 65, PTB_EO_RECORD},
    {213, 55,259, 65, PTB_EO_DELETE},
    {260, 55,305, 65, PTB_EO_KILL},
    {120, 66,212, 76, PTB_EO_QUANTIZE},
    {213, 66,259, 76, PTB_EO_EXCHGE},
    {260, 66,305, 76, PTB_EO_COPY},
    {120, 77,188, 87, PTB_EO_METRO_1},
    {189, 77,212, 87, PTB_EO_METRO_2},
    {213, 77,259, 87, PTB_EO_FROM},
    {260, 77,283, 87, PTB_EO_FROM_NUM},
    {284, 77,294, 87, PTB_EO_FROM_UP},
    {295, 77,305, 87, PTB_EO_FROM_DOWN},
    {120, 88,212, 98, PTB_EO_KEYS},
    {213, 88,259, 98, PTB_EO_TO},
    {260, 88,283, 98, PTB_EO_TO_NUM},
    {284, 88,294, 98, PTB_EO_TO_UP},
    {295, 88,305, 98, PTB_EO_TO_DOWN}
};

static const pt_button_s bEditOp3[] =
{
    {120, 44,305, 54, PTB_EO_TITLEBAR},
    {306, 44,319, 54, PTB_DUMMY},
    {306, 55,319, 65, PTB_EO_1},
    {306, 66,319, 76, PTB_EO_2},
    {306, 77,319, 87, PTB_DUMMY},
    {306, 88,319, 98, PTB_EO_EXIT},
    {120, 55,165, 65, PTB_EO_MIX},
    {166, 55,212, 65, PTB_EO_ECHO},
    {213, 55,237, 65, PTB_DUMMY},
    {238, 55,283, 65, PTB_EO_POS_NUM},
    {284, 55,294, 65, PTB_EO_POS_UP},
    {295, 55,305, 65, PTB_EO_POS_DOWN},
    {120, 66,165, 76, PTB_EO_BOOST},
    {166, 66,212, 76, PTB_EO_FILTER},
    {213, 66,243, 76, PTB_EO_MOD},
    {244, 66,283, 76, PTB_DUMMY},
    {284, 66,294, 76, PTB_EO_MOD_UP},
    {295, 66,305, 76, PTB_EO_MOD_DOWN},
    {120, 77,165, 87, PTB_EO_X_FADE},
    {166, 77,212, 87, PTB_EO_BACKWD},
    {213, 77,230, 87, PTB_EO_CB},
    {231, 77,269, 87, PTB_EO_CHORD},
    {270, 77,287, 87, PTB_EO_FU},
    {288, 77,305, 87, PTB_EO_FD},
    {120, 88,165, 98, PTB_EO_UPSAMP},
    {166, 88,212, 98, PTB_EO_DNSAMP},
    {213, 88,243, 98, PTB_EO_VOL},
    {244, 88,283, 98, PTB_EO_VOL_NUM},
    {284, 88,294, 98, PTB_EO_VOL_UP},
    {295, 88,305, 98, PTB_EO_VOL_DOWN}
};

static const pt_button_s bEditOp4[] =
{
    {120, 44,305, 54, PTB_EO_TITLEBAR},
    {306, 44,319, 54, PTB_DUMMY},
    {306, 55,319, 65, PTB_EO_1},
    {306, 66,319, 76, PTB_EO_2},
    {306, 77,319, 87, PTB_EO_3},
    {306, 88,319, 98, PTB_EO_EXIT},
    {120, 55,165, 65, PTB_EO_DOCHORD},
    {166, 55,204, 65, PTB_EO_MAJOR},
    {205, 55,251, 65, PTB_EO_MAJOR7},
    {251, 55,283, 65, PTB_EO_NOTE1},
    {284, 55,294, 65, PTB_EO_NOTE1_UP},
    {295, 55,305, 65, PTB_EO_NOTE1_DOWN},
    {120, 66,165, 76, PTB_EO_RESET},
    {166, 66,204, 76, PTB_EO_MINOR},
    {205, 66,251, 76, PTB_EO_MINOR7},
    {251, 66,283, 76, PTB_EO_NOTE2},
    {284, 66,294, 76, PTB_EO_NOTE2_UP},
    {295, 66,305, 76, PTB_EO_NOTE2_DOWN},
    {120, 77,165, 87, PTB_EO_UNDO},
    {166, 77,204, 87, PTB_EO_SUS4},
    {205, 77,251, 87, PTB_EO_MAJOR6},
    {251, 77,283, 87, PTB_EO_NOTE3},
    {284, 77,294, 87, PTB_EO_NOTE3_UP},
    {295, 77,305, 87, PTB_EO_NOTE3_DOWN},
    {120, 88,157, 98, PTB_EO_LENGTH},
    {158, 88,204, 98, PTB_DUMMY},
    {205, 88,251, 98, PTB_EO_MINOR6},
    {251, 88,283, 98, PTB_EO_NOTE4},
    {284, 88,294, 98, PTB_EO_NOTE4_UP},
    {295, 88,305, 98, PTB_EO_NOTE4_DOWN}
};

static const pt_button_s bSampler[] =
{ 
    {  6,124, 25,134, PTB_SA_EXIT},
    { 32,212, 95,220, PTB_SA_PLAYWAVE},
    { 96,212,175,220, PTB_SA_SHOWRANGE},
    {176,212,245,220, PTB_SA_ZOOMOUT},
    {  0,223, 30,242, PTB_SA_STOP},
    { 32,223, 95,231, PTB_SA_PLAYDISPLAYED},
    { 96,223,175,231, PTB_SA_SHOWALL},
    {176,223,245,231, PTB_SA_RANGEALL},
    {246,223,319,231, PTB_SA_LOOP},
    { 32,234, 94,242, PTB_SA_PLAYRANGE},
    {136,234,174,242, PTB_SA_RANGECENTER},
    { 96,234,114,242, PTB_SA_RANGEBEG},
    {116,234,135,242, PTB_SA_RANGEEND},
    {  0,245, 31,256, PTB_SA_CUT},
    { 32,245, 63,256, PTB_SA_COPY},
    { 64,245, 95,256, PTB_SA_PASTE},
    { 96,245,135,256, PTB_SA_VOLUME},
    {136,245,175,256, PTB_SA_TUNETONE},
    {176,245,210,256, PTB_SA_FIXDC},
    {211,245,245,256, PTB_SA_INVERT},
    {176,234,245,242, PTB_SA_RESAMPLE},
    {246,234,319,242, PTB_SA_RESAMPLENOTE},
    {  0,138,319,201, PTB_SA_SAMPLEAREA},
    {  3,205,316,210, PTB_SA_ZOOMBARAREA}
};

#define ASK_BUTTONS       (sizeof (bAsk)       / sizeof (pt_button_s))
#define CLEAR_BUTTONS     (sizeof (bClear)     / sizeof (pt_button_s))
#define TOPSCREEN_BUTTONS (sizeof (bTopScreen) / sizeof (pt_button_s))
#define MIDSCREEN_BUTTONS (sizeof (bMidScreen) / sizeof (pt_button_s))
#define BOTSCREEN_BUTTONS (sizeof (bBotScreen) / sizeof (pt_button_s))
#define DISKOP_BUTTONS    (sizeof (bDiskOp)    / sizeof (pt_button_s))
#define POSED_BUTTONS     (sizeof (bPosEd)     / sizeof (pt_button_s))
#define EDITOP1_BUTTONS   (sizeof (bEditOp1)   / sizeof (pt_button_s))
#define EDITOP2_BUTTONS   (sizeof (bEditOp2)   / sizeof (pt_button_s))
#define EDITOP3_BUTTONS   (sizeof (bEditOp3)   / sizeof (pt_button_s))
#define EDITOP4_BUTTONS   (sizeof (bEditOp4)   / sizeof (pt_button_s))
#define SAMPLER_BUTTONS   (sizeof (bSampler)   / sizeof (pt_button_s))

#ifdef WIN32
extern uint8_t windowsKeyIsDown;
extern HHOOK g_hKeyboardHook;
#endif

void movePatCurPrevCh(void);
void movePatCurNextCh(void);
void movePatCurRight(void);
void movePatCurLeft(void);

int8_t handleGeneralModes(SDLKey keyEntry, MODULE *modEntry);
int8_t handleTextEditMode(SDLKey keyEntry, MODULE *modEntry);

void updateKeyModifiers(void)
{
    uint8_t *keyState;

    keyState = SDL_GetKeyState(NULL);

    editor.NoteRepFlag = (SDL_GetModState() & KMOD_CAPS) ? true : false;

    input.keyb.controlKeyDown     = keyState[SDLK_LCTRL]  | keyState[SDLK_RCTRL];
    input.keyb.altKeyDown         = keyState[SDLK_LALT]   | keyState[SDLK_RALT] | keyState[SDLK_MODE];
    input.keyb.shiftKeyDown       = keyState[SDLK_LSHIFT] | keyState[SDLK_RSHIFT];
    input.keyb.keypadEnterKeyDown = keyState[SDLK_KP_ENTER];

#ifndef WIN32 // MS Windows: handled in lowLevelKeyboardProc
    input.keyb.leftAmigaKeyDown = keyState[SDLK_LWIN];
#endif
}

#ifdef WIN32
// for taking control over windows key on keyboard if app has focus
LRESULT CALLBACK lowLevelKeyboardProc(int32_t nCode, WPARAM wParam, LPARAM lParam)
{
    uint8_t bEatKeystroke;

    KBDLLHOOKSTRUCT *p;

    SDL_Event inputEvent;

    if ((nCode < 0) || (nCode != HC_ACTION)) // do not process message
        return (CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam));

    bEatKeystroke = false;
    p = (KBDLLHOOKSTRUCT *)(lParam);

    memset(&inputEvent, 0, sizeof (inputEvent));

    switch (wParam)
    {
        case WM_KEYUP:
        case WM_KEYDOWN:
        {
            bEatKeystroke = (SDL_GetAppState() & SDL_APPINPUTFOCUS) && ((p->vkCode == VK_LWIN) || (p->vkCode == VK_RWIN));

            if (bEatKeystroke)
            {
                if (wParam == WM_KEYDOWN)
                {
                    if (!windowsKeyIsDown)
                    {
                        windowsKeyIsDown = true;

                        input.keyb.leftAmigaKeyDown = true;

                        inputEvent.type                = SDL_KEYDOWN;
                        inputEvent.key.type            = SDL_KEYDOWN;
                        inputEvent.key.state           = 1;
                        inputEvent.key.keysym.scancode = (p->vkCode == VK_LWIN) ? 91 : 92;
                        inputEvent.key.keysym.sym      = (p->vkCode == VK_LWIN) ? SDLK_LWIN : SDLK_RWIN;

                        SDL_PushEvent(&inputEvent);
                    }
                }
                else if (wParam == WM_KEYUP)
                {
                    windowsKeyIsDown = false;

                    input.keyb.leftAmigaKeyDown = false;

                    inputEvent.type                = SDL_KEYUP;
                    inputEvent.key.type            = SDL_KEYUP;
                    inputEvent.key.keysym.scancode = (p->vkCode == VK_LWIN) ? 91 : 92;
                    inputEvent.key.keysym.sym      = (p->vkCode == VK_LWIN) ? SDLK_LWIN : SDLK_RWIN;

                    SDL_PushEvent(&inputEvent);
                }
            }
            break;
        }

        default: break;
    }

    return ((bEatKeystroke) ? true : (CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam)));
}
#endif

int32_t checkGUIButtons(void)
{
    uint32_t i;

    // these two makes *no other* buttons clickable
    if (editor.ui.askScreenShown)
    {
        for (i = 0; i < ASK_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bAsk[i].x1) && (input.mouse.x <= bAsk[i].x2)
             && (input.mouse.y >= bAsk[i].y1) && (input.mouse.y <= bAsk[i].y2))
                 return (bAsk[i].b);
        }

        return (-1);
    }
    else if (editor.ui.clearScreenShown)
    {
        for (i = 0; i < CLEAR_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bClear[i].x1) && (input.mouse.x <= bClear[i].x2)
             && (input.mouse.y >= bClear[i].y1) && (input.mouse.y <= bClear[i].y2))
                 return (bClear[i].b);
        }

        return (-1);
    }

    // QUIT (xy 0,0) works on all screens except for ask/clear screen
    if ((input.mouse.x == 0) && (input.mouse.y == 0))
        return (PTB_QUIT);


    // top screen buttons
    if (editor.ui.diskOpScreenShown)
    {
        for (i = 0; i < DISKOP_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bDiskOp[i].x1) && (input.mouse.x <= bDiskOp[i].x2)
                && (input.mouse.y >= bDiskOp[i].y1) && (input.mouse.y <= bDiskOp[i].y2))
                return (bDiskOp[i].b);
        }
    }
    else
    {
        if (editor.ui.posEdScreenShown)
        {
            for (i = 0; i < POSED_BUTTONS; ++i)
            {
                if ((input.mouse.x >= bPosEd[i].x1) && (input.mouse.x <= bPosEd[i].x2)
                    && (input.mouse.y >= bPosEd[i].y1) && (input.mouse.y <= bPosEd[i].y2))
                        return (bPosEd[i].b);
            }
        }
        else if (editor.ui.editOpScreenShown)
        {
            switch (editor.ui.editOpScreen)
            {
                default:
                case 0:
                {
                    for (i = 0; i < EDITOP1_BUTTONS; ++i)
                    {
                        if ((input.mouse.x >= bEditOp1[i].x1) && (input.mouse.x <= bEditOp1[i].x2)
                         && (input.mouse.y >= bEditOp1[i].y1) && (input.mouse.y <= bEditOp1[i].y2))
                            return (bEditOp1[i].b);
                    }
                }
                break;

                case 1:
                {
                    for (i = 0; i < EDITOP2_BUTTONS; ++i)
                    {
                        if ((input.mouse.x >= bEditOp2[i].x1) && (input.mouse.x <= bEditOp2[i].x2)
                         && (input.mouse.y >= bEditOp2[i].y1) && (input.mouse.y <= bEditOp2[i].y2))
                            return (bEditOp2[i].b);
                    }
                }
                break;

                case 2:
                {
                    for (i = 0; i < EDITOP3_BUTTONS; ++i)
                    {
                        if ((input.mouse.x >= bEditOp3[i].x1) && (input.mouse.x <= bEditOp3[i].x2)
                         && (input.mouse.y >= bEditOp3[i].y1) && (input.mouse.y <= bEditOp3[i].y2))
                            return (bEditOp3[i].b);
                    }
                }
                break;

                case 3:
                {
                    for (i = 0; i < EDITOP4_BUTTONS; ++i)
                    {
                        if ((input.mouse.x >= bEditOp4[i].x1) && (input.mouse.x <= bEditOp4[i].x2)
                         && (input.mouse.y >= bEditOp4[i].y1) && (input.mouse.y <= bEditOp4[i].y2))
                            return (bEditOp4[i].b);
                    }
                }
                break;
            }
        }

        for (i = 0; i < TOPSCREEN_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bTopScreen[i].x1) && (input.mouse.x <= bTopScreen[i].x2)
                && (input.mouse.y >= bTopScreen[i].y1) && (input.mouse.y <= bTopScreen[i].y2))
                    return (bTopScreen[i].b);
        }
    }

    // middle buttons (always present)
    for (i = 0; i < MIDSCREEN_BUTTONS; ++i)
    {
        if ((input.mouse.x >= bMidScreen[i].x1) && (input.mouse.x <= bMidScreen[i].x2)
            && (input.mouse.y >= bMidScreen[i].y1) && (input.mouse.y <= bMidScreen[i].y2))
            return (bMidScreen[i].b);
    }

    // bottom screen buttons
    if (editor.ui.samplerScreenShown)
    {
        for (i = 0; i < SAMPLER_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bSampler[i].x1) && (input.mouse.x <= bSampler[i].x2)
                && (input.mouse.y >= bSampler[i].y1) && (input.mouse.y <= bSampler[i].y2))
                return (bSampler[i].b);
        }
    }
    else
    {
        for (i = 0; i < BOTSCREEN_BUTTONS; ++i)
        {
            if ((input.mouse.x >= bBotScreen[i].x1) && (input.mouse.x <= bBotScreen[i].x2)
                && (input.mouse.y >= bBotScreen[i].y1) && (input.mouse.y <= bBotScreen[i].y2))
                return (bBotScreen[i].b);
        }
    }

    return (-1);
}

void posMoveLeft(void)
{
    if (editor.ui.dstPos > 0)
    {
        editor.ui.dstPos   -= 1;
        editor.ui.lineCurX -= 8;
    }
    else
    {
        if (editor.ui.dstOffset != NULL)
            (*editor.ui.dstOffset)--;
    }
}

void posMoveRight(void)
{
    if (editor.ui.getLineType == TEXT_EDIT_STRING)
    {
        if (editor.ui.dstPos < (editor.ui.textLength - 1))
        {
            editor.ui.dstPos++;
            editor.ui.lineCurX += 8;
        }
        else
        {
            if (editor.ui.dstOffset != NULL)
                (*editor.ui.dstOffset)++;
        }
    }
    else
    {
        editor.ui.dstPos++;
        editor.ui.lineCurX += 8;
    }
}

void moveCharLeft(void)
{
    if (editor.MixFlag && (editor.ui.dstPos <= 4))
        return;

    if (editor.ui.editPos > editor.ui.showTextPtr)
    {
        editor.ui.editPos--;
        posMoveLeft();

        if (editor.MixFlag)
        {
            if (editor.ui.dstPos == 12)
            {
                editor.ui.editPos--;
                posMoveLeft();
                editor.ui.editPos--;
                posMoveLeft();
                editor.ui.editPos--;
                posMoveLeft();
                editor.ui.editPos--;
                posMoveLeft();
            }
            else if (editor.ui.dstPos == 6)
            {
                editor.ui.editPos--;
                posMoveLeft();
            }
        }
    }

    editor.ui.dstOffsetEnd = false;
}

void moveCharRight(void)
{
    if (editor.MixFlag && (editor.ui.dstPos >= 14))
        return;

    if (editor.ui.editPos < editor.ui.textEndPtr)
    {
        if (*editor.ui.editPos != '\0')
        {
            editor.ui.editPos++;
            posMoveRight();

            if (editor.MixFlag)
            {
                if (editor.ui.dstPos == 9)
                {
                    editor.ui.editPos++;
                    posMoveRight();
                    editor.ui.editPos++;
                    posMoveRight();
                    editor.ui.editPos++;
                    posMoveRight();
                    editor.ui.editPos++;
                    posMoveRight();
                }
                else if (editor.ui.dstPos == 6)
                {
                    editor.ui.editPos++;
                    posMoveRight();
                }
            }
        }
        else
        {
            editor.ui.dstOffsetEnd = true;
        }
    }
    else
    {
        editor.ui.dstOffsetEnd = true;
    }
}

void keyUpHandler(SDLKey keyEntry)
{
    switch (keyEntry)
    {
        // modifiers shouldn't reset keyb repeat/delay flags & counters
        case SDLK_LCTRL:
        case SDLK_RCTRL:
        case SDLK_LSHIFT:
        case SDLK_RSHIFT:
        case SDLK_LALT:
        case SDLK_RALT:
        case SDLK_LWIN:
        case SDLK_RWIN:
        case SDLK_MENU:
        case SDLK_MODE:
        break;

        default:
        {
            input.keyb.repeatKey     = false;
            input.keyb.delayKey      = false;
            input.keyb.repeatCounter = 0;
            input.keyb.delayCounter  = 0;
        }
        break;
    }
}

void keyDownHandler(SDLKey keyEntry, MODULE *modEntry)
{
    int8_t chTmp;
    uint8_t blockFrom;
    uint8_t blockTo;

    int16_t i;
    int16_t j;

    modnote_t *noteSrc;
    modnote_t *noteDst;
    modnote_t noteTmp;

    MODULE_SAMPLE *s;

    // TOGGLE FULLSCREEN (should always react)
    if (keyEntry == SDLK_F11)
    {
        toggleFullscreen();
        return;
    }

    if ((input.mouse.buttonErrorMsgActive && input.mouse.buttonErrorMsgBlock) || (keyEntry == SDLK_UNKNOWN))
        return;

    // if no ALT/SHIFT/CTRL/AMIGA, update last key for repeat routine
    if ((keyEntry < 303) || (keyEntry > 311))
    {
        // if Repeat Flag, repeat all keys
        if (editor.NoteRepFlag
             && (keyEntry < SDLK_F1)
             && (keyEntry != SDLK_ESCAPE)
             && (keyEntry != SDLK_CAPSLOCK)
           )
        {
            if (!input.keyb.repeatKey)
                input.keyb.delayCounter = 0;

            input.keyb.repeatKey = true;
            input.keyb.delayKey  = true;
        }

        input.keyb.repeatCounter = 0;
        input.keyb.lastKey       = keyEntry;
    }

    // Macbook/powerbook keyboard hack for entering '/'
#ifdef __APPLE__
    if (keyEntry == SDLK_LESS)
        keyEntry = SDLK_SLASH;
#endif

    // ENTRY JUMPING IN DISK OP. FILELIST
    if (!editor.ui.getLineFlag
        && input.keyb.shiftKeyDown
        && editor.ui.diskOpScreenShown
        && ((keyEntry >= 32) && (keyEntry <= 126)))
    {
        handleEntryJumping(keyEntry);
        return;
    }

    if (!handleGeneralModes(keyEntry, modEntry)) return;
    if (!handleTextEditMode(keyEntry, modEntry)) return;

    // GENERAL KEYS
    switch (keyEntry)
    {
        case SDLK_BACKSLASH: editor.pnoteflag = (editor.pnoteflag + 1) % 3; break;

        case SDLK_RSHIFT:
        {
            if (!editor.ui.samplerScreenShown && !editor.ui.askScreenShown)
            {
                editor.playMode = PLAY_MODE_PATTERN;
                modPlay(modEntry->currPattern, DONT_SET_ORDER, DONT_SET_ROW);

                editor.currentMode = MODE_RECORD;
                pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
        }
        break;

        case SDLK_MODE: // this fall-through is needed for GNU/Linux, BSD or Unix...
        case SDLK_RALT:
        {
            if (!editor.ui.askScreenShown)
            {
                editor.playMode = PLAY_MODE_NORMAL;
                modPlay(DONT_SET_PATTERN, modEntry->currOrder, DONT_SET_ROW);

                editor.currentMode = MODE_PLAY;
                pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
        }
        break;

        case SDLK_ESCAPE:
        {
            if (editor.ui.posEdScreenShown)
            {
                editor.ui.posEdScreenShown = false;
            }
            else if (editor.ui.diskOpScreenShown)
            {
                editor.ui.diskOpScreenShown = false;
            }
            else if (editor.ui.samplerScreenShown)
            {
                editor.ui.samplerScreenShown = false;
            }
            else if (editor.ui.editOpScreenShown)
            {
                editor.ui.editOpScreenShown = false;
            }
            else
            {
                editor.ui.askScreenShown = true;
                editor.ui.askScreenType  = ASK_QUIT;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("REALLY QUIT ?", NO_CARRY);

                return;
            }

            pointerSetPreviousMode();
            setPrevStatusMessage();
        }
        break;

        case SDLK_INSERT:
        {
            if (editor.ui.samplerScreenShown)
            {
                samplerSamPaste(modEntry);
                return;
            }
        }
        break;

        case SDLK_PAGEUP:
        {
            if (editor.ui.posEdScreenShown)
            {
                if (modEntry->currOrder > 0)
                {
                    if ((modEntry->currOrder - (POSED_LIST_SIZE - 1)) > 0)
                        modSetPos(modEntry->currOrder - (POSED_LIST_SIZE - 1), DONT_SET_ROW);
                    else
                        modSetPos(0, DONT_SET_ROW);
                }
            }
            else if (editor.ui.diskOpScreenShown)
            {
                editor.diskop.scrollOffset -= (DISKOP_LIST_SIZE - 1);
                if (editor.diskop.scrollOffset < 0)
                    editor.diskop.scrollOffset = 0;
            }
            else
            {
                if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                {
                         if (modEntry->currRow == 63) modSetPos(DONT_SET_ORDER, modEntry->currRow - 15);
                    else if (modEntry->currRow == 15) modSetPos(DONT_SET_ORDER, 0); // 15-16 would turn into -1, which is "DON'T SET ROW" flag
                    else                              modSetPos(DONT_SET_ORDER, modEntry->currRow - 16);
                }
            }

            if (!input.keyb.repeatKey)
                input.keyb.delayCounter = 0;

            input.keyb.repeatKey = true;
            input.keyb.delayKey  = true;
        }
        break;

        case SDLK_PAGEDOWN:
        {
            if (editor.ui.posEdScreenShown)
            {
                if (modEntry->currOrder != (modEntry->head.orderCount - 1))
                {
                    if ((modEntry->currOrder + (POSED_LIST_SIZE - 1)) <= (modEntry->head.orderCount - 1))
                        modSetPos(modEntry->currOrder + (POSED_LIST_SIZE - 1), DONT_SET_ROW);
                    else
                        modSetPos(modEntry->head.orderCount - 1, DONT_SET_ROW);
                }
            }
            else if (editor.ui.diskOpScreenShown)
            {
                if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                {
                    editor.diskop.scrollOffset += (DISKOP_LIST_SIZE - 1);
                    if (editor.diskop.scrollOffset > (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                        editor.diskop.scrollOffset =  editor.diskop.numFiles - DISKOP_LIST_SIZE;
                }
            }
            else
            {
                if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                    modSetPos(DONT_SET_ORDER, modEntry->currRow + 16);
            }

            if (!input.keyb.repeatKey)
                input.keyb.delayCounter = 0;

            input.keyb.repeatKey = true;
            input.keyb.delayKey  = true;
        }
        break;

        case SDLK_HOME:
        {
            if (editor.ui.posEdScreenShown)
            {
                if (modEntry->currOrder > 0)
                    modSetPos(0, DONT_SET_ROW);
            }
            else if (editor.ui.diskOpScreenShown)
            {
                editor.diskop.scrollOffset = 0;
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                    moveSampleMarkerLeft(input.keyb.shiftKeyDown, true);
                else if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                    modSetPos(DONT_SET_ORDER, 0);
            }
        }
        break;

        case SDLK_END:
        {
            if (editor.ui.posEdScreenShown)
            {
                modSetPos(modEntry->head.orderCount - 1, DONT_SET_ROW);
            }
            else if (editor.ui.diskOpScreenShown)
            {
                if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                    editor.diskop.scrollOffset = editor.diskop.numFiles - DISKOP_LIST_SIZE;
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                    moveSampleMarkerRight(input.keyb.shiftKeyDown, true);
                else if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                    modSetPos(DONT_SET_ORDER, 63);
            }
        }
        break;

        case SDLK_DELETE:
        {
            if (editor.ui.samplerScreenShown)
                samplerSamDelete(modEntry, SAMPLE_CUT);
            else
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
        }
        break;

        case SDLK_F12:
        {
            if (input.keyb.controlKeyDown)
            {
                editor.timingMode ^= 1;
                if (editor.timingMode == TEMPO_MODE_VBLANK)
                {
                    editor.oldBPM = modEntry->currBPM;
                    modSetTempo(125);
                }
                else
                {
                    modSetTempo(editor.oldBPM);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                toggleAmigaPanMode();
            }
            else
            {
                toggleLowPassFilter();
            }
        }
        break;

        case SDLK_RETURN:
        {
            if (editor.ui.askScreenShown)
            {
                editor.ui.answerNo       = false;
                editor.ui.answerYes      = true;
                editor.ui.askScreenShown = false;

                handleAskYes(modEntry);
            }
            else
            {
                if (input.keyb.shiftKeyDown || input.keyb.altKeyDown || input.keyb.controlKeyDown)
                {
                    SaveUndo(modEntry);

                    if (input.keyb.altKeyDown && !input.keyb.controlKeyDown)
                    {
                        if (modEntry->currRow < 63)
                        {
                            for (i = 0; i < PAULA_CHANNELS; ++i)
                            {
                                for (j = 62; j >= modEntry->currRow; --j)
                                {
                                    noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];
                                    modEntry->patterns[modEntry->currPattern][((j + 1) * PAULA_CHANNELS) + i] = *noteSrc;
                                }

                                j = ((j + 1) * PAULA_CHANNELS) + i;

                                modEntry->patterns[modEntry->currPattern][j].period  = 0;
                                modEntry->patterns[modEntry->currPattern][j].sample  = 0;
                                modEntry->patterns[modEntry->currPattern][j].command = 0;
                                modEntry->patterns[modEntry->currPattern][j].param   = 0;
                            }

                            modEntry->currRow++;

                            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        }
                    }
                    else
                    {
                        if (modEntry->currRow < 63)
                        {
                            for (i = 62; i >= modEntry->currRow; --i)
                            {
                                noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                                if (input.keyb.controlKeyDown)
                                {
                                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].command = noteSrc->command;
                                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].param   = noteSrc->param;
                                }
                                else
                                {
                                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc;
                                }
                            }

                            i = ((i + 1) * PAULA_CHANNELS) + editor.cursor.channel;

                            if (!input.keyb.controlKeyDown)
                            {
                                modEntry->patterns[modEntry->currPattern][i].period = 0;
                                modEntry->patterns[modEntry->currPattern][i].sample = 0;
                            }

                            modEntry->patterns[modEntry->currPattern][i].command = 0;
                            modEntry->patterns[modEntry->currPattern][i].param   = 0;

                            modEntry->currRow++;

                            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        }
                    }
                }
                else
                {
                    editor.stepPlayEnabled   = true;
                    editor.stepPlayBackwards = false;

                    doStopIt();
                    playPattern(modEntry->currRow);
                }
            }
        }
        break;

        // toggle between IDLE and EDIT (IDLE if PLAY)
        case SDLK_SPACE:
        {
            if (editor.currentMode == MODE_PLAY)
            {
                modStop();

                editor.currentMode = MODE_IDLE;
                pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
            else if ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD))
            {
                if (!editor.ui.samplerScreenShown)
                {
                    modStop();

                    editor.currentMode = MODE_IDLE;
                    pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
            }
            else if (!editor.ui.samplerScreenShown)
            {
                modStop();

                editor.currentMode = MODE_EDIT;
                pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
        }
        break;

        case SDLK_F1: editor.keyOctave = OCTAVE_LOW;  break;
        case SDLK_F2: editor.keyOctave = OCTAVE_HIGH; break;

        case SDLK_F3:
        {
            if (editor.ui.samplerScreenShown)
            {
                samplerSamDelete(modEntry, SAMPLE_CUT);
            }
            else
            {
                if (input.keyb.shiftKeyDown)
                {
                    // cut channel and put in buffer
                    SaveUndo(modEntry);

                    noteDst = editor.TrackBuffer;
                    for (i = 0; i < MOD_ROWS; ++i)
                    {
                        *noteDst++ = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;
                    }

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
                else if (input.keyb.altKeyDown)
                {
                    // cut pattern and put in buffer
                    SaveUndo(modEntry);

                    memcpy(editor.PatternBuffer, modEntry->patterns[modEntry->currPattern],
                        sizeof (modnote_t) * (PAULA_CHANNELS * MOD_ROWS));

                    memset(modEntry->patterns[modEntry->currPattern], 0,
                        sizeof (modnote_t) * (PAULA_CHANNELS * MOD_ROWS));

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
                else if (input.keyb.controlKeyDown)
                {
                    // cut channel commands and put in buffer
                    SaveUndo(modEntry);

                    noteDst = editor.CmdsBuffer;
                    for (i = 0; i < MOD_ROWS; ++i)
                    {
                        *noteDst++ = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;
                    }

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
        }
        break;

        case SDLK_F4:
        {
            if (editor.ui.samplerScreenShown)
            {
                samplerSamCopy(modEntry);
            }
            else
            {
                if (input.keyb.shiftKeyDown)
                {
                    // copy channel to buffer

                    noteDst = editor.TrackBuffer;
                    for (i = 0; i < MOD_ROWS; ++i)
                        *noteDst++ = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                }
                else if (input.keyb.altKeyDown)
                {
                    // copy pattern to buffer

                    memcpy(editor.PatternBuffer, modEntry->patterns[modEntry->currPattern],
                        sizeof (modnote_t) * (PAULA_CHANNELS * MOD_ROWS));
                }
                else if (input.keyb.controlKeyDown)
                {
                    // copy channel commands to buffer

                    noteDst = editor.CmdsBuffer;
                    for (i = 0; i < MOD_ROWS; ++i)
                    {
                        noteDst->command = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command;
                        noteDst->param   = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param;

                        noteDst++;
                    }
                }
            }
        }
        break;

        case SDLK_F5:
        {
            if (editor.ui.samplerScreenShown)
            {
                samplerSamPaste(modEntry);
            }
            else
            {
                if (input.keyb.shiftKeyDown)
                {
                    // paste channel buffer to channel
                    SaveUndo(modEntry);

                    noteSrc = editor.TrackBuffer;

                    for (i = 0; i < MOD_ROWS; ++i)
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc++;

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
                else if (input.keyb.altKeyDown)
                {
                    // paste pattern buffer to pattern
                    SaveUndo(modEntry);

                    memcpy(modEntry->patterns[modEntry->currPattern],
                        editor.PatternBuffer, sizeof (modnote_t) * (PAULA_CHANNELS * MOD_ROWS));

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
                else if (input.keyb.controlKeyDown)
                {
                    // paste channel commands buffer to channel
                    SaveUndo(modEntry);

                    noteSrc = editor.CmdsBuffer;

                    for (i = 0; i < MOD_ROWS; ++i)
                    {
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = noteSrc->command;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = noteSrc->param;

                        noteSrc++;
                    }

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
        }
        break;

        case SDLK_F6:
        {
            if (input.keyb.shiftKeyDown)
            {
                editor.F6pos = modEntry->currRow;

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.errorMsgCounter      = 32; // short flash

                setStatusMessage("POSITION SET", NO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    editor.playMode = PLAY_MODE_PATTERN;
                    modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F6pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if (input.keyb.controlKeyDown)
                {
                    if (!editor.ui.samplerScreenShown)
                    {
                        editor.playMode = PLAY_MODE_PATTERN;
                        modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F6pos);

                        editor.currentMode = MODE_RECORD;
                        pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                }
                else if (input.keyb.leftAmigaKeyDown)
                {
                    editor.playMode = PLAY_MODE_NORMAL;
                    modPlay(DONT_SET_PATTERN, modEntry->currOrder, editor.F6pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                {
                    modSetPos(DONT_SET_ORDER, editor.F6pos);
                }
            }
        }
        break;

        case SDLK_F7:
        {
            if (input.keyb.shiftKeyDown)
            {
                editor.F7pos = modEntry->currRow;

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.errorMsgCounter      = 32; // short flash

                setStatusMessage("POSITION SET", NO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    editor.playMode = PLAY_MODE_PATTERN;
                    modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F7pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if (input.keyb.controlKeyDown)
                {
                    if (!editor.ui.samplerScreenShown)
                    {
                        editor.playMode = PLAY_MODE_PATTERN;
                        modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F7pos);

                        editor.currentMode = MODE_RECORD;
                        pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                }
                else if (input.keyb.leftAmigaKeyDown)
                {
                    editor.playMode = PLAY_MODE_NORMAL;
                    modPlay(DONT_SET_PATTERN, modEntry->currOrder, editor.F7pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                {
                    modSetPos(DONT_SET_ORDER, editor.F7pos);
                }
            }
        }
        break;

        case SDLK_F8:
        {
            if (input.keyb.shiftKeyDown)
            {
                editor.F8pos = modEntry->currRow;

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.errorMsgCounter      = 32; // short flash

                setStatusMessage("POSITION SET", NO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    editor.playMode = PLAY_MODE_PATTERN;
                    modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F8pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if (input.keyb.controlKeyDown)
                {
                    if (!editor.ui.samplerScreenShown)
                    {
                        editor.playMode = PLAY_MODE_PATTERN;
                        modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F8pos);

                        editor.currentMode = MODE_RECORD;
                        pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                }
                else if (input.keyb.leftAmigaKeyDown)
                {
                    editor.playMode = PLAY_MODE_NORMAL;
                    modPlay(DONT_SET_PATTERN, modEntry->currOrder, editor.F8pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                {
                    modSetPos(DONT_SET_ORDER, editor.F8pos);
                }
            }
        }
        break;

        case SDLK_F9:
        {
            if (input.keyb.shiftKeyDown)
            {
                editor.F9pos = modEntry->currRow;

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.errorMsgCounter      = 32; // short flash

                setStatusMessage("POSITION SET", NO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    editor.playMode = PLAY_MODE_PATTERN;
                    modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F9pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if (input.keyb.controlKeyDown)
                {
                    if (!editor.ui.samplerScreenShown)
                    {
                        editor.playMode = PLAY_MODE_PATTERN;
                        modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F9pos);

                        editor.currentMode = MODE_RECORD;
                        pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                }
                else if (input.keyb.leftAmigaKeyDown)
                {
                    editor.playMode = PLAY_MODE_NORMAL;
                    modPlay(DONT_SET_PATTERN, modEntry->currOrder, editor.F9pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                {
                    modSetPos(DONT_SET_ORDER, editor.F9pos);
                }
            }
        }
        break;

        case SDLK_F10:
        {
            if (input.keyb.shiftKeyDown)
            {
                editor.F10pos = modEntry->currRow;

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.errorMsgCounter      = 32; // short flash

                setStatusMessage("POSITION SET", NO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    editor.playMode = PLAY_MODE_PATTERN;
                    modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F10pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if (input.keyb.controlKeyDown)
                {
                    if (!editor.ui.samplerScreenShown)
                    {
                        editor.playMode = PLAY_MODE_PATTERN;
                        modPlay(modEntry->currPattern, DONT_SET_ORDER, editor.F10pos);

                        editor.currentMode = MODE_RECORD;
                        pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                }
                else if (input.keyb.leftAmigaKeyDown)
                {
                    editor.playMode = PLAY_MODE_NORMAL;
                    modPlay(DONT_SET_PATTERN, modEntry->currOrder, editor.F10pos);

                    editor.currentMode = MODE_PLAY;
                    pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }
                else if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                {
                    modSetPos(DONT_SET_ORDER, editor.F10pos);
                }
            }
        }
        break;

        case SDLK_RWIN:
        case SDLK_MENU:
        {
            if (!editor.ui.askScreenShown)
            {
                editor.playMode = PLAY_MODE_PATTERN;
                modPlay(modEntry->currPattern, DONT_SET_ORDER, DONT_SET_ROW);

                editor.currentMode = MODE_PLAY;
                pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
        }
        break;

        case SDLK_TAB:
        {
            if (input.keyb.shiftKeyDown)
                movePatCurPrevCh();
            else
                movePatCurNextCh();
        }
        break;

        case SDLK_0:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 0;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 0", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[9] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_1:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackNoteUp(false, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 1;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 1", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[0] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_2:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattNoteUp(false, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 2;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 2", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[1] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_3:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackNoteUp(true, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 3;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 3", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[2] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_4:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattNoteUp(true, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 4;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 4", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[3] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_5:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 5;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 5", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[4] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_6:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 6;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 6", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[5] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_7:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 7;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 7", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[6] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_8:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 8;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 8", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[7] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_9:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.ui.clearScreenShown && !editor.ui.askScreenShown)
                {
                    editor.EditMoveAdd = 9;

                    input.mouse.buttonErrorMsgActive = true;
                    input.mouse.errorMsgCounter      = 32; // short flash

                    setStatusMessage("EDITSKIP = 9", NO_CARRY);
                }
            }
            else if (input.keyb.shiftKeyDown)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
                editor.EffectMacros[8] = (noteSrc->command << 8) | noteSrc->param;

                displayMsg("COMMAND STORED!");
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_KP0:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 0;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP1:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 12;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP2:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 13;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP3:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 14;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP4:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 8;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP5:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 9;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP6:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 10;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP7:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 4;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP8:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 5;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP9:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 6;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_ENTER:
        {
            editor.SampleZero = false;

            editor.currentSample++;

            if (editor.currentSample >= 0x10)
            {
                editor.keypadSampleOffset = 0x00;

                editor.currentSample -= 0x10;
                if (editor.currentSample < 0x01)
                    editor.currentSample = 0x01;
            }
            else
            {
                editor.currentSample     += 0x10;
                editor.keypadSampleOffset = 0x10;
            }

            editor.currentSample--;

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_PLUS:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 11;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_MINUS:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 7;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_MULTIPLY:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 3;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_DIVIDE:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 2;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_NUMLOCK:
        {
            editor.SampleZero = false;
            editor.currentSample = editor.keypadSampleOffset + 1;
            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_KP_PERIOD:
        {
            editor.SampleZero = false;

            editor.currentSample = editor.keypadSampleOffset - 1;
            if (editor.currentSample < 0)
                editor.currentSample = 0;

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING); // sample data is not redrawn if the sampler screen is hidden

            if (input.keyb.altKeyDown && (editor.pnoteflag > 0))
            {
                editor.ui.changingDrumPadNote = true;

                setStatusMessage("SELECT NOTE", NO_CARRY);
                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                break;
            }

            if (editor.pnoteflag > 0)
                handleEditKeys(keyEntry, modEntry, EDIT_SPECIAL);
        }
        break;

        case SDLK_DOWN:
        {
            input.keyb.delayKey  = false;
            input.keyb.repeatKey = false;

            if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
            {
                if (editor.ui.diskOpScreenShown)
                {
                    if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                    {
                        if (editor.diskop.scrollOffset < (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                            editor.diskop.scrollOffset++;
                    }

                    if (!input.keyb.repeatKey)
                        input.keyb.delayCounter = 0;

                    input.keyb.repeatKey = true;
                    input.keyb.delayKey  = false;
                }
                else if (editor.ui.posEdScreenShown)
                {
                    if (modEntry->currOrder != (modEntry->head.orderCount - 1))
                    {
                        if (++modEntry->currOrder > (modEntry->head.orderCount - 1))
                              modEntry->currOrder =  modEntry->head.orderCount - 1;

                        modSetPos(modEntry->currOrder, DONT_SET_ROW);
                    }

                    if (!input.keyb.repeatKey)
                        input.keyb.delayCounter = 0;

                    input.keyb.repeatKey = true;
                    input.keyb.delayKey  = false;
                }
                else if (!editor.ui.samplerScreenShown)
                {
                    if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                        modSetPos(DONT_SET_ORDER, (modEntry->currRow + 1) & 0x3F);

                    input.keyb.repeatKey = true;
                }
            }
        }
        break;

        case SDLK_UP:
        {
            input.keyb.delayKey  = false;
            input.keyb.repeatKey = false;

            if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
            {
                if (editor.ui.diskOpScreenShown)
                {
                    if (editor.diskop.scrollOffset > 0)
                        editor.diskop.scrollOffset--;

                    if (!input.keyb.repeatKey)
                        input.keyb.delayCounter = 0;

                    input.keyb.repeatKey = true;
                    input.keyb.delayKey  = false;
                }
                else if (editor.ui.posEdScreenShown)
                {
                    if (modEntry->currOrder > 0)
                        modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);

                    if (!input.keyb.repeatKey)
                        input.keyb.delayCounter = 0;

                    input.keyb.repeatKey = true;
                    input.keyb.delayKey  = false;
                }
                else if (!editor.ui.samplerScreenShown)
                {
                    if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                        modSetPos(DONT_SET_ORDER, (modEntry->currRow - 1) & 0x3F);

                    input.keyb.repeatKey = true;
                }
            }
        }
        break;

        case SDLK_LEFT:
        {
            input.keyb.delayKey  = false;
            input.keyb.repeatKey = false;

            if (input.keyb.controlKeyDown)
            {
                sampleDownButton(modEntry);
            }
            else if (input.keyb.shiftKeyDown)
            {
                if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
                {
                    if (editor.ui.samplerScreenShown)
                    {
                        moveSampleMarkerLeft(true, false);

                        input.keyb.delayKey  = true;
                        input.keyb.repeatKey = true;
                    }
                    else if (modEntry->currOrder > 0)
                    {
                        modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);

                        input.keyb.repeatKey = true;
                    }
                }
            }
            else if (input.keyb.altKeyDown)
            {
                if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
                    decPatt();
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                {
                    moveSampleMarkerLeft(false, false);

                    input.keyb.delayKey  = true;
                    input.keyb.repeatKey = true;
                }
                else
                {
                    movePatCurLeft();

                    input.keyb.repeatKey = true;
                }
            }
        }
        break;

        case SDLK_RIGHT:
        {
            input.keyb.delayKey  = false;
            input.keyb.repeatKey = false;

            if (input.keyb.controlKeyDown)
            {
                sampleUpButton(modEntry);
            }
            else if (input.keyb.shiftKeyDown)
            {
                if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
                {
                    if (editor.ui.samplerScreenShown)
                    {
                        moveSampleMarkerRight(true, false);

                        input.keyb.delayKey  = true;
                        input.keyb.repeatKey = true;
                    }
                    else if (modEntry->currOrder < 126)
                    {
                        modSetPos(modEntry->currOrder + 1, DONT_SET_ROW);
                    }
                }
            }
            else if (input.keyb.altKeyDown)
            {
                if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
                    incPatt();
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                {
                    moveSampleMarkerRight(false, false);

                    input.keyb.delayKey  = true;
                    input.keyb.repeatKey = true;
                }
                else
                {
                    movePatCurRight();

                    input.keyb.repeatKey = true;
                }
            }
        }
        break;

        case SDLK_a:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackOctaUp(TRANSPOSE_NORMAL, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (editor.ui.samplerScreenShown)
                {
                    samplerRangeAll();
                }
                else
                {
                    if (input.keyb.shiftKeyDown)
                    {
                        editor.muted[0] = true;
                        editor.muted[1] = true;
                        editor.muted[2] = true;
                        editor.muted[3] = true;

                        editor.muted[editor.cursor.channel] = false;

                        break;
                    }

                    editor.muted[editor.cursor.channel] ^= 1;
                }
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_b:
        {
            if (input.keyb.controlKeyDown)
            {
                // CTRL+B doesn't change the status message back, so do this:
                if (editor.ui.introScreenShown)
                {
                    editor.ui.introScreenShown = false;
                    setStatusMessage("ALL RIGHT", DO_CARRY);
                }

                if (editor.BlockMarkFlag)
                {
                    editor.BlockMarkFlag = false;
                    return;
                }

                editor.BlockMarkFlag = true;
                editor.BlockFromPos  = modEntry->currRow;
                editor.BlockToPos    = modEntry->currRow;
            }
            else if (input.keyb.altKeyDown)
            {
                s = &modEntry->samples[editor.currentSample];

                if (s->length == 0)
                {
                    displayErrorMsg("SAMPLE IS EMPTY");
                    break;
                }

                BoostSample(modEntry, editor.currentSample);
                setupSampler(modEntry, KEEP_SAMPLE_MARKING);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_c:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackOctaDown(TRANSPOSE_SAMPLE, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (editor.ui.samplerScreenShown)
                {
                    samplerSamCopy(modEntry);
                    return;
                }

                if (!editor.BlockMarkFlag)
                {
                    displayErrorMsg("NO BLOCK MARKED !");
                    return;
                }

                editor.BlockMarkFlag   = false;
                editor.BlockBufferFlag = true;

                for (i = 0; i < MOD_ROWS; ++i)
                    editor.BlockBuffer[i] = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                if (editor.BlockFromPos > editor.BlockToPos)
                {
                    editor.BuffFromPos = editor.BlockToPos;
                    editor.BuffToPos   = editor.BlockFromPos;
                }
                else
                {
                    editor.BuffFromPos = editor.BlockFromPos;
                    editor.BuffToPos   = editor.BlockToPos;
                }

                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
            else
            {
                if (input.keyb.altKeyDown)
                    editor.muted[2] ^= 1; // toggle channel 3
                else
                    handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_d:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackOctaUp(TRANSPOSE_SAMPLE, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                SaveUndo(modEntry);
            }
            else
            {
                if (input.keyb.altKeyDown)
                {
                    if (!editor.ui.posEdScreenShown)
                    {
                        editor.BlockMarkFlag = false;

                        editor.ui.diskOpScreenShown ^= 1;
                        if (!editor.ui.diskOpScreenShown)
                        {
                            pointerSetPreviousMode();
                            setPrevStatusMessage();
                        }
                    }
                }
                else
                {
                    handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
                }
            }
        }
        break;

        case SDLK_e:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackNoteDown(TRANSPOSE_SAMPLE, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.altKeyDown)
            {
                if (editor.ui.editOpScreenShown)
                    editor.ui.editOpScreen = (editor.ui.editOpScreen + 1) % 3;
                else
                    editor.ui.editOpScreenShown = true;
            }
            else if (input.keyb.controlKeyDown)
            {
                SaveUndo(modEntry);

                j = modEntry->currRow + 1;

                while (j < MOD_ROWS)
                {
                    for (i = 62; i >= j; i--)
                    {
                        noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                        modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc;
                    }

                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                    modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;

                   j += 2;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_f:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattOctaUp(TRANSPOSE_SAMPLE, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                toggleLEDFilter();

                if (editor.useLEDFilter)
                    displayMsg("LED FILTER ON");
                else
                    displayMsg("LED FILTER OFF");
            }
            else if (input.keyb.altKeyDown)
            {
                s = &modEntry->samples[editor.currentSample];

                if (s->length == 0)
                {
                    displayErrorMsg("SAMPLE IS EMPTY");
                    break;
                }

                FilterSample(modEntry, editor.currentSample);
                setupSampler(modEntry, KEEP_SAMPLE_MARKING);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_g:
        {
            if (input.keyb.controlKeyDown)
            {
                editor.ui.askScreenShown = true;
                editor.ui.askScreenType  = ASK_BOOST_ALL_SAMPLES;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("BOOST ALL SAMPLES", NO_CARRY);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_h:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockMarkFlag)
                {
                    displayErrorMsg("NO BLOCK MARKED !");
                    return;
                }

                TrackNoteUp(0, editor.BlockFromPos, editor.BlockToPos, modEntry);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_i:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockBufferFlag)
                {
                    displayErrorMsg("BUFFER IS EMPTY !");
                    return;
                }

                if (modEntry->currRow < 63)
                {
                    for (i = 0; i <= (editor.BuffToPos - editor.BuffFromPos); ++i)
                    {
                        for (j = 62; j >= modEntry->currRow; --j)
                        {
                            noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + editor.cursor.channel];
                            modEntry->patterns[modEntry->currPattern][((j + 1) * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc;
                        }
                    }
                }

                SaveUndo(modEntry);

                for (i = 0; i <= (editor.BuffToPos - editor.BuffFromPos); ++i)
                {
                    if ((modEntry->currRow + i) > 63)
                        break;

                    modEntry->patterns[modEntry->currPattern][((modEntry->currRow + i) * PAULA_CHANNELS) + editor.cursor.channel]
                        = editor.BlockBuffer[editor.BuffFromPos + i];
                }

                if (!input.keyb.shiftKeyDown)
                {
                    modEntry->currRow += (i & 0xFF);
                    if (modEntry->currRow > 63)
                        modEntry->currRow = 0;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else if (input.keyb.altKeyDown)
            {
                editor.AutoInsFlag ^= 1;
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_j:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockBufferFlag)
                {
                    displayErrorMsg("BUFFER IS EMPTY !");
                    return;
                }

                SaveUndo(modEntry);

                noteSrc = &editor.BlockBuffer[editor.BuffFromPos];
                for (i = 0; i <= (editor.BuffToPos - editor.BuffFromPos); ++i)
                {
                    if ((modEntry->currRow + i) > 63)
                        break;

                    noteDst = &modEntry->patterns[modEntry->currPattern][((modEntry->currRow + i) * PAULA_CHANNELS) + editor.cursor.channel];

                    if (noteSrc->period || noteSrc->sample)
                    {
                        *noteDst = *noteSrc;
                    }
                    else
                    {
                        noteDst->command = noteSrc->command;
                        noteDst->param   = noteSrc->param;
                    }

                    noteSrc++;
                }

                if (!input.keyb.shiftKeyDown)
                {
                    modEntry->currRow += (i & 0xFF);
                    if (modEntry->currRow > 63)
                        modEntry->currRow = 0;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_k:
        {
            if (input.keyb.altKeyDown)
            {
                for (i = 0; i < MOD_ROWS; ++i)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                    if (noteSrc->sample == (editor.currentSample + 1))
                    {
                        noteSrc->period  = 0;
                        noteSrc->sample  = 0;
                        noteSrc->command = 0;
                        noteSrc->param   = 0;
                    }
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else if (input.keyb.controlKeyDown)
            {
                SaveUndo(modEntry);

                i = modEntry->currRow;
                if (input.keyb.shiftKeyDown)
                {
                    // kill to start
                    while (i >= 0)
                    {
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;

                        i--;
                    }
                }
                else
                {
                    // kill to end
                    while (i < MOD_ROWS)
                    {
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;

                        i++;
                    }
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_l:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockMarkFlag)
                {
                    displayErrorMsg("NO BLOCK MARKED !");
                    return;
                }

                TrackNoteDown(TRANSPOSE_NORMAL, editor.BlockFromPos, editor.BlockToPos, modEntry);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;
        
        case SDLK_m:
        {
                 if (input.keyb.controlKeyDown) editor.MultiFlag ^= 1;
            else if (input.keyb.altKeyDown)     editor.MetroFlag ^= 1;
            else                                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
        }
        break;

        case SDLK_n:
        {
            if (input.keyb.controlKeyDown)
            {
                editor.BlockMarkFlag = true;
                modEntry->currRow = editor.BlockToPos;
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_o:
        {
            if (input.keyb.controlKeyDown)
            {
                // fun fact: this function is broken in PT but I fixed it in my port

                SaveUndo(modEntry);

                j = modEntry->currRow + 1;
                while (j < MOD_ROWS)
                {
                    for (i = j; i < (MOD_ROWS - 1); ++i)
                    {
                        noteSrc = &modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel];
                        modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc;
                    }

                    // clear newly made row on very bottom
                    modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                    modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                    modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                    modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;

                   j++;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_p:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockBufferFlag)
                {
                    displayErrorMsg("BUFFER IS EMPTY !");
                    return;
                }

                SaveUndo(modEntry);

                for (i = 0; i <= (editor.BuffToPos - editor.BuffFromPos); ++i)
                {
                    if ((modEntry->currRow + i) > 63)
                        break;

                    modEntry->patterns[modEntry->currPattern][((modEntry->currRow + i) * PAULA_CHANNELS) + editor.cursor.channel]
                        = editor.BlockBuffer[editor.BuffFromPos + i];
                }

                if (!input.keyb.shiftKeyDown)
                {
                    modEntry->currRow += (i & 0xFF);
                    if (modEntry->currRow > 63)
                        modEntry->currRow = 0;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_q:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackNoteDown(TRANSPOSE_NORMAL, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                editor.muted[0] = false;
                editor.muted[1] = false;
                editor.muted[2] = false;
                editor.muted[3] = false;
            }
            else if (input.keyb.altKeyDown)
            {
                editor.ui.askScreenShown = true;
                editor.ui.askScreenType  = ASK_QUIT;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("REALLY QUIT ?", NO_CARRY);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_r:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattNoteDown(TRANSPOSE_SAMPLE, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                editor.F6pos  = 0;
                editor.F7pos  = 16;
                editor.F8pos  = 32;
                editor.F9pos  = 48;
                editor.F10pos = 63;

                displayMsg("POS RESTORED !");
            }
            else if (input.keyb.altKeyDown)
            {
                editor.ui.askScreenShown = true;
                editor.ui.askScreenType  = ASK_RESAMPLE;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("RESAMPLE?", NO_CARRY);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_s:
        {
            if (input.keyb.controlKeyDown)
            {
                saveModule(modEntry, DONT_CHECK_IF_FILE_EXIST, DONT_GIVE_NEW_FILENAME);
            }
            else if (input.keyb.leftAmigaKeyDown)
            {
                PattOctaUp(TRANSPOSE_NORMAL, modEntry);
            }
            else if (input.keyb.altKeyDown)
            {
                editor.ui.samplerScreenShown ^= 1;
                if (editor.ui.samplerScreenShown)
                    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_t:
        {
            if (input.keyb.controlKeyDown)
            {
                editor.swapChannelFlag = true;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("SWAP (1/2/3/4) ?", NO_CARRY);
            }
            else if (input.keyb.altKeyDown)
            {
                toggleTuningTone(modEntry);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_u:
        {
            if (input.keyb.controlKeyDown)
                UndoLastChange(modEntry);
            else
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
        }
        break;

        case SDLK_v:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattOctaDown(TRANSPOSE_SAMPLE, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (editor.ui.samplerScreenShown)
                {
                    samplerSamPaste(modEntry);
                }
                else
                {
                    editor.ui.askScreenShown = true;
                    editor.ui.askScreenType  = ASK_FILTER_ALL_SAMPLES;

                    pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                    setStatusMessage("FILTER ALL SAMPLS", NO_CARRY);
                }
            }
            else if (input.keyb.altKeyDown)
            {
                editor.muted[3] ^= 1; // toggle channel 4
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_w:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattNoteDown(TRANSPOSE_NORMAL, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockBufferFlag)
                {
                    displayErrorMsg("BUFFER IS EMPTY !");
                    return;
                }

                SaveUndo(modEntry);

                chTmp = editor.cursor.channel;
                for (i = 0; i <= (editor.BuffToPos - editor.BuffFromPos); ++i)
                {
                    if ((modEntry->currRow + i) > 63)
                        break;

                    noteDst = &modEntry->patterns[modEntry->currPattern][((modEntry->currRow + i) * PAULA_CHANNELS) + chTmp];

                    if (!(editor.BlockBuffer[i].period || editor.BlockBuffer[i].sample))
                    {
                        noteDst->command = editor.BlockBuffer[i].command;
                        noteDst->param   = editor.BlockBuffer[i].param;
                    }
                    else
                    {
                        *noteDst = editor.BlockBuffer[i];
                    }

                    chTmp = (chTmp + 1) & 3;
                }

                if (!input.keyb.shiftKeyDown)
                {
                    modEntry->currRow += (i & 0xFF);
                    if (modEntry->currRow > 63)
                        modEntry->currRow = 0;
                }

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_x:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                PattOctaDown(TRANSPOSE_NORMAL, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (editor.ui.samplerScreenShown)
                {
                    samplerSamDelete(modEntry, SAMPLE_CUT);
                    return;
                }

                if (!editor.BlockMarkFlag)
                {
                    displayErrorMsg("NO BLOCK MARKED !");
                    return;
                }

                editor.BlockMarkFlag = false;

                SaveUndo(modEntry); // Fun fact: One of the PT coders forgot to call this in PT1.2a/2.0, but it was added later on

                editor.BlockBufferFlag = true;

                for (i = 0; i < MOD_ROWS; ++i)
                    editor.BlockBuffer[i] = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                if (editor.BlockFromPos > editor.BlockToPos)
                {
                    editor.BuffFromPos = editor.BlockToPos;
                    editor.BuffToPos   = editor.BlockFromPos;
                }
                else
                {
                    editor.BuffFromPos = editor.BlockFromPos;
                    editor.BuffToPos   = editor.BlockToPos;
                }

                for (i = editor.BuffFromPos; i <= editor.BuffToPos; ++i)
                {
                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;
                }

                setStatusMessage("ALL RIGHT", DO_CARRY);
                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else
            {
                if (input.keyb.altKeyDown)
                    editor.muted[1] ^= 1; // toggle channel 2
                else
                    handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_y:
        {
            if (input.keyb.controlKeyDown)
            {
                if (!editor.BlockMarkFlag)
                {
                    displayErrorMsg("NO BLOCK MARKED !");
                    return;
                }

                editor.BlockMarkFlag = false;

                SaveUndo(modEntry);

                if (editor.BlockFromPos >= editor.BlockToPos)
                {
                    blockFrom = editor.BlockToPos;
                    blockTo   = editor.BlockFromPos;
                }
                else
                {
                    blockFrom = editor.BlockFromPos;
                    blockTo   = editor.BlockToPos;
                }

                while (blockFrom < blockTo)
                {
                    noteDst = &modEntry->patterns[modEntry->currPattern][(blockFrom * PAULA_CHANNELS) + editor.cursor.channel];
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(blockTo   * PAULA_CHANNELS) + editor.cursor.channel];

                    noteTmp  = *noteDst;
                    *noteDst = *noteSrc;
                    *noteSrc =  noteTmp;

                    blockFrom += 1;
                    blockTo   -= 1;
                }

                setStatusMessage("ALL RIGHT", DO_CARRY);
                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
            else if (input.keyb.altKeyDown)
            {
                editor.ui.askScreenShown = true;
                editor.ui.askScreenType  = ASK_SAVE_ALL_SAMPLES;

                pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                setStatusMessage("SAVE ALL SAMPLES?", NO_CARRY);
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        case SDLK_z:
        {
            if (input.keyb.leftAmigaKeyDown)
            {
                TrackOctaDown(TRANSPOSE_NORMAL, 0, MOD_ROWS - 1, modEntry);
            }
            else if (input.keyb.controlKeyDown)
            {
                if (editor.ui.samplerScreenShown)
                {
                    editor.ui.askScreenShown = true;
                    editor.ui.askScreenType  = ASK_RESTORE_SAMPLE;

                    pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                    setStatusMessage("RESTORE SAMPLE ?", NO_CARRY);
                }
                else
                {
                    modSetTempo(125);
                    modSetSpeed(6);

                    for (i = 0; i < PAULA_CHANNELS; ++i)
                    {
                        modEntry->channels[i].waveControl        = 0;
                        modEntry->channels[i].glissandoControl   = 0;
                        modEntry->channels[i].invertLoopSpeed    = 0;
                        modEntry->channels[i].fineTune           = 0;
                        modEntry->channels[i].patternLoopCounter = 0;
                    }

                    displayMsg("EFX RESTORED !");
                }
            }
            else if (input.keyb.altKeyDown)
            {
                editor.muted[0] ^= 1; // toggle channel 1
            }
            else
            {
                handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
            }
        }
        break;

        default:
            handleEditKeys(keyEntry, modEntry, EDIT_NORMAL);
        break;
    }
}

void movePatCurPrevCh(void)
{
    int8_t pos;

    pos = ((editor.cursor.pos + 5) / 6) - 1;

    editor.cursor.pos  = (pos < 0) ? (3 * 6) : (pos * 6);
    editor.cursor.mode = CURSOR_NOTE;

         if (editor.cursor.pos <  6) editor.cursor.channel = 0;
    else if (editor.cursor.pos < 12) editor.cursor.channel = 1;
    else if (editor.cursor.pos < 18) editor.cursor.channel = 2;
    else if (editor.cursor.pos < 24) editor.cursor.channel = 3;
}

void movePatCurNextCh(void)
{
    int8_t pos;

    pos = (editor.cursor.pos / 6) + 1;

    editor.cursor.pos  = (pos == 4) ? 0 : (pos * 6);
    editor.cursor.mode = CURSOR_NOTE;

         if (editor.cursor.pos <  6) editor.cursor.channel = 0;
    else if (editor.cursor.pos < 12) editor.cursor.channel = 1;
    else if (editor.cursor.pos < 18) editor.cursor.channel = 2;
    else if (editor.cursor.pos < 24) editor.cursor.channel = 3;
}

void movePatCurRight(void)
{
    editor.cursor.pos = (editor.cursor.pos == 23) ? 0 : (editor.cursor.pos + 1);

         if (editor.cursor.pos <  6) editor.cursor.channel = 0;
    else if (editor.cursor.pos < 12) editor.cursor.channel = 1;
    else if (editor.cursor.pos < 18) editor.cursor.channel = 2;
    else if (editor.cursor.pos < 24) editor.cursor.channel = 3;

    editor.cursor.mode = editor.cursor.pos % 6;
}

void movePatCurLeft(void)
{
    editor.cursor.pos = (editor.cursor.pos == 0) ? 23 : (editor.cursor.pos - 1);

         if (editor.cursor.pos <  6) editor.cursor.channel = 0;
    else if (editor.cursor.pos < 12) editor.cursor.channel = 1;
    else if (editor.cursor.pos < 18) editor.cursor.channel = 2;
    else if (editor.cursor.pos < 24) editor.cursor.channel = 3;

    editor.cursor.mode = editor.cursor.pos % 6;
}

void handleKeyRepeat(SDLKey keyEntry, MODULE *modEntry)
{
    uint8_t repeatNum;

    if (!input.keyb.repeatKey)
    {
        input.keyb.repeatCounter = 0;
        return;
    }

    if (input.keyb.delayKey)
    {
        if (input.keyb.delayCounter < KEYB_REPEAT_DELAY)
        {
            input.keyb.delayCounter++;
            return;
        }
    }

    switch (keyEntry)
    {
        // Special cases
        case SDLK_PAGEUP:
        {
            if (input.keyb.repeatCounter >= 3)
            {
                input.keyb.repeatCounter = 0;

                if (editor.ui.posEdScreenShown)
                {
                    if ((modEntry->currOrder - (POSED_LIST_SIZE - 1)) > 0)
                        modSetPos(modEntry->currOrder - (POSED_LIST_SIZE - 1), DONT_SET_ROW);
                    else
                        modSetPos(0, DONT_SET_ROW);
                }
                else if (editor.ui.diskOpScreenShown)
                {
                    if (editor.ui.diskOpScreenShown)
                    {
                        editor.diskop.scrollOffset -= (DISKOP_LIST_SIZE - 1);
                        if (editor.diskop.scrollOffset < 0)
                            editor.diskop.scrollOffset = 0;
                    }
                }
                else
                {
                    if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                    {
                             if (modEntry->currRow == 63) modSetPos(DONT_SET_ORDER, modEntry->currRow - 15);
                        else if (modEntry->currRow == 15) modSetPos(DONT_SET_ORDER, 0); // 15-16 would turn into -1, which is "DON'T SET ROW" flag
                        else                              modSetPos(DONT_SET_ORDER, modEntry->currRow - 16);
                    }
                }
            }
        }
        break;

        case SDLK_PAGEDOWN:
        {
            if (input.keyb.repeatCounter >= 3)
            {
                input.keyb.repeatCounter = 0;

                if (editor.ui.posEdScreenShown)
                {
                    if ((modEntry->currOrder + (POSED_LIST_SIZE - 1)) <= (modEntry->head.orderCount - 1))
                        modSetPos(modEntry->currOrder + (POSED_LIST_SIZE - 1), DONT_SET_ROW);
                    else
                        modSetPos(modEntry->head.orderCount - 1, DONT_SET_ROW);
                }
                else if (editor.ui.diskOpScreenShown)
                {
                    if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                    {
                        editor.diskop.scrollOffset += (DISKOP_LIST_SIZE - 1);
                        if (editor.diskop.scrollOffset > (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                            editor.diskop.scrollOffset =  editor.diskop.numFiles - DISKOP_LIST_SIZE;
                    }
                }
                else
                {
                    if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        modSetPos(DONT_SET_ORDER, modEntry->currRow + 16);
                }
            }
        }
        break;

        case SDLK_LEFT:
        {
            if (editor.ui.getLineFlag)
            {
                if (input.keyb.repeatCounter >= 3)
                {
                    input.keyb.repeatCounter = 0;
                    moveCharLeft();
                }
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                {
                   if (input.keyb.repeatCounter >= 1)
                    {
                        input.keyb.repeatCounter = 0;

                        // twice for super speed!
                        moveSampleMarkerLeft(input.keyb.shiftKeyDown, false);
                        moveSampleMarkerLeft(input.keyb.shiftKeyDown, false);
                    }
                }
                else
                {
                    if (input.keyb.repeatCounter >= 7)
                    {
                        input.keyb.repeatCounter = 0;

                        if (!input.keyb.shiftKeyDown && !input.keyb.altKeyDown && !input.keyb.controlKeyDown)
                            movePatCurLeft();
                    }
                }
            }
        }
        break;

        case SDLK_RIGHT:
        {
            if (editor.ui.getLineFlag)
            {
                if (input.keyb.repeatCounter >= 3)
                {
                    input.keyb.repeatCounter = 0;
                    moveCharRight();
                }
            }
            else
            {
                if (editor.ui.samplerScreenShown)
                {
                    if (input.keyb.repeatCounter >= 1)
                    {
                        input.keyb.repeatCounter = 0;

                        // twice for super speed!
                        moveSampleMarkerRight(input.keyb.shiftKeyDown, false);
                        moveSampleMarkerRight(input.keyb.shiftKeyDown, false);
                    }
                }
                else
                {
                    if (input.keyb.repeatCounter >= 7)
                    {
                        input.keyb.repeatCounter = 0;

                        if (!input.keyb.shiftKeyDown && !input.keyb.altKeyDown && !input.keyb.controlKeyDown)
                            movePatCurRight();
                    }
                }
            }
        }
        break;

        case SDLK_UP:
        {
            if (editor.ui.getLineFlag)
                return;

            if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
            {
                if (editor.ui.diskOpScreenShown)
                {
                    if (input.keyb.repeatCounter >= 1)
                    {
                        input.keyb.repeatCounter = 0;

                        if (editor.diskop.scrollOffset > 0)
                            editor.diskop.scrollOffset--;
                    }
                }
                else if (editor.ui.posEdScreenShown)
                {
                    if (input.keyb.repeatCounter >= 2)
                    {
                        input.keyb.repeatCounter = 0;

                        if (modEntry->currOrder > 0)
                            modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);
                    }
                }
                else if (!editor.ui.samplerScreenShown)
                {
                    if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                    {
                        repeatNum = 7;

                             if (input.keyb.altKeyDown)   repeatNum = 1;
                        else if (input.keyb.shiftKeyDown) repeatNum = 3;

                        if (input.keyb.repeatCounter >= repeatNum)
                        {
                            input.keyb.repeatCounter = 0;

                            modSetPos(DONT_SET_ORDER, (modEntry->currRow - 1) & 0x3F);
                        }
                    }
                }
            }
        }
        break;

        case SDLK_DOWN:
        {
            if (editor.ui.getLineFlag)
                return;

            if (!editor.ui.askScreenShown && !editor.ui.clearScreenShown)
            {
                if (editor.ui.diskOpScreenShown)
                {
                    if (input.keyb.repeatCounter >= 1)
                    {
                        input.keyb.repeatCounter = 0;

                        if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                        {
                            if (editor.diskop.scrollOffset < (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                                editor.diskop.scrollOffset++;
                        }
                    }
                }
                else if (editor.ui.posEdScreenShown)
                {
                    if (input.keyb.repeatCounter >= 2)
                    {
                        input.keyb.repeatCounter = 0;

                        if (modEntry->currOrder != (modEntry->head.orderCount - 1))
                        {
                            if (++modEntry->currOrder > (modEntry->head.orderCount - 1))
                                  modEntry->currOrder =  modEntry->head.orderCount - 1;

                            modSetPos(modEntry->currOrder, DONT_SET_ROW);
                        }
                    }
                }
                else if (!editor.ui.samplerScreenShown)
                {
                    if ((editor.currentMode != MODE_PLAY) && (editor.currentMode != MODE_RECORD))
                    {
                        repeatNum = 7;

                             if (input.keyb.altKeyDown)   repeatNum = 1;
                        else if (input.keyb.shiftKeyDown) repeatNum = 3;

                        if (input.keyb.repeatCounter >= repeatNum)
                        {
                            input.keyb.repeatCounter = 0;

                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + 1) & 0x3F);
                        }
                    }
                }
            }
        }
        break;

        default:
        {
            if (input.keyb.repeatCounter >= 3)
            {
                input.keyb.repeatCounter = 0;

                keyDownHandler(keyEntry, modEntry);
            }
        }
        break;
    }

    input.keyb.repeatCounter++;
}

int8_t handleGeneralModes(SDLKey keyEntry, MODULE *modEntry)
{
    int8_t rawKey;

    int16_t i;

    modnote_t *noteSrc;
    modnote_t noteTmp;

    // SAMPLER SCREEN (volume box)
    if (editor.ui.samplerVolBoxShown && !editor.ui.getLineFlag)
    {
        if (keyEntry == SDLK_ESCAPE)
        {
            editor.ui.samplerVolBoxShown = false;
            return (false);
        }
    }

    // EDIT OP. SCREEN #3
    if (editor.MixFlag)
    {
        if (keyEntry == SDLK_ESCAPE)
        {
            exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
            editor.MixFlag = false;

            return (false);
        }
    }

    // EDIT OP. SCREEN #4
    if (editor.ui.changingChordNote)
    {
        if (keyEntry == SDLK_ESCAPE)
        {
            editor.ui.changingChordNote = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();

            return (false);
        }

             if (keyEntry == SDLK_F1) editor.keyOctave = OCTAVE_LOW;
        else if (keyEntry == SDLK_F2) editor.keyOctave = OCTAVE_HIGH;

        rawKey = keyToNote(keyEntry);
        if (rawKey >= 0)
        {
                 if (editor.ui.changingChordNote == 1) editor.Note1 = rawKey;
            else if (editor.ui.changingChordNote == 2) editor.Note2 = rawKey;
            else if (editor.ui.changingChordNote == 3) editor.Note3 = rawKey;
            else if (editor.ui.changingChordNote == 4) editor.Note4 = rawKey;

            editor.ui.changingChordNote = false;

            recalcChordLength(modEntry);

            setPrevStatusMessage();
            pointerSetPreviousMode();
        }

        return (false);
    }

    // CHANGE DRUMPAD NOTE
    if (editor.ui.changingDrumPadNote)
    {
        if (keyEntry == SDLK_ESCAPE)
        {
            editor.ui.changingDrumPadNote = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();

            return (false);
        }

             if (keyEntry == SDLK_F1) editor.keyOctave = OCTAVE_LOW;
        else if (keyEntry == SDLK_F2) editor.keyOctave = OCTAVE_HIGH;

        rawKey = keyToNote(keyEntry);
        if (rawKey >= 0)
        {
            pnotetable[editor.currentSample] = rawKey;
            editor.ui.changingDrumPadNote    = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();
        }

        return (false);
    }


    // SAMPLER SCREEN
    if (editor.ui.changingSmpResample)
    {
        if (keyEntry == SDLK_ESCAPE)
        {
            editor.ui.changingSmpResample = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();

            return (false);
        }

             if (keyEntry == SDLK_F1) editor.keyOctave = OCTAVE_LOW;
        else if (keyEntry == SDLK_F2) editor.keyOctave = OCTAVE_HIGH;

        rawKey = keyToNote(keyEntry);
        if (rawKey >= 0)
        {
            editor.ResampleNote           = rawKey;
            editor.ui.changingSmpResample = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();
        }

        return (false);
    }

    // DISK OP. SCREEN
    if (editor.diskop.isFilling)
    {
        if (editor.ui.askScreenShown && (editor.ui.askScreenType == ASK_QUIT))
        {
            if (keyEntry == SDLK_y)
            {
                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = false;
                editor.ui.answerYes      = true;

                handleAskYes(modEntry);
            }
            else if (keyEntry == SDLK_n)
            {
                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = true;
                editor.ui.answerYes      = false;

                handleAskNo(modEntry);
            }
        }

        return (false);
    }

    // if MOD2WAV is ongoing, only react to ESC and Y/N on exit ask dialog
    if (editor.isWAVRendering)
    {
        if (editor.ui.askScreenShown && (editor.ui.askScreenType == ASK_QUIT))
        {
            if (keyEntry == SDLK_y)
            {
                editor.isWAVRendering = false;
                SDL_WaitThread(editor.mod2WavThread, NULL);

                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = false;
                editor.ui.answerYes      = true;

                handleAskYes(modEntry);
            }
            else if (keyEntry == SDLK_n)
            {
                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = true;
                editor.ui.answerYes      = false;

                handleAskNo(modEntry);

                pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
                setStatusMessage("RENDERING MOD...", NO_CARRY);
            }
        }
        else if (keyEntry == SDLK_ESCAPE)
        {
            editor.playMode    = PLAY_MODE_NORMAL;
            editor.currentMode = MODE_IDLE;

            pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
            setStatusMessage("ALL RIGHT", DO_CARRY);

            editor.isWAVRendering = false;
            SDL_WaitThread(editor.mod2WavThread, NULL);
            displayErrorMsg("MOD2WAV ABORTED !");

            resetSong();
        }

        return (false);
    }

    // SWAP CHANNEL (CTRL+T)
    if (editor.swapChannelFlag)
    {
        switch (keyEntry)
        {
            case SDLK_ESCAPE:
            {
                editor.swapChannelFlag = false;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_1:
            {
                for (i = 0; i < MOD_ROWS; ++i)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                    noteTmp = modEntry->patterns[modEntry->currPattern][i * PAULA_CHANNELS];

                    modEntry->patterns[modEntry->currPattern][i * PAULA_CHANNELS] = *noteSrc;
                    *noteSrc = noteTmp;
                }

                editor.swapChannelFlag = false;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_2:
            {
                for (i = 0; i < MOD_ROWS; ++i)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                    noteTmp = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 1];

                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 1] = *noteSrc;
                    *noteSrc = noteTmp;
                }

                editor.swapChannelFlag = false;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_3:
            {
                for (i = 0; i < MOD_ROWS; ++i)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                    noteTmp = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 2];

                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 2] = *noteSrc;
                    *noteSrc = noteTmp;
                }

                editor.swapChannelFlag = false;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_4:
            {
                for (i = 0; i < MOD_ROWS; ++i)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                    noteTmp = modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 3];

                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + 3] = *noteSrc;
                    *noteSrc = noteTmp;
                }

                editor.swapChannelFlag = false;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            default: break;
        }

        return (false);
    }

    // YES/NO ASK DIALOG
    if (editor.ui.askScreenShown)
    {
        switch (keyEntry)
        {
            case SDLK_ESCAPE:
            case SDLK_n:
            {
                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = true;
                editor.ui.answerYes      = false;

                handleAskNo(modEntry);
            }
            break;

            case SDLK_RETURN:
            case SDLK_y:
            {
                editor.ui.askScreenShown = false;
                editor.ui.answerNo       = false;
                editor.ui.answerYes      = true;

                handleAskYes(modEntry);

                // pointer/status is updated by the 'yes handler'
            }
            break;

            default: break;
        }

        return (false);
    }

    // CLEAR SCREEN DIALOG
    if (editor.ui.clearScreenShown)
    {
        switch (keyEntry)
        {
            case SDLK_s:
            {
                editor.ui.clearScreenShown = false;

                modStop();
                clearSamples();

                editor.playMode    = PLAY_MODE_NORMAL;
                editor.currentMode = MODE_IDLE;

                pointerSetPreviousMode();
                setPrevStatusMessage();

                setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            }
            break;

            case SDLK_o:
            {
                editor.ui.clearScreenShown = false;

                modStop();
                clearSong();

                editor.playMode    = PLAY_MODE_NORMAL;
                editor.currentMode = MODE_IDLE;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_a:
            {
                editor.ui.clearScreenShown = false;

                modStop();
                clearAll();

                editor.playMode    = PLAY_MODE_NORMAL;
                editor.currentMode = MODE_IDLE;

                pointerSetPreviousMode();
                setPrevStatusMessage();
            }
            break;

            case SDLK_c:
            case SDLK_ESCAPE:
            {
                editor.ui.clearScreenShown = false;

                editor.currentMode = MODE_IDLE;

                pointerSetPreviousMode();
                setPrevStatusMessage();

                input.mouse.buttonErrorMsgActive = true;
                input.mouse.buttonErrorMsgBlock  = true;
                input.mouse.errorMsgCounter      = 0;

                pointerErrorMode();
            }
            break;

            default: break;
        }

        return (false);
    }

    return (true);
}

int8_t handleTextEditMode(SDLKey keyEntry, MODULE *modEntry)
{
    char *readTmp;
    int8_t readTmpPrev;
    int8_t readTmpNext;
    int8_t rawKey;
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    uint8_t digit4;

    int16_t i;
    int16_t j;

    uint32_t number;

    modnote_t *noteSrc;

    switch (keyEntry)
    {
        case SDLK_ESCAPE:
        {
            editor.BlockMarkFlag = false;

            if (editor.ui.getLineFlag)
            {
                exitGetTextLine(modEntry, EDIT_TEXT_NO_UPDATE);
                return (false);
            }
        }
        break;

        case SDLK_HOME:
        {
            if (editor.ui.getLineFlag && !editor.MixFlag)
            {
                while (editor.ui.editPos > editor.ui.showTextPtr)
                    moveCharLeft();
            }
        }
        break;

        case SDLK_END:
        {
            if (editor.ui.getLineFlag && !editor.MixFlag)
            {
                if (editor.ui.getLineType != TEXT_EDIT_STRING)
                    break;

                while (!editor.ui.dstOffsetEnd)
                    moveCharRight();
            }
        }
        break;

        case SDLK_LEFT:
        {
            if (editor.ui.getLineFlag)
            {
                if (editor.ui.getLineType != TEXT_EDIT_STRING)
                    break;

                moveCharLeft();

                if (!input.keyb.repeatKey)
                    input.keyb.delayCounter = 0;

                input.keyb.repeatKey = true;
                input.keyb.delayKey  = true;
            }
            else
            {
                input.keyb.delayKey  = false;
                input.keyb.repeatKey = true;
            }
        }
        break;

        case SDLK_RIGHT:
        {
            if (editor.ui.getLineFlag)
            {
                if (editor.ui.getLineType != TEXT_EDIT_STRING)
                    break;

                moveCharRight();

                if (!input.keyb.repeatKey)
                    input.keyb.delayCounter = 0;

                input.keyb.repeatKey = true;
                input.keyb.delayKey  = true;
            }
            else
            {
                input.keyb.delayKey  = false;
                input.keyb.repeatKey = true;
            }
        }
        break;

        case SDLK_DELETE:
        {
            if (editor.ui.getLineFlag)
            {
                if (editor.MixFlag || (editor.ui.getLineType != TEXT_EDIT_STRING))
                    break;

                readTmp = editor.ui.editPos;
                while (readTmp < editor.ui.textEndPtr)
                {
                    readTmpNext = *(readTmp + 1);
                    *readTmp++  = readTmpNext;
                }

                if (!input.keyb.repeatKey)
                    input.keyb.delayCounter = 0;

                input.keyb.repeatKey = true;
                input.keyb.delayKey  = true;
            }
        }
        break;

        case SDLK_BACKSPACE:
        {
            if (editor.ui.getLineFlag)
            {
                if (editor.MixFlag || (editor.ui.getLineType != TEXT_EDIT_STRING))
                    break;

                if (editor.ui.editPos > editor.ui.dstPtr)
                {
                    editor.ui.editPos--;

                    readTmp = editor.ui.editPos;
                    while (readTmp < editor.ui.textEndPtr)
                    {
                        readTmpNext = *(readTmp + 1);
                        *readTmp++  = readTmpNext;
                    }

                    posMoveLeft();
                }

                if (!input.keyb.repeatKey)
                    input.keyb.delayCounter = 0;

                input.keyb.repeatKey = true;
                input.keyb.delayKey  = true;
            }
            else
            {
                if (editor.ui.diskOpScreenShown)
                {
                    diskOpGoParent();
                }
                else if (input.keyb.shiftKeyDown || input.keyb.altKeyDown || input.keyb.controlKeyDown)
                {
                    SaveUndo(modEntry);

                    if (input.keyb.altKeyDown && !input.keyb.controlKeyDown)
                    {
                        if (modEntry->currRow > 0)
                        {
                            for (i = 0; i < PAULA_CHANNELS; ++i)
                            {
                                for (j = (modEntry->currRow - 1); j < MOD_ROWS; ++j)
                                {
                                    noteSrc = &modEntry->patterns[modEntry->currPattern][((j + 1) * PAULA_CHANNELS) + i];
                                    modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i] = *noteSrc;
                                }

                                // clear newly made row on very bottom
                                modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + i].period  = 0;
                                modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + i].sample  = 0;
                                modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + i].command = 0;
                                modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + i].param   = 0;
                            }

                            modEntry->currRow--;
                        }
                    }
                    else
                    {
                        if (modEntry->currRow > 0)
                        {
                            for (i = (modEntry->currRow - 1); i < (MOD_ROWS - 1); ++i)
                            {
                                noteSrc = &modEntry->patterns[modEntry->currPattern][((i + 1) * PAULA_CHANNELS) + editor.cursor.channel];

                                if (input.keyb.controlKeyDown)
                                {
                                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].command = noteSrc->command;
                                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel].param   = noteSrc->param;
                                }
                                else
                                {
                                    modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel] = *noteSrc;
                                }
                            }

                            // clear newly made row on very bottom

                            modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].period  = 0;
                            modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].sample  = 0;
                            modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].command = 0;
                            modEntry->patterns[modEntry->currPattern][(63 * PAULA_CHANNELS) + editor.cursor.channel].param   = 0;

                            modEntry->currRow--;
                        }
                    }
                }
                else
                {
                    editor.stepPlayEnabled   = true;
                    editor.stepPlayBackwards = true;

                    doStopIt();
                    playPattern((modEntry->currRow - 1) & 0x3F);
                }
            }
        }
        break;

        default: break;
    }

    if (editor.ui.getLineFlag)
    {
        if ((keyEntry == SDLK_RETURN) || (keyEntry == SDLK_KP_ENTER))
        {
            // dirty hack
            if (editor.ui.editID == PTB_SAMPLES)
                editor.ui.tmpDisp8++;

            exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);

            if (editor.MixFlag)
            {
                editor.MixFlag = false;
                DoMix(modEntry);
            }

            return (false);
        }

        if (editor.ui.getLineType == TEXT_EDIT_STRING)
        {
            if ((keyEntry == SDLK_DELETE) || (keyEntry == SDLK_BACKSPACE)) return (false); // handled above this section
            if ((keyEntry == SDLK_LSHIFT) || (keyEntry == SDLK_RSHIFT))    return (false); // get normal key the next round
        }

        rawKey = (char)(keyEntry);

        // check for numpad keys
        if ((keyEntry >= SDLK_KP0) && (keyEntry <= SDLK_KP_EQUALS))
        {
            switch (keyEntry)
            {
                case SDLK_KP0:         rawKey = '0'; break;
                case SDLK_KP1:         rawKey = '1'; break;
                case SDLK_KP2:         rawKey = '2'; break;
                case SDLK_KP3:         rawKey = '3'; break;
                case SDLK_KP4:         rawKey = '4'; break;
                case SDLK_KP5:         rawKey = '5'; break;
                case SDLK_KP6:         rawKey = '6'; break;
                case SDLK_KP7:         rawKey = '7'; break;
                case SDLK_KP8:         rawKey = '8'; break;
                case SDLK_KP9:         rawKey = '9'; break;
                case SDLK_KP_PERIOD:   rawKey = '.'; break;
                case SDLK_KP_DIVIDE:   rawKey = '/'; break;
                case SDLK_KP_MULTIPLY: rawKey = '*'; break;
                case SDLK_KP_MINUS:    rawKey = '-'; break;
                case SDLK_KP_PLUS:     rawKey = '+'; break;
                case SDLK_KP_EQUALS:   rawKey = '='; break;

                default: break;
            }
        }

        // if not numpad keys, check for shift (for shifting keys)
        else if (input.keyb.shiftKeyDown)
        {
            if (editor.ui.getLineType == TEXT_EDIT_STRING)
            {
                switch ((char)(keyEntry))
                {
                    // U.S. keyb shift table
                    case  '`': rawKey = '~'; break;
                    case  '1': rawKey = '!'; break;
                    case  '2': rawKey = '@'; break;
                    case  '3': rawKey = '#'; break;
                    case  '4': rawKey = '$'; break;
                    case  '5': rawKey = '%'; break;
                    case  '6': rawKey = '^'; break;
                    case  '7': rawKey = '&'; break;
                    case  '8': rawKey = '*'; break;
                    case  '9': rawKey = '('; break;
                    case  '0': rawKey = ')'; break;
                    case  '-': rawKey = '_'; break;
                    case  '=': rawKey = '+'; break;
                    case  '[': rawKey = '{'; break;
                    case  ']': rawKey = '}'; break;
                    case '\\': rawKey = '|'; break;
                    case  ';': rawKey = ':'; break;
                    case '\'': rawKey = '"'; break;
                    case  ',': rawKey = '<'; break;
                    case  '.': rawKey = '>'; break;
                    case  '/': rawKey = '?'; break;

                    default: break;
                }
            }
        }

        // we only want certain ASCII keys
        if (((keyEntry >= 32) && (keyEntry <= 126)) || ((keyEntry >= SDLK_KP0) && (keyEntry <= SDLK_KP_EQUALS)))
        {
            if ((rawKey >= 'a') && (rawKey <= 'z'))
                rawKey -= 32; // a..z -> A..Z

            if (editor.ui.getLineType == TEXT_EDIT_STRING)
            {
                if (editor.ui.editPos < editor.ui.textEndPtr)
                {
                    if (!editor.MixFlag)
                    {
                        readTmp = editor.ui.textEndPtr;
                        while (readTmp > editor.ui.editPos)
                        {
                            readTmpPrev    = *--readTmp;
                            *(readTmp + 1) = readTmpPrev;
                        }

                        *editor.ui.textEndPtr = '\0';
                        *editor.ui.editPos++  = rawKey;

                        posMoveRight();
                    }
                    else
                    {
                        if (((rawKey >= '0') && (rawKey <= '9')) || ((rawKey >= 'A') && (rawKey <= 'F')))
                        {
                            if (editor.ui.dstPos == 14)
                            {
                                *editor.ui.editPos = rawKey;
                            }
                            else
                            {
                                *editor.ui.editPos++ = rawKey;
                                posMoveRight();

                                if (editor.ui.dstPos == 9)
                                {
                                    editor.ui.editPos++;
                                    posMoveRight();

                                    editor.ui.editPos++;
                                    posMoveRight();

                                    editor.ui.editPos++;
                                    posMoveRight();

                                    editor.ui.editPos++;
                                    posMoveRight();
                                }
                                else if (editor.ui.dstPos == 6)
                                {
                                    editor.ui.editPos++;
                                    posMoveRight();
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                if (editor.ui.getLineType == TEXT_EDIT_DECIMAL)
                {
                    if ((rawKey >= '0') && (rawKey <= '9'))
                    {
                        rawKey -= '0';

                        if (editor.ui.numLen == 4)
                        {
                            number = *editor.ui.numPtr16;
                            digit4 = number % 10; number /= 10;
                            digit3 = number % 10; number /= 10;
                            digit2 = number % 10; number /= 10;
                            digit1 = (uint8_t)(number);

                                 if (editor.ui.dstPos == 0) *editor.ui.numPtr16 = (rawKey * 1000) + (digit2 * 100) + (digit3 * 10) + digit4;
                            else if (editor.ui.dstPos == 1) *editor.ui.numPtr16 = (digit1 * 1000) + (rawKey * 100) + (digit3 * 10) + digit4;
                            else if (editor.ui.dstPos == 2) *editor.ui.numPtr16 = (digit1 * 1000) + (digit2 * 100) + (rawKey * 10) + digit4;
                            else if (editor.ui.dstPos == 3) *editor.ui.numPtr16 = (digit1 * 1000) + (digit2 * 100) + (digit3 * 10) + rawKey;
                        }
                        else if (editor.ui.numLen == 3)
                        {
                            number = *editor.ui.numPtr16;
                            digit3 = number % 10; number /= 10;
                            digit2 = number % 10; number /= 10;
                            digit1 = (uint8_t)(number);

                                 if (editor.ui.dstPos == 0) *editor.ui.numPtr16 = (rawKey * 100) + (digit2 * 10) + digit3;
                            else if (editor.ui.dstPos == 1) *editor.ui.numPtr16 = (digit1 * 100) + (rawKey * 10) + digit3;
                            else if (editor.ui.dstPos == 2) *editor.ui.numPtr16 = (digit1 * 100) + (digit2 * 10) + rawKey;
                        }
                        else if (editor.ui.numLen == 2)
                        {
                            number = *editor.ui.numPtr16;
                            digit2 = number % 10; number /= 10;
                            digit1 = (uint8_t)(number);

                                 if (editor.ui.dstPos == 0) *editor.ui.numPtr16 = (rawKey * 10) + digit2;
                            else if (editor.ui.dstPos == 1) *editor.ui.numPtr16 = (digit1 * 10) + rawKey;
                        }

                        posMoveRight();
                        if (editor.ui.dstPos >= editor.ui.numLen)
                            exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
                    }
                }
                else
                {
                    if (((rawKey >= '0') && (rawKey <= '9')) || ((rawKey >= 'A') && (rawKey <= 'F')))
                    {
                             if (rawKey <= '9') rawKey -= '0';
                        else if (rawKey <= 'F') rawKey -= ('A' - 10);

                        if (editor.ui.numBits == 17)
                        {
                            // only for sample length/repeat/replen
                            *editor.ui.numPtr32 &= ~(0x000F0000 >> (editor.ui.dstPos << 2));
                            *editor.ui.numPtr32 |= (rawKey << (16 - (editor.ui.dstPos << 2)));
                        }
                        else if (editor.ui.numBits == 16)
                        {
                            *editor.ui.numPtr16 &= ~(0xF000 >> (editor.ui.dstPos << 2));
                            *editor.ui.numPtr16 |= (rawKey << (12 - (editor.ui.dstPos << 2)));
                        }
                        else if (editor.ui.numBits == 8)
                        {
                            *editor.ui.numPtr8 &= ~(0xF0 >> (editor.ui.dstPos << 2));
                            *editor.ui.numPtr8 |= (rawKey << (4 - (editor.ui.dstPos << 2)));
                        }

                        posMoveRight();
                        if (editor.ui.dstPos >= editor.ui.numLen)
                            exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
                    }
                }
            }

            if (!input.keyb.repeatKey)
                input.keyb.delayCounter = 0;

            input.keyb.repeatKey = true;
            input.keyb.delayKey  = true;
        }

        return (false);
    }

    return (true);
}
