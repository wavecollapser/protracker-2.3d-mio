/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <SDL/SDL.h>
#include <string.h>
#include <stdint.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include "pt_header.h"
#include "pt_buttons.h"
#include "pt_helpers.h"
#include "pt_palette.h"
#include "pt_diskop.h"
#include "pt_sampler.h"
#include "pt_modloader.h"
#include "pt_edit.h"
#include "pt_sampleloader.h"
#include "pt_visuals.h"
#include "pt_tables.h"
#include "pt_audio.h"

void edNote1UpButton(MODULE *modEntry);
void edNote1DownButton(MODULE *modEntry);
void edNote2UpButton(MODULE *modEntry);
void edNote2DownButton(MODULE *modEntry);
void edNote3UpButton(MODULE *modEntry);
void edNote3DownButton(MODULE *modEntry);
void edNote4UpButton(MODULE *modEntry);
void edNote4DownButton(MODULE *modEntry);
void edPosUpButton(MODULE *modEntry, int8_t fast);
void edPosDownButton(int8_t fast);
void edModUpButton(void);
void edModDownButton(void);
void edVolUpButton(void);
void edVolDownButton(void);

void sampleUpButton(MODULE *modEntry);
void sampleDownButton(MODULE *modEntry);
void sampleFineTuneUpButton(MODULE *modEntry);
void sampleFineTuneDownButton(MODULE *modEntry);
void sampleVolumeUpButton(MODULE *modEntry);
void sampleVolumeDownButton(MODULE *modEntry);
void sampleLengthUpButton(MODULE *modEntry, int8_t fast);
void sampleLengthDownButton(MODULE *modEntry, int8_t fast);
void sampleRepeatUpButton(MODULE *modEntry, int8_t fast);
void sampleRepeatDownButton(MODULE *modEntry, int8_t fast);
void sampleRepeatLengthUpButton(MODULE *modEntry, int8_t fast);
void sampleRepeatLengthDownButton(MODULE *modEntry, int8_t fast);
void tempoUpButton(MODULE *modEntry);
void tempoDownButton(MODULE *modEntry);
void songLengthUpButton(MODULE *modEntry);
void songLengthDownButton(MODULE *modEntry);
void patternUpButton(MODULE *modEntry);
void patternDownButton(MODULE *modEntry);
void positionUpButton(MODULE *modEntry);
void positionDownButton(MODULE *modEntry);

void handleSamplerVolumeBox(MODULE *modEntry);

void mouseButtonUpHandler(uint8_t mouseButton, MODULE *modEntry)
{
    input.mouse.buttonWaitCounter = 0;
    input.mouse.buttonWaiting     = false;

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        input.mouse.leftButtonPressed    = false;
        editor.ui.forceSampleDrag        = false;
        editor.ui.forceVolDrag           = false;
        editor.ui.leftLoopPinMoving      = false;
        editor.ui.rightLoopPinMoving     = false;
        editor.ui.sampleMarkingPos       = -1;

        switch (input.mouse.lastClickedGUIButton)
        {
            case PTB_SLENGTHU:
            case PTB_SLENGTHD:
            {
                setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                UpdateVoiceParameters(modEntry);
                calcRealModSize(modEntry);
                recalcChordLength(modEntry);
            }
            break;

            case PTB_LENGTHU:
            case PTB_LENGTHD:
            case PTB_PATTERNU:
            case PTB_PATTERND:
                calcRealModSize(modEntry);
            break;

            default:
                break;
        }

        input.mouse.lastClickedGUIButton = -1;
    }

    if (mouseButton == SDL_BUTTON_RIGHT)
    {
        input.mouse.rightButtonPressed = false;
        editor.ui.forceSampleEdit      = false;
    }
}

void mouseButtonDownHandler(uint8_t mouseButton, MODULE *modEntry)
{
    char pat2SmpText[18];
    char *tmpRead;
    int8_t *ptr8_1;
    int8_t *ptr8_2;
    int8_t *ptr8_3;
    int8_t *ptr8_4;
    int8_t tmpSmp;
    int8_t modTmp;
    int8_t modDat;
    uint8_t i;

    int16_t tmpWord;

    int32_t modPos;
    int32_t j;
    int32_t guiButton;
    int32_t tmp;

    MODULE_SAMPLE *s;

    if (mouseButton == SDL_BUTTON_LEFT)
    {
        input.mouse.leftButtonPressed = true;
        input.mouse.buttonWaiting     = true;
    }

    if (mouseButton == SDL_BUTTON_RIGHT)
        input.mouse.rightButtonPressed = true;

    // after cancelling an ask dialog, block further input for a while
    if (input.mouse.buttonErrorMsgActive && input.mouse.buttonErrorMsgBlock)
        return;

    // close dialogs with right mouse button
    if (input.mouse.rightButtonPressed)
    {
        if (editor.ui.clearScreenShown)
        {
            editor.ui.clearScreenShown = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();

            input.mouse.buttonErrorMsgActive = true;
            input.mouse.buttonErrorMsgBlock  = true;
            input.mouse.errorMsgCounter      = 0;

            pointerErrorMode();
            return;
        }

        if (editor.ui.askScreenShown)
        {
            editor.ui.askScreenShown = false;
            editor.ui.answerNo       = true;
            editor.ui.answerYes      = false;

            handleAskNo(modEntry); // mouse pointer is set to error (red) in here
            return;
        }
    }

    // handle volume toolbox in sampler screen
    if (editor.ui.samplerVolBoxShown)
    {
        handleSamplerVolumeBox(modEntry);
        return;
    }

    // cancel note input gadgets with left/right mouse button
    if (editor.ui.changingSmpResample || editor.ui.changingChordNote || editor.ui.changingDrumPadNote)
    {
        if (input.mouse.leftButtonPressed || input.mouse.rightButtonPressed)
        {
            editor.ui.changingSmpResample = false;
            editor.ui.changingChordNote   = false;
            editor.ui.changingDrumPadNote = false;

            setPrevStatusMessage();
            pointerSetPreviousMode();
        }

        return;
    }

    // handle QUIT ask dialog while Disk Op. filling is ongoing
    if (editor.diskop.isFilling)
    {
        if (input.mouse.leftButtonPressed)
        {
            if (editor.ui.askScreenShown && (editor.ui.askScreenType == ASK_QUIT))
            {
                guiButton = checkGUIButtons();
                if (guiButton != -1)
                {
                    switch (guiButton)
                    {
                        case PTB_SUREY:
                        {
                            editor.ui.askScreenShown = false;
                            editor.ui.answerNo       = false;
                            editor.ui.answerYes      = true;

                            handleAskYes(modEntry);
                        }
                        break;

                        case PTB_SUREN:
                        {
                            editor.ui.askScreenShown = false;
                            editor.ui.answerNo       = true;
                            editor.ui.answerYes      = false;

                            handleAskNo(modEntry);
                        }
                        break;

                        default: break;
                    }
                }
            }
        }

        return;
    }

    // CANCEL and YES/NO (ask exit) buttons while MOD2WAV is ongoing
    if (editor.isWAVRendering)
    {
        if (input.mouse.leftButtonPressed)
        {
            if (editor.ui.askScreenShown && (editor.ui.askScreenType == ASK_QUIT))
            {
                guiButton = checkGUIButtons();
                if (guiButton != -1)
                {
                    switch (guiButton)
                    {
                        case PTB_SUREY:
                        {
                            editor.isWAVRendering = false;
                            SDL_WaitThread(editor.mod2WavThread, NULL);

                            editor.ui.askScreenShown = false;
                            editor.ui.answerNo       = false;
                            editor.ui.answerYes      = true;

                            handleAskYes(modEntry);
                        }
                        break;

                        case PTB_SUREN:
                        {
                            editor.ui.askScreenShown = false;
                            editor.ui.answerNo       = true;
                            editor.ui.answerYes      = false;

                            handleAskNo(modEntry);

                            pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
                            setStatusMessage("RENDERING MOD...", NO_CARRY);
                        }
                        break;

                        default: break;
                    }
                }
            }
            else if ((input.mouse.y >= 58) && (input.mouse.y <= 68) && (input.mouse.x >= 133) && (input.mouse.x <= 186))
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
        }

        return;
    }

    if (!editor.swapChannelFlag && !editor.ui.getLineFlag)
    {
        if (mouseButton == SDL_BUTTON_WHEELUP)
        {
            if (editor.ui.posEdScreenShown)
            {
                if (modEntry->currOrder > 0)
                    modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);
            }
            else if (editor.ui.diskOpScreenShown)
            {
                if (editor.diskop.scrollOffset > 0)
                    editor.diskop.scrollOffset--;

                return;
            }
            else if (!editor.ui.samplerScreenShown && !editor.songPlaying)
            {
                if (modEntry->currRow > 0)
                    modSetPos(DONT_SET_ORDER, modEntry->currRow - 1);
            }
        }
        else if (mouseButton == SDL_BUTTON_WHEELDOWN)
        {
            if (editor.ui.posEdScreenShown)
            {
                if (modEntry->currOrder < (modEntry->head.orderCount - 1))
                    modSetPos(modEntry->currOrder + 1, DONT_SET_ROW);
            }
            else if (editor.ui.diskOpScreenShown)
            {
                if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                {
                    if (editor.diskop.scrollOffset < (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                        editor.diskop.scrollOffset++;
                }

                return;
            }
            else if (!editor.ui.samplerScreenShown && !editor.songPlaying)
            {
                if (modEntry->currRow < MOD_ROWS)
                    modSetPos(DONT_SET_ORDER, modEntry->currRow + 1);
            }
        }

        if (input.mouse.rightButtonPressed)
        {
            guiButton = checkGUIButtons();
            if (guiButton != -1)
            {
                switch (guiButton)
                {
                    case PTB_SA_SAMPLEAREA:
                    {
                        if (!editor.ui.forceSampleEdit)
                        {
                            samplerEditSample(false);
                            return;
                        }
                    }
                    break;

                    default: break;
                }
            }
        }

        if (input.mouse.leftButtonPressed)
        {
            guiButton = checkGUIButtons();
            if (guiButton != -1)
            {
                switch (guiButton)
                {
                    case PTB_DUMMY: break; // for gaps/empty spaces/dummies

                    case PTB_PAT2SMP:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_PAT2SMP;

                        if (input.mouse.rightButtonPressed)
                            editor.pat2SmpHQ = false;
                        else
                            editor.pat2SmpHQ = true;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

                        if (editor.songPlaying)
                            sprintf(pat2SmpText, "ROW 00 TO SMP %02X?", editor.currentSample + 1);
                        else
                            sprintf(pat2SmpText, "ROW %02d TO SMP %02X?", modEntry->currRow, editor.currentSample + 1);

                        setStatusMessage(pat2SmpText, NO_CARRY);
                    }
                    break;

                    // Edit Op. All Screens
                    case PTB_EO_TITLEBAR:
                    {
                             if (editor.ui.editOpScreen == 0) editor.SampleAllFlag ^= 1;
                        else if (editor.ui.editOpScreen == 1) editor.TrackPattFlag  = (editor.TrackPattFlag + 1) % 3;
                        else if (editor.ui.editOpScreen == 2) editor.HalfClipFlag  ^= 1;
                        else if (editor.ui.editOpScreen == 3) editor.NewOldFlag    ^= 1;
                    }
                    break;

                    case PTB_EO_1: editor.ui.editOpScreen = 0; break;
                    case PTB_EO_2: editor.ui.editOpScreen = 1; break;
                    case PTB_EO_3: editor.ui.editOpScreen = 2; break;

                    case PTB_EO_EXIT:
                    {
                        editor.ui.aboutScreenShown  = false;
                        editor.ui.editOpScreenShown = false;
                    }
                    break;
                    // ----------------------------------------------------------

                    // Edit Op. Screen #1
                    case PTB_EO_TRACK_NOTE_UP:   TrackNoteUp(editor.SampleAllFlag,   0, MOD_ROWS - 1, modEntry); break;
                    case PTB_EO_TRACK_NOTE_DOWN: TrackNoteDown(editor.SampleAllFlag, 0, MOD_ROWS - 1, modEntry); break;
                    case PTB_EO_TRACK_OCTA_UP:   TrackOctaUp(editor.SampleAllFlag,   0, MOD_ROWS - 1, modEntry); break;
                    case PTB_EO_TRACK_OCTA_DOWN: TrackOctaDown(editor.SampleAllFlag, 0, MOD_ROWS - 1, modEntry); break;
                    case PTB_EO_PATT_NOTE_UP:    PattNoteUp(editor.SampleAllFlag,   modEntry);                   break;
                    case PTB_EO_PATT_NOTE_DOWN:  PattNoteDown(editor.SampleAllFlag, modEntry);                   break;
                    case PTB_EO_PATT_OCTA_UP:    PattOctaUp(editor.SampleAllFlag,   modEntry);                   break;
                    case PTB_EO_PATT_OCTA_DOWN:  PattOctaDown(editor.SampleAllFlag, modEntry);                   break;
                    // ----------------------------------------------------------

                    // Edit Op. Screen #2
                    case PTB_EO_RECORD: editor.RecordMode ^= 1;                       break;
                    case PTB_EO_DELETE: DelSampleTrack(modEntry);                     break;
                    case PTB_EO_EXCHGE: ExchSampleTrack(modEntry);                    break;
                    case PTB_EO_COPY:   CopySampleTrack(modEntry);                    break;
                    case PTB_EO_FROM:   editor.SampleFrom = editor.currentSample + 1; break;
                    case PTB_EO_TO:     editor.SampleTo   = editor.currentSample + 1; break;

                    case PTB_EO_KILL:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_KILL_SAMPLE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("KILL SAMPLE ?", NO_CARRY);
                    }
                    break;

                    case PTB_EO_QUANTIZE:
                    {
                        editor.ui.tmpDisp16 = editor.QuantizeValue;
                        editor.QuantizeValueDisp = &editor.ui.tmpDisp16;

                        editor.ui.editID      = PTB_EO_QUANTIZE;
                        editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 9;
                        editor.ui.editTextPos = 2824; // (y * 40) + x

                        getNumLine(TEXT_EDIT_DECIMAL);
                    }
                    break;

                    case PTB_EO_METRO_1: // metronome speed
                    {
                        editor.ui.tmpDisp16 = editor.MetroSpeed;
                        editor.MetroSpeedDisp = &editor.ui.tmpDisp16;

                        editor.ui.editID      = PTB_EO_METRO_1;
                        editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 6;
                        editor.ui.editTextPos = 3261; // (y * 40) + x

                        getNumLine(TEXT_EDIT_DECIMAL);
                    }
                    break;

                    case PTB_EO_METRO_2: // metronome channel
                    {
                        editor.ui.tmpDisp16 = editor.MetroChannel;
                        editor.MetroChannelDisp = &editor.ui.tmpDisp16;

                        editor.ui.editID      = PTB_EO_METRO_2;
                        editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 1;
                        editor.ui.editTextPos = 3264; // (y * 40) + x

                        getNumLine(TEXT_EDIT_DECIMAL);
                    }
                    break;

                    case PTB_EO_FROM_NUM:
                    {
                        editor.ui.tmpDisp8 = editor.SampleFrom;
                        editor.SampleFromDisp = &editor.ui.tmpDisp8;

                        editor.ui.editID      = PTB_EO_FROM_NUM;
                        editor.ui.numPtr8     = &editor.ui.tmpDisp8;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 3273; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_EO_TO_NUM:
                    {
                        editor.ui.tmpDisp8 = editor.SampleTo;
                        editor.SampleToDisp = &editor.ui.tmpDisp8;

                        editor.ui.editID      = PTB_EO_TO_NUM;
                        editor.ui.numPtr8     = &editor.ui.tmpDisp8;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 3713; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_EO_FROM_UP:
                    {
                        editor.SampleFrom++;
                        if (editor.SampleFrom > 0x1F)
                            editor.SampleFrom = 0x1F;

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_FROM_DOWN:
                    {
                        editor.SampleFrom--;
                        if (editor.SampleFrom < 0x00)
                            editor.SampleFrom = 0x00;

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_TO_UP:
                    {
                        editor.SampleTo++;
                        if (editor.SampleTo > 0x1F)
                            editor.SampleTo = 0x1F;

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_TO_DOWN:
                    {
                        editor.SampleTo--;
                        if (editor.SampleTo < 0x00)
                            editor.SampleTo = 0x00;

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_KEYS: editor.MultiFlag ^= 1; break;
                    // ----------------------------------------------------------

                    // Edit Op. Screen #3

                    case PTB_EO_MIX:
                    {
                        if (!input.mouse.rightButtonPressed)
                        {
                            editor.MixFlag = true;

                            editor.ui.showTextPtr  = editor.MixText;
                            editor.ui.textEndPtr   = editor.MixText + 15;
                            editor.ui.textLength   = 16;
                            editor.ui.editTextPos  = 1936; // (y * 40) + x
                            editor.ui.dstOffset    = NULL;
                            editor.ui.dstOffsetEnd = false;

                            getTextLine();
                        }
                        else
                        {
                            s = &modEntry->samples[editor.currentSample];

                            if (s->length == 0)
                            {
                                displayErrorMsg("SAMPLE IS EMPTY");
                                break;
                            }

                            if (editor.SamplePos == s->length)
                            {
                                displayErrorMsg("INVALID POS !");
                                break;
                            }

                            ptr8_1 = (int8_t *)(malloc(0x0001FFFE));
                            if (ptr8_1 == NULL)
                            {
                                displayErrorMsg("OUT OF MEMORY !!!");
                                return;
                            }

                            memcpy(ptr8_1, &modEntry->sampleData[s->offset], 0x0001FFFE);

                            ptr8_2 = &modEntry->sampleData[s->offset + editor.SamplePos];
                            ptr8_3 = &modEntry->sampleData[s->offset + s->length];
                            ptr8_4 = ptr8_1;

                            editor.ModulateOffset = 0;
                            editor.ModulatePos    = 0;

                            do
                            {
                                tmpWord = *ptr8_2 + *ptr8_1;
                                tmpWord = (editor.HalfClipFlag == 0) ? (tmpWord / 2) : CLAMP(tmpWord, -128, 127);

                                *ptr8_2++ = (int8_t)(tmpWord);

                                if (editor.ModulateSpeed == 0)
                                {
                                    ptr8_1++;
                                }
                                else
                                {
                                    editor.ModulatePos += editor.ModulateSpeed;

                                    modTmp = (editor.ModulatePos / 4096) & 0x000000FF;
                                    modDat = VibratoTable[modTmp & 0x1F] / 4;
                                    modPos = ((modTmp & 32) ? (editor.ModulateOffset - modDat) : (editor.ModulateOffset + modDat)) + 2048;

                                    editor.ModulateOffset = modPos;

                                    ptr8_1 = &ptr8_4[CLAMP(modPos / 2048, 0, s->length - 1)];
                                }
                            }
                            while (ptr8_2 < ptr8_3);

                            free(ptr8_4);

                            setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        }
                    }
                    break;

                    case PTB_EO_ECHO:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if (editor.SamplePos == 0)
                        {
                            displayErrorMsg("SET SAMPLE POS !");
                            break;
                        }

                        if (editor.SamplePos == s->length)
                        {
                            displayErrorMsg("INVALID POS !");
                            break;
                        }

                        ptr8_1 = &modEntry->sampleData[s->offset + editor.SamplePos];
                        ptr8_2 = &modEntry->sampleData[s->offset];
                        ptr8_3 = ptr8_2;

                        editor.ModulateOffset = 0;
                        editor.ModulatePos    = 0;

                        for (j = 0; j < s->length; ++j)
                        {
                            *ptr8_1 = (*ptr8_2 + *ptr8_1) / 2;

                            ptr8_1++;

                            if (editor.ModulateSpeed == 0)
                            {
                                ptr8_2++;
                            }
                            else
                            {
                                editor.ModulatePos += editor.ModulateSpeed;

                                modTmp = (editor.ModulatePos / 4096) & 0x000000FF;
                                modDat = VibratoTable[modTmp & 0x1F] / 4;
                                modPos = ((modTmp & 32) ? (editor.ModulateOffset - modDat) : (editor.ModulateOffset + modDat)) + 2048;

                                editor.ModulateOffset = modPos;

                                ptr8_2 = &ptr8_3[CLAMP(modPos / 2048, 0, s->length - 1)];
                            }
                        }

                        if (editor.HalfClipFlag != 0)
                        {
                            for (j = 0; j < s->length; ++j)
                                ptr8_3[j] = (int8_t)(CLAMP((int16_t)(ptr8_3[j]) * 2, -128, 127));
                        }

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_POS_NUM:
                    {
                        editor.ui.tmpDisp32 = editor.SamplePos;
                        editor.SamplePosDisp = &editor.ui.tmpDisp32;

                        editor.ui.editID      = PTB_EO_POS_NUM;
                        editor.ui.numPtr32    = &editor.ui.tmpDisp32;
                        editor.ui.numLen      = 5;
                        editor.ui.numBits     = 17;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 2390; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_EO_POS_UP:
                    {
                        edPosUpButton(modEntry, INCREMENT_SLOW);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_POS_DOWN:
                    {
                        edPosDownButton(INCREMENT_SLOW);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_BOOST: // this is actually treble increase
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
                    break;

                    case PTB_EO_FILTER: // this is actually treble decrease
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        FilterSample(modEntry, editor.currentSample);
                        setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    }
                    break;

                    case PTB_EO_MOD:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if (editor.ModulateSpeed == 0)
                        {
                            displayErrorMsg("SET MOD. SPEED !");
                            break;
                        }

                        ptr8_1 = &modEntry->sampleData[s->offset];

                        ptr8_3 = (int8_t *)(malloc(0x0001FFFE));
                        if (ptr8_3 == NULL)
                        {
                            displayErrorMsg("OUT OF MEMORY !!!");
                            return;
                        }

                        ptr8_2 = ptr8_3;

                        memcpy(ptr8_2, ptr8_1, 0x0001FFFE);

                        editor.ModulateOffset = 0;
                        editor.ModulatePos    = 0;

                        for (j = 0; j < s->length; ++j)
                        {
                            *ptr8_1++ = *ptr8_2;

                            if (editor.ModulateSpeed == 0)
                            {
                                ptr8_2++;
                            }
                            else
                            {
                                editor.ModulatePos += editor.ModulateSpeed;

                                modTmp = (editor.ModulatePos / 4096) & 0x000000FF;
                                modDat = VibratoTable[modTmp & 0x1F] / 4;
                                modPos = ((modTmp & 32) ? (editor.ModulateOffset - modDat) : (editor.ModulateOffset + modDat)) + 2048;

                                editor.ModulateOffset = modPos;

                                ptr8_2 = &ptr8_3[CLAMP(modPos / 2048, 0, s->length - 1)];
                            }
                        }

                        free(ptr8_3);

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_MOD_UP:
                    {
                        edModUpButton();
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_MOD_DOWN:
                    {
                        edModDownButton();
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_X_FADE:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        ptr8_1 = &modEntry->sampleData[s->offset];
                        ptr8_2 = &modEntry->sampleData[s->offset + (s->length - 1)];

                        do
                        {
                            tmpWord = *ptr8_1 + *ptr8_2;
                            tmpWord = (editor.HalfClipFlag == 0) ? (tmpWord / 2) : CLAMP(tmpWord, -128, 127);

                            *ptr8_1++ = (int8_t)(tmpWord);
                            *ptr8_2-- = (int8_t)(tmpWord);
                        }
                        while (ptr8_1 < ptr8_2);

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_BACKWD:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
                        {
                            ptr8_1 = &modEntry->sampleData[s->offset + editor.MarkStartOfs];
                            ptr8_2 = &modEntry->sampleData[s->offset + editor.MarkEndOfs];
                        }
                        else
                        {
                            ptr8_1 = &modEntry->sampleData[s->offset];
                            ptr8_2 = &modEntry->sampleData[s->offset + (s->length - 1)];
                        }

                        do
                        {
                            tmpSmp    = *ptr8_1;
                            *ptr8_1++ = *ptr8_2;
                            *ptr8_2-- = tmpSmp;
                        }
                        while (ptr8_1 < ptr8_2);

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_CB:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if (editor.SamplePos == 0)
                        {
                            displayErrorMsg("SET SAMPLE POS !");
                            break;
                        }

                        if (editor.SamplePos >= s->length)
                        {
                            displayErrorMsg("INVALID POS !");
                            break;
                        }

                        mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

                        memcpy(&modEntry->sampleData[s->offset], &modEntry->sampleData[s->offset + editor.SamplePos], 0x0001FFFE - editor.SamplePos);
                        memset(&modEntry->sampleData[s->offset + (0x0001FFFE - editor.SamplePos)], 0, editor.SamplePos);

                        if (editor.SamplePos > s->loopStart)
                        {
                            s->loopStart  = 0;
                            s->loopLength = 2;
                        }
                        else
                        {
                            s->loopStart = (s->loopStart - editor.SamplePos) & 0xFFFFFFFE;
                        }

                        s->length = (s->length - editor.SamplePos) & 0xFFFFFFFE;

                        editor.SamplePos = 0;

                        setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_CHORD: editor.ui.editOpScreen = 3; break;

                    case PTB_EO_FU:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if (editor.SamplePos == 0)
                        {
                            displayErrorMsg("INVALID POS !");
                            break;
                        }

                        tmp    = editor.SamplePos / 2;
                        ptr8_1 = &modEntry->sampleData[s->offset];

                        for (j = 0; j < editor.SamplePos; ++j)
                        {
                            *ptr8_1 = (int8_t)(((int32_t)(*ptr8_1) * (j / 2)) / tmp);
                            ptr8_1++;
                        }

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_FD:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        if (editor.SamplePos >= (s->length - 1))
                        {
                            displayErrorMsg("INVALID POS !");
                            break;
                        }

                        tmp    = (s->length - 1) - editor.SamplePos;
                        ptr8_1 = &modEntry->sampleData[s->offset + (s->length - 1)];

                        for (j = editor.SamplePos; j < s->length; ++j)
                        {
                            *ptr8_1 = (int8_t)(((int32_t)(*ptr8_1) * ((j - editor.SamplePos))) / tmp);
                            ptr8_1--;
                        }

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_UPSAMP:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_UPSAMPLE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("UPSAMPLE ?", NO_CARRY);
                    }
                    break;

                    case PTB_EO_DNSAMP:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_DOWNSAMPLE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("DOWNSAMPLE ?", NO_CARRY);
                    }
                    break;

                    case PTB_EO_VOL_NUM:
                    {
                        editor.ui.tmpDisp16 = editor.SampleVol;
                        editor.SampleVolDisp = &editor.ui.tmpDisp16;

                        editor.ui.editID      = PTB_EO_VOL_NUM;
                        editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                        editor.ui.numLen      = 3;
                        editor.ui.numBits     = 16;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 3711; // (y * 40) + x

                        getNumLine(TEXT_EDIT_DECIMAL);
                    }
                    break;

                    case PTB_EO_VOL:
                    {
                        s = &modEntry->samples[editor.currentSample];

                        if (s->length == 0)
                        {
                            displayErrorMsg("SAMPLE IS EMPTY");
                            break;
                        }

                        ptr8_1 = &modEntry->sampleData[modEntry->samples[editor.currentSample].offset];
                        for (j = 0; j < s->length; ++j)
                        {
                            *ptr8_1 = (int8_t)(CLAMP(((int32_t)(*ptr8_1) * editor.SampleVol) / 100, -128, 127));
                             ptr8_1++;
                        }

                        setupSampler(modEntry, KEEP_SAMPLE_MARKING);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                    break;

                    case PTB_EO_VOL_UP:
                    {
                        edVolUpButton();
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_VOL_DOWN:
                    {
                        edVolDownButton();
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;
                    // ----------------------------------------------------------

                    // Edit Op. Screen #4

                    case PTB_EO_DOCHORD:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_MAKE_CHORD;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("MAKE CHORD?", NO_CARRY);
                    }
                    break;

                    case PTB_EO_MAJOR:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 4;
                        editor.Note3 = editor.Note1 + 7;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;

                        editor.Note4 = 36;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_MAJOR7:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 4;
                        editor.Note3 = editor.Note1 + 7;
                        editor.Note4 = editor.Note1 + 11;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;
                        if (editor.Note4 >= 36) editor.Note4 -= 12;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_NOTE1:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.Note1 = 36;
                        }
                        else
                        {
                            editor.ui.changingChordNote = 1;

                            setStatusMessage("SELECT NOTE", NO_CARRY);
                            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        }
                    }
                    break;

                    case PTB_EO_NOTE1_UP:
                    {
                        edNote1UpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE1_DOWN:
                    {
                        edNote1DownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE2:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.Note2 = 36;
                        }
                        else
                        {
                            editor.ui.changingChordNote = 2;

                            setStatusMessage("SELECT NOTE", NO_CARRY);
                            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        }
                    }
                    break;

                    case PTB_EO_NOTE2_UP:
                    {
                        edNote2UpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE2_DOWN:
                    {
                        edNote2DownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE3:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.Note3 = 36;
                        }
                        else
                        {
                            editor.ui.changingChordNote = 3;

                            setStatusMessage("SELECT NOTE", NO_CARRY);
                            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        }
                    }
                    break;

                    case PTB_EO_NOTE3_UP:
                    {
                        edNote3UpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE3_DOWN:
                    {
                        edNote3DownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE4:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.Note4 = 36;
                        }
                        else
                        {
                            editor.ui.changingChordNote = 4;

                            setStatusMessage("SELECT NOTE", NO_CARRY);
                            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        }
                    }
                    break;

                    case PTB_EO_NOTE4_UP:
                    {
                        edNote4UpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_NOTE4_DOWN:
                    {
                        edNote4DownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_EO_RESET:
                    {
                        editor.Note1 = 36;
                        editor.Note2 = 36;
                        editor.Note3 = 36;
                        editor.Note4 = 36;

                        editor.ChordLengthMin = false;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_MINOR:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 3;
                        editor.Note3 = editor.Note1 + 7;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;

                        editor.Note4 = 36;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_MINOR7:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 3;
                        editor.Note3 = editor.Note1 + 7;
                        editor.Note4 = editor.Note1 + 10;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;
                        if (editor.Note4 >= 36) editor.Note4 -= 12;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_UNDO:
                    {
                        editor.Note1 = editor.OldNote1;
                        editor.Note2 = editor.OldNote2;
                        editor.Note3 = editor.OldNote3;
                        editor.Note4 = editor.OldNote4;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_SUS4:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 5;
                        editor.Note3 = editor.Note1 + 7;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;

                        editor.Note4 = 36;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_MAJOR6:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 4;
                        editor.Note3 = editor.Note1 + 7;
                        editor.Note4 = editor.Note1 + 9;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;
                        if (editor.Note4 >= 36) editor.Note4 -= 12;

                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_LENGTH:
                    {
                        editor.ChordLengthMin = input.mouse.rightButtonPressed ? true : false;
                        recalcChordLength(modEntry);
                    }
                    break;

                    case PTB_EO_MINOR6:
                    {
                        if (editor.Note1 == 36)
                        {
                            displayErrorMsg("NO BASENOTE!");
                            break;
                        }

                        editor.OldNote1 = editor.Note1;
                        editor.OldNote2 = editor.Note2;
                        editor.OldNote3 = editor.Note3;
                        editor.OldNote4 = editor.Note4;

                        editor.Note2 = editor.Note1 + 3;
                        editor.Note3 = editor.Note1 + 7;
                        editor.Note4 = editor.Note1 + 9;

                        if (editor.Note2 >= 36) editor.Note2 -= 12;
                        if (editor.Note3 >= 36) editor.Note3 -= 12;
                        if (editor.Note4 >= 36) editor.Note4 -= 12;

                        recalcChordLength(modEntry);
                    }
                    break;
                    // ----------------------------------------------------------

                    case PTB_ABOUT: editor.ui.aboutScreenShown = true; break;

                    // easter egg stuff :)
                    case PTB_DO_BADGE:
                    {
                        spawnLemming();

                        input.mouse.buttonErrorMsgActive = true;
                        input.mouse.errorMsgCounter      = 32; // short flash

                        setStatusMessage("LEMMINGS!", NO_CARRY);
                    }
                    break;
                    // ----------------------

                    case PTB_PE_PATT:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            editor.ui.tmpDisp16 = modEntry->currOrder;
                            if (editor.ui.tmpDisp16 > (modEntry->head.orderCount - 1))
                                editor.ui.tmpDisp16 =  modEntry->head.orderCount - 1;

                            editor.ui.tmpDisp16 = modEntry->head.order[editor.ui.tmpDisp16];
                            editor.currentPosEdPattDisp = &editor.ui.tmpDisp16;

                            editor.ui.editID      = PTB_PE_PATT;
                            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                            editor.ui.numLen      = 2;
                            editor.ui.numBits     = 8;
                            editor.ui.digitStart  = 5;
                            editor.ui.editTextPos = 2180; // (y * 40) + x

                            getNumLine(TEXT_EDIT_DECIMAL);
                        }
                    }
                    break;

                    case PTB_PE_PATTNAME:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            editor.ui.showTextPtr  = &editor.ui.pattNames[modEntry->head.order[modEntry->currOrder] * 16];
                            editor.ui.textEndPtr   = &editor.ui.pattNames[(modEntry->head.order[modEntry->currOrder] * 16) + 15];
                            editor.ui.textLength   = 15;
                            editor.ui.editTextPos  = 2183; // (y * 40) + x
                            editor.ui.dstOffset    = &editor.textofs.posEdPattName;
                            editor.ui.dstOffsetEnd = false;

                            getTextLine();

                            return; // yes
                        }
                    }
                    break;
                    
                    case PTB_PE_SCROLLTOP:
                    {
                        if (modEntry->currOrder != 0)
                            modSetPos(0, DONT_SET_ROW);
                    }
                    break;

                    case PTB_PE_SCROLLUP:
                    {
                        if (modEntry->currOrder > 0)
                            modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_PE_SCROLLDOWN:
                    {
                        if (modEntry->currOrder < (modEntry->head.orderCount - 1))
                            modSetPos(modEntry->currOrder + 1, DONT_SET_ROW);

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_PE_SCROLLBOT:
                    {
                        if (modEntry->currOrder != (modEntry->head.orderCount - 1))
                            modSetPos(modEntry->head.orderCount - 1, DONT_SET_ROW);
                    }
                    break;

                    case PTB_PE_EXIT:
                    {
                        editor.ui.aboutScreenShown = false;
                        editor.ui.posEdScreenShown = false;
                    }
                    break;

                    case PTB_POS:
                    case PTB_POSED:
                        editor.ui.posEdScreenShown ^= 1;
                    break;

                    case PTB_POSS:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            editor.ui.tmpDisp16 = modEntry->currOrder;
                            editor.currentPosDisp = &editor.ui.tmpDisp16;

                            editor.ui.editID      = PTB_POSS;
                            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                            editor.ui.numLen      = 3;
                            editor.ui.numBits     = 8;
                            editor.ui.digitStart  = 1;
                            editor.ui.editTextPos = 169; // (y * 40) + x

                            getNumLine(TEXT_EDIT_DECIMAL);
                        }
                    }
                    break;

                    case PTB_PATTERNS:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            editor.ui.tmpDisp16 = modEntry->head.order[modEntry->currOrder];
                            editor.currentPatternDisp = &editor.ui.tmpDisp16;

                            editor.ui.editID      = PTB_PATTERNS;
                            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                            editor.ui.numLen      = 2;
                            editor.ui.numBits     = 8;
                            editor.ui.digitStart  = 2;
                            editor.ui.editTextPos = 610; // (y * 40) + x

                            getNumLine(TEXT_EDIT_DECIMAL);
                        }
                    }
                    break;

                    case PTB_LENGTHS:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            editor.ui.tmpDisp16 = modEntry->head.orderCount;
                            editor.currentLengthDisp = &editor.ui.tmpDisp16;

                            editor.ui.editID      = PTB_LENGTHS;
                            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                            editor.ui.numLen      = 3;
                            editor.ui.numBits     = 8;
                            editor.ui.digitStart  = 1;
                            editor.ui.editTextPos = 1049; // (y * 40) + x

                            getNumLine(TEXT_EDIT_DECIMAL);
                        }
                    }
                    break;

                    case PTB_PATTBOX:
                    case PTB_PATTDATA:
                    {
                        if (!editor.ui.introScreenShown)
                        {
                            if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT) || (editor.playMode != PLAY_MODE_NORMAL))
                            {
                                editor.ui.tmpDisp16 = modEntry->currPattern;
                                editor.currentEditPatternDisp = &editor.ui.tmpDisp16;

                                editor.ui.editID      = PTB_PATTDATA;
                                editor.ui.numPtr16    = &editor.ui.tmpDisp16;
                                editor.ui.numLen      = 2;
                                editor.ui.numBits     = 8;
                                editor.ui.digitStart  = 0;
                                editor.ui.editTextPos = 5121; // (y * 40) + x

                                getNumLine(TEXT_EDIT_DECIMAL);
                            }
                        }
                    }
                    break;

                    case PTB_SAMPLES:
                    {
                        editor.SampleZero = false;

                        editor.ui.tmpDisp8 = editor.currentSample;
                        editor.currentSampleDisp = &editor.ui.tmpDisp8;

                        editor.ui.editID      = PTB_SAMPLES;
                        editor.ui.numPtr8     = &editor.ui.tmpDisp8;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 2;
                        editor.ui.editTextPos = 1930; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_SVOLUMES:
                    {
                        editor.ui.tmpDisp8 = modEntry->samples[editor.currentSample].volume;
                        modEntry->samples[editor.currentSample].volumeDisp = &editor.ui.tmpDisp8;

                        editor.ui.editID      = PTB_SVOLUMES;
                        editor.ui.numPtr8     = &editor.ui.tmpDisp8;
                        editor.ui.numLen      = 2;
                        editor.ui.numBits     = 8;
                        editor.ui.digitStart  = 2;
                        editor.ui.editTextPos = 2370; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_SLENGTHS:
                    {
                        editor.ui.tmpDisp32 = modEntry->samples[editor.currentSample].length;
                        modEntry->samples[editor.currentSample].lengthDisp = &editor.ui.tmpDisp32;

                        editor.ui.editID      = PTB_SLENGTHS;
                        editor.ui.numPtr32    = &editor.ui.tmpDisp32;
                        editor.ui.numLen      = 5;
                        editor.ui.numBits     = 17;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 2807; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_SREPEATS:
                    {
                        editor.ui.tmpDisp32 = modEntry->samples[editor.currentSample].loopStart;
                        modEntry->samples[editor.currentSample].loopStartDisp = &editor.ui.tmpDisp32;

                        editor.ui.editID      = PTB_SREPEATS;
                        editor.ui.numPtr32    = &editor.ui.tmpDisp32;
                        editor.ui.numLen      = 5;
                        editor.ui.numBits     = 17;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 3247; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_SREPLENS:
                    {
                        editor.ui.tmpDisp32 = modEntry->samples[editor.currentSample].loopLength;
                        modEntry->samples[editor.currentSample].loopLengthDisp = &editor.ui.tmpDisp32;

                        editor.ui.editID      = PTB_SREPLENS;
                        editor.ui.numPtr32    = &editor.ui.tmpDisp32;
                        editor.ui.numLen      = 5;
                        editor.ui.numBits     = 17;
                        editor.ui.digitStart  = 0;
                        editor.ui.editTextPos = 3687; // (y * 40) + x

                        getNumLine(TEXT_EDIT_HEX);
                    }
                    break;

                    case PTB_EDITOP:
                    {
                        if (editor.ui.editOpScreenShown)
                            editor.ui.editOpScreen = (editor.ui.editOpScreen + 1) % 3;
                        else
                            editor.ui.editOpScreenShown = true;
                    }
                    break;

                    case PTB_DO_LOADMODULE:
                    {
                        editor.diskop.mode         = DISKOP_MODE_MOD;
                        editor.diskop.scrollOffset = 0;
                        editor.diskop.cached       = false;
                    }
                    break;

                    case PTB_DO_LOADSAMPLE:
                    case PTB_LOADSAMPLE:
                    {
                        editor.diskop.mode          = DISKOP_MODE_SMP;
                        editor.diskop.scrollOffset  = 0;
                        editor.diskop.cached        = false;
                        editor.ui.diskOpScreenShown = true;
                    }
                    break;

                    case PTB_DO_SAVESAMPLE:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_SAVE_SAMPLE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("SAVE SAMPLE ?", NO_CARRY);
                    }
                    break;

                    case PTB_MOD2WAV:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_MOD2WAV;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("RENDER WAV FILE?", NO_CARRY);
                    }
                    break;

                    case PTB_SA_RESAMPLENOTE:
                    {
                        editor.ui.changingSmpResample = true;

                        setStatusMessage("SELECT NOTE", NO_CARRY);
                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                    }
                    break;

                    case PTB_SA_RESAMPLE:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_RESAMPLE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("RESAMPLE?", NO_CARRY);
                    }
                    break;

                    case PTB_SA_SAMPLEAREA:
                    {
                        if (editor.ui.sampleMarkingPos == -1)
                        {
                            samplerSamplePressed(MOUSE_BUTTON_NOT_HELD);
                            return;
                        }
                    }
                    break;

                    case PTB_SA_ZOOMBARAREA:
                    {
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat

                        if (!editor.ui.forceSampleDrag)
                        {
                            samplerBarPressed(MOUSE_BUTTON_NOT_HELD);
                            return;
                        }
                    }
                    break;

                    case PTB_SA_FIXDC:         samplerRemoveDcOffset(modEntry);        break;
                    case PTB_SA_INVERT:        samplerInvertSample(modEntry);          break;
                    case PTB_SA_CUT:           samplerSamDelete(modEntry, SAMPLE_CUT); break;
                    case PTB_SA_PASTE:         samplerSamPaste(modEntry);              break;
                    case PTB_SA_COPY:          samplerSamCopy(modEntry);               break;
                    case PTB_SA_LOOP:          samplerLoopToggle();                    break;
                    case PTB_SA_PLAYWAVE:      samplerPlayWaveform(modEntry);          break;
                    case PTB_SA_PLAYDISPLAYED: samplerPlayDisplay(modEntry);           break;
                    case PTB_SA_PLAYRANGE:     samplerPlayRange(modEntry);             break;
                    case PTB_SA_RANGEALL:      samplerRangeAll();                      break;
                    case PTB_SA_SHOWALL:       samplerShowAll();                       break;
                    case PTB_SA_SHOWRANGE:     samplerShowRange();                     break;
                    case PTB_SA_RANGECENTER:   sampleMarkerToCenter();                 break;
                    case PTB_SA_RANGEBEG:      sampleMarkerToBeg();                    break;
                    case PTB_SA_RANGEEND:      sampleMarkerToEnd();                    break;
                    case PTB_SA_ZOOMOUT:       samplerZoomOut();                       break;
                    case PTB_SA_VOLUME:        editor.ui.samplerVolBoxShown = true;    break;

                    case PTB_SA_STOP:
                    {
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
                    break;

                    case PTB_DO_REFRESH:
                    {
                        editor.diskop.scrollOffset = 0;
                        editor.diskop.cached       = false;
                    }
                    break;

                    // TODO: Make a PowerPacker packer and enable this
                    /* case PTB_DO_PACKMOD: editor.diskop.modPackFlg ^= 1; break; */

                    case PTB_DO_SAMPLEFORMAT: editor.diskop.smpSaveType = (editor.diskop.smpSaveType + 1) % 3; break;

                    case PTB_DO_MODARROW:
                    {
                        editor.diskop.mode         = DISKOP_MODE_MOD;
                        editor.diskop.scrollOffset = 0;
                        editor.diskop.cached       = false;
                    }
                    break;

                    case PTB_DO_SAMPLEARROW:
                    {
                        editor.diskop.mode         = DISKOP_MODE_SMP;
                        editor.diskop.scrollOffset = 0;
                        editor.diskop.cached       = false;
                    }
                    break;

                    case PTB_SA_TUNETONE: toggleTuningTone(modEntry); break;

                    case PTB_POSINS:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            if (modEntry->head.orderCount < 127)
                            {
                                tmp = 127 - modEntry->currOrder;
                                for (i = 0; i < tmp; ++i)
                                    modEntry->head.order[127 - i] = modEntry->head.order[126 - i];

                                modEntry->head.order[modEntry->currOrder] = 0;

                                modEntry->head.orderCount++;
                                if (modEntry->currOrder > (modEntry->head.orderCount - 1))
                                    editor.currentPosEdPattDisp = &modEntry->head.order[modEntry->head.orderCount - 1];
                            }

                            calcRealModSize(modEntry);
                            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        }
                    };
                    break;

                    case PTB_POSDEL:
                    {
                        if ((editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_EDIT))
                        {
                            if (modEntry->head.orderCount > 1)
                            {
                                tmp = 126 - modEntry->currOrder;
                                for (i = 0; i < tmp; ++i)
                                    modEntry->head.order[modEntry->currOrder + i] = modEntry->head.order[modEntry->currOrder + 1 + i];

                                modEntry->head.orderCount--;
                                if (modEntry->currOrder > (modEntry->head.orderCount - 1))
                                    editor.currentPosEdPattDisp = &modEntry->head.order[modEntry->head.orderCount - 1];
                            }

                            calcRealModSize(modEntry);
                            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        }
                    }
                    break;

                    case PTB_DO_SAVEMODULE:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_SAVE_MODULE;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("SAVE MODULE ?", NO_CARRY);
                    }
                    break;

                    case PTB_DO_DATAPATH:
                    {
                        editor.ui.showTextPtr  = editor.displayPath;
                        editor.ui.textEndPtr   = editor.displayPath + (PATH_MAX_LEN - 1);
                        editor.ui.textLength   = 26;
                        editor.ui.editTextPos  = 1043; // (y * 40) + x
                        editor.ui.dstOffset    = &editor.textofs.diskOpPath;
                        editor.ui.dstOffsetEnd = false;

                        getTextLine();
                    }
                    break;

                    case PTB_SONGNAME:
                    {
                        editor.ui.showTextPtr  = modEntry->head.moduleTitle;
                        editor.ui.textEndPtr   = modEntry->head.moduleTitle + 19;
                        editor.ui.textLength   = 20;
                        editor.ui.editTextPos  = 4133; // (y * 40) + x
                        editor.ui.dstOffset    = NULL;
                        editor.ui.dstOffsetEnd = false;

                        getTextLine();
                    }
                    break;

                    case PTB_SAMPLENAME:
                    {
                        editor.ui.showTextPtr  = modEntry->samples[editor.currentSample].text;
                        editor.ui.textEndPtr   = modEntry->samples[editor.currentSample].text + 21;
                        editor.ui.textLength   = 22;
                        editor.ui.editTextPos  = 4573; // (y * 40) + x
                        editor.ui.dstOffset    = NULL;
                        editor.ui.dstOffsetEnd = false;

                        getTextLine();
                    }
                    break;

                    case PTB_SUREY:
                    {
                        editor.ui.askScreenShown = false;
                        editor.ui.answerNo       = false;
                        editor.ui.answerYes      = true;

                        handleAskYes(modEntry);
                    }
                    break;

                    case PTB_SUREN:
                    {
                        editor.ui.askScreenShown = false;
                        editor.ui.answerNo       = true;
                        editor.ui.answerYes      = false;

                        handleAskNo(modEntry);
                    }
                    break;

                    case PTB_VISUALS:
                    {
                        if (!editor.ui.introScreenShown)
                        {
                            editor.ui.aboutScreenShown = false;

                            if (editor.ui.visualizerMode == VISUAL_QUADRASCOPE)
                            {
                                editor.ui.visualizerMode = VISUAL_SPECTRUM;
                                memset((int8_t *)(editor.spectrumVolumes), 0, sizeof (editor.spectrumVolumes));
                            }
                            else
                            {
                                editor.ui.visualizerMode = VISUAL_QUADRASCOPE;
                            }
                        }
                    }
                    break;

                    case PTB_QUIT:
                    {
                        editor.ui.askScreenShown = true;
                        editor.ui.askScreenType  = ASK_QUIT;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("REALLY QUIT ?", NO_CARRY);
                    }
                    break;

                    case PTB_CHAN1:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.muted[0] = false;
                            editor.muted[1] = true;
                            editor.muted[2] = true;
                            editor.muted[3] = true;
                        }
                        else
                        {
                            editor.muted[0] ^= 1;
                        }
                    }
                    break;

                    case PTB_CHAN2:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.muted[0] = true;
                            editor.muted[1] = false;
                            editor.muted[2] = true;
                            editor.muted[3] = true;
                        }
                        else
                        {
                            editor.muted[1] ^= 1;
                        }
                    }
                    break;

                    case PTB_CHAN3:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.muted[0] = true;
                            editor.muted[1] = true;
                            editor.muted[2] = false;
                            editor.muted[3] = true;
                        }
                        else
                        {
                            editor.muted[2] ^= 1;
                        }
                    }
                    break;

                    case PTB_CHAN4:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.muted[0] = true;
                            editor.muted[1] = true;
                            editor.muted[2] = true;
                            editor.muted[3] = false;
                        }
                        else
                        {
                            editor.muted[3] ^= 1;
                        }
                    }
                    break;

                    case PTB_SAMPLER:
                    {
                        editor.ui.samplerScreenShown ^= 1;

                        if (editor.ui.samplerScreenShown)
                            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    }
                    break;

                    case PTB_SA_EXIT:     editor.ui.samplerScreenShown = false;               break;
                    case PTB_DO_FILEAREA: diskOpLoadFile((input.mouse.y - 34) / 6, modEntry); break;
                    case PTB_DO_PARENT:   diskOpGoParent();                                   break;

                    case PTB_DISKOP:
                    {
                        editor.ui.diskOpScreenShown  = true;
                        editor.BlockMarkFlag         = false;
                    }
                    break;

                    case PTB_DO_EXIT:
                    {
                        editor.ui.aboutScreenShown  = false;
                        editor.ui.diskOpScreenShown = false;
                        editor.BlockMarkFlag        = false;

                        pointerSetPreviousMode();
                        setPrevStatusMessage();
                    }
                    break;

                    case PTB_DO_SCROLLUP:
                    {
                        if (editor.diskop.scrollOffset > 0)
                            editor.diskop.scrollOffset--;

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_DO_SCROLLTOP:
                    {
                        editor.diskop.scrollOffset = 0;
                    }
                    break;

                    case PTB_DO_SCROLLDOWN:
                    {
                        if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                        {
                            if (editor.diskop.scrollOffset < (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                                editor.diskop.scrollOffset++;
                        }

                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_DO_SCROLLBOT:
                    {
                        if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                            editor.diskop.scrollOffset = editor.diskop.numFiles - DISKOP_LIST_SIZE;
                    }
                    break;

                    case PTB_STOP:
                    {
                        editor.playMode = PLAY_MODE_NORMAL;

                        modStop();

                        editor.currentMode = MODE_IDLE;
                        pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_PLAY:
                    {
                        editor.playMode = PLAY_MODE_NORMAL;

                        if (input.mouse.rightButtonPressed)
                            modPlay(DONT_SET_PATTERN, modEntry->currOrder, modEntry->currRow);
                        else
                            modPlay(DONT_SET_PATTERN, modEntry->currOrder, DONT_SET_ROW);

                        editor.currentMode = MODE_PLAY;
                        pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_PATTERN:
                    {
                        editor.playMode = PLAY_MODE_PATTERN;

                        if (input.mouse.rightButtonPressed)
                            modPlay(modEntry->currPattern, DONT_SET_ORDER, modEntry->currRow);
                        else
                            modPlay(modEntry->currPattern, DONT_SET_ORDER, DONT_SET_ROW);

                        editor.currentMode = MODE_PLAY;
                        pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_EDIT:
                    {
                        if (!editor.ui.samplerScreenShown)
                        {
                            editor.playMode = PLAY_MODE_NORMAL;

                            modStop();

                            editor.currentMode = MODE_EDIT;
                            pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                            setStatusMessage("ALL RIGHT", DO_CARRY);
                        }
                    }
                    break;

                    case PTB_RECORD:
                    {
                        if (!editor.ui.samplerScreenShown)
                        {
                            editor.playMode = PLAY_MODE_PATTERN;

                            if (input.mouse.rightButtonPressed)
                                modPlay(modEntry->currPattern, DONT_SET_ORDER, modEntry->currRow);
                            else
                                modPlay(modEntry->currPattern, DONT_SET_ORDER, DONT_SET_ROW);

                            editor.currentMode = MODE_RECORD;
                            pointerSetMode(POINTER_MODE_EDIT, DO_CARRY);
                            setStatusMessage("ALL RIGHT", DO_CARRY);
                        }
                    }
                    break;

                    case PTB_CLEAR:
                    {
                        editor.ui.clearScreenShown = true;

                        pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
                        setStatusMessage("PLEASE SELECT", NO_CARRY);
                    }
                    break;

                    case PTB_CLEARSONG:
                    {
                        editor.ui.clearScreenShown = false;
                        editor.playMode            = PLAY_MODE_NORMAL;

                        modStop();
                        clearSong();

                        editor.currentMode = MODE_IDLE;
                        pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_CLEARSAMPLES:
                    {
                        editor.ui.clearScreenShown = false;
                        editor.playMode            = PLAY_MODE_NORMAL;

                        modStop();
                        clearSamples();

                        editor.currentMode = MODE_IDLE;
                        pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_CLEARALL:
                    {
                        editor.ui.clearScreenShown = false;
                        editor.playMode            = PLAY_MODE_NORMAL;

                        modStop();
                        clearAll();

                        editor.currentMode = MODE_IDLE;
                        pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                        setStatusMessage("ALL RIGHT", DO_CARRY);
                    }
                    break;

                    case PTB_CLEARCANCEL:
                    {
                        editor.ui.clearScreenShown = false;

                        setPrevStatusMessage();
                        pointerSetPreviousMode();

                        input.mouse.buttonErrorMsgActive = true;
                        input.mouse.buttonErrorMsgBlock  = true;
                        input.mouse.errorMsgCounter      = 0;

                        pointerErrorMode();
                    }
                    break;

                    case PTB_SAMPLEU:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.SampleZero = true;
                        }
                        else
                        {
                            sampleUpButton(modEntry);
                            input.mouse.lastClickedGUIButton = guiButton; // button repeat
                        }
                    }
                    break;

                    case PTB_SAMPLED:
                    {
                        if (input.mouse.rightButtonPressed)
                        {
                            editor.SampleZero = true;
                        }
                        else
                        {
                            sampleDownButton(modEntry);
                            input.mouse.lastClickedGUIButton = guiButton; // button repeat
                        }
                    }
                    break;

                    case PTB_FTUNEU:
                    {
                        sampleFineTuneUpButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_FTUNED:
                    {
                        sampleFineTuneDownButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SVOLUMEU:
                    {
                        sampleVolumeUpButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SVOLUMED:
                    {
                        sampleVolumeDownButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SLENGTHU:
                    {
                        sampleLengthUpButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SLENGTHD:
                    {
                        sampleLengthDownButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SREPEATU:
                    {
                        sampleRepeatUpButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SREPEATD:
                    {
                        sampleRepeatDownButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SREPLENU:
                    {
                        sampleRepeatLengthUpButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_SREPLEND:
                    {
                        sampleRepeatLengthDownButton(modEntry, INCREMENT_SLOW);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_TEMPOU:
                    {
                        tempoUpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_TEMPOD:
                    {
                        tempoDownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_LENGTHU:
                    {
                        songLengthUpButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_LENGTHD:
                    {
                        songLengthDownButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_PATTERNU:
                    {
                        patternUpButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_PATTERND:
                    {
                        patternDownButton(modEntry);
                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_POSU:
                    {
                        positionUpButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    case PTB_POSD:
                    {
                        positionDownButton(modEntry);
                        input.mouse.lastClickedGUIButton = guiButton; // button repeat
                    }
                    break;

                    default: // button not mapped
                    {
                        input.mouse.buttonErrorMsgActive = true;
                        input.mouse.buttonErrorMsgBlock  = true;
                        input.mouse.errorMsgCounter      = 0;

                        pointerErrorMode();
                        setStatusMessage("NOT IMPLEMENTED", NO_CARRY);
                    }
                    break;
                }
            }
        }
    }

    if (editor.ui.getLineFlag)
    {
        if (editor.ui.getLineType != TEXT_EDIT_STRING)
        {
            if (mouseButton == SDL_BUTTON_RIGHT)
                exitGetTextLine(modEntry, EDIT_TEXT_NO_UPDATE);
        }
        else if ((mouseButton == SDL_BUTTON_LEFT) && !editor.MixFlag)
        {
            tmp = input.mouse.y - editor.ui.lineCurY;
            if ((tmp <= 2) && (tmp >= -9))
            {
                tmp = (input.mouse.x - editor.ui.lineCurX) + 4;

                // 68k simulation on signed number: ASR.L #3,D1
                if (tmp < 0)
                    tmp = 0xE0000000 | ((uint32_t)(tmp) >> 3);
                else
                    tmp /= 8;

                while (tmp != 0) // 0 = pos we want
                {
                    if (tmp > 0)
                    {
                        if (editor.ui.editPos < editor.ui.textEndPtr)
                        {
                            if (*editor.ui.editPos != '\0')
                            {
                                editor.ui.editPos++;
                                posMoveRight();
                            }
                        }

                        tmp--;
                    }
                    else if (tmp < 0)
                    {
                        if (editor.ui.editPos > editor.ui.dstPtr)
                        {
                            editor.ui.editPos--;
                            posMoveLeft();
                        }

                        tmp++;
                    }
                }
            }
            else
            {
                exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
            }
        }
        else if (mouseButton == SDL_BUTTON_RIGHT)
        {
            if (editor.MixFlag)
            {
                exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
                editor.MixFlag = false;
            }
            else
            {
                tmpRead = editor.ui.dstPtr;
                while (tmpRead < editor.ui.textEndPtr)
                    *tmpRead++ = '\0';

                *editor.ui.textEndPtr = '\0';

                // don't exit text edit mode if the disk op. path was about to be deleted
                if (editor.ui.editTextPos == 1043)
                {
                    // move text cursor to pos 0
                    while (editor.ui.editPos > editor.ui.dstPtr)
                    {
                        editor.ui.editPos--;
                        posMoveLeft();
                    }
                }
                else
                {
                    exitGetTextLine(modEntry, EDIT_TEXT_UPDATE);
                }
            }
        }
    }
}

void handleMouseButtons(MODULE *modEntry)
{
    if (!input.mouse.leftButtonPressed)
    {
        // left mouse button released, stop repeating buttons
        input.mouse.repeatCounter = 0;
        return;
    }

    if (input.mouse.lastClickedGUIButton != checkGUIButtons())
    {
        // only repeat the button that was first clicked (e.g. if you hold and move mouse to another button)
        input.mouse.repeatCounter = 0;
        return;
    }

    // repeat button
    switch (input.mouse.lastClickedGUIButton)
    {
        case PTB_EO_NOTE1_UP:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote1UpButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE1_DOWN:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote1DownButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE2_UP:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote2UpButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE2_DOWN:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote2DownButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE3_UP:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote3UpButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE3_DOWN:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote3DownButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE4_UP:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote4UpButton(modEntry);
            }
        }
        break;

        case PTB_EO_NOTE4_DOWN:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;
                edNote4DownButton(modEntry);
            }
        }
        break;

        case PTB_EO_VOL_UP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edVolUpButton();
            }
        }
        break;

        case PTB_EO_VOL_DOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edVolDownButton();
            }
        }
        break;

        case PTB_EO_MOD_UP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edModUpButton();
            }
        }
        break;

        case PTB_EO_MOD_DOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edModDownButton();
            }
        }
        break;

        case PTB_EO_POS_UP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edPosUpButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_EO_POS_DOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                edPosDownButton(INCREMENT_FAST);
            }
        }
        break;

        case PTB_EO_FROM_UP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (++editor.SampleFrom > 0x1F)
                      editor.SampleFrom = 0x1F;
            }
        }
        break;

        case PTB_EO_FROM_DOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (--editor.SampleFrom < 0x00)
                      editor.SampleFrom = 0x00;
            }
        }
        break;

        case PTB_EO_TO_UP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (++editor.SampleTo > 0x1F)
                      editor.SampleTo = 0x1F;
            }
        }
        break;

        case PTB_EO_TO_DOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (--editor.SampleTo < 0x00)
                      editor.SampleTo = 0x00;
            }
        }
        break;

        case PTB_SAMPLEU:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;

                if (!input.mouse.rightButtonPressed)
                    sampleUpButton(modEntry);
            }
        }
        break;

        case PTB_SAMPLED:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;

                if (!input.mouse.rightButtonPressed)
                    sampleDownButton(modEntry);
            }
        }
        break;

        case PTB_FTUNEU:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;
                sampleFineTuneUpButton(modEntry);
            }
        }
        break;

        case PTB_FTUNED:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;
                sampleFineTuneDownButton(modEntry);
            }
        }
        break;

        case PTB_SVOLUMEU:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;
                sampleVolumeUpButton(modEntry);
            }
        }
        break;

        case PTB_SVOLUMED:
        {
            if (input.mouse.repeatCounter >= 3)
            {
                input.mouse.repeatCounter = 0;
                sampleVolumeDownButton(modEntry);
            }
        }
        break;

        case PTB_SLENGTHU:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleLengthUpButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_SLENGTHD:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleLengthDownButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_SREPEATU:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleRepeatUpButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_SREPEATD:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleRepeatDownButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_SREPLENU:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleRepeatLengthUpButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_SREPLEND:
        {
            if (input.mouse.rightButtonPressed || (input.mouse.repeatCounter >= 1))
            {
                input.mouse.repeatCounter = 0;
                sampleRepeatLengthDownButton(modEntry, INCREMENT_FAST);
            }
        }
        break;

        case PTB_TEMPOU:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                tempoUpButton(modEntry);
            }
        }
        break;

        case PTB_TEMPOD:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;
                tempoDownButton(modEntry);
            }
        }
        break;

        case PTB_LENGTHU:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                songLengthUpButton(modEntry);
            }
        }
        break;

        case PTB_LENGTHD:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                songLengthDownButton(modEntry);
            }
        }
        break;

        case PTB_PATTERNU:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                patternUpButton(modEntry);
            }
        }
        break;

        case PTB_PATTERND:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                patternDownButton(modEntry);
            }
        }
        break;

        case PTB_POSU:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                positionUpButton(modEntry);
            }
        }
        break;

        case PTB_POSD:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;
                positionDownButton(modEntry);
            }
        }
        break;

        case PTB_PE_SCROLLUP:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;

                if (modEntry->currOrder > 0)
                    modSetPos(modEntry->currOrder - 1, DONT_SET_ROW);
            }
        }
        break;

        case PTB_PE_SCROLLDOWN:
        {
            if (input.mouse.repeatCounter >= 2)
            {
                input.mouse.repeatCounter = 0;

                if (modEntry->currOrder < (modEntry->head.orderCount - 1))
                    modSetPos(modEntry->currOrder + 1, DONT_SET_ROW);
            }
        }
        break;

        case PTB_DO_SCROLLUP:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (editor.diskop.scrollOffset > 0)
                    editor.diskop.scrollOffset--;
            }
        }
        break;

        case PTB_DO_SCROLLDOWN:
        {
            if (input.mouse.repeatCounter >= 1)
            {
                input.mouse.repeatCounter = 0;

                if (editor.diskop.numFiles > DISKOP_LIST_SIZE)
                {
                    if (editor.diskop.scrollOffset < (editor.diskop.numFiles - DISKOP_LIST_SIZE))
                        editor.diskop.scrollOffset++;
                }
            }
        }
        break;

        case PTB_SA_ZOOMBARAREA:
        {
            if (input.mouse.repeatCounter >= 4)
            {
                input.mouse.repeatCounter = 0;

                if (!editor.ui.forceSampleDrag)
                    samplerBarPressed(MOUSE_BUTTON_NOT_HELD);
            }
        }
        break;

        default: break;
    }

    input.mouse.repeatCounter++;
}

void edNote1UpButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note1 += 12;
    else
        editor.Note1++;

    if (editor.Note1 > 36)
        editor.Note1 = 36;

    recalcChordLength(modEntry);
}

void edNote1DownButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note1 -= 12;
    else
        editor.Note1--;

    if (editor.Note1 < 0)
        editor.Note1 = 0;

    recalcChordLength(modEntry);
}

void edNote2UpButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note2 += 12;
    else
        editor.Note2++;

    if (editor.Note2 > 36)
        editor.Note2 = 36;

    recalcChordLength(modEntry);
}

void edNote2DownButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note2 -= 12;
    else
        editor.Note2--;

    if (editor.Note2 < 0)
        editor.Note2 = 0;

    recalcChordLength(modEntry);
}

void edNote3UpButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note3 += 12;
    else
        editor.Note3++;

    if (editor.Note3 > 36)
        editor.Note3 = 36;

    recalcChordLength(modEntry);
}

void edNote3DownButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note3 -= 12;
    else
        editor.Note3--;

    if (editor.Note3 < 0)
        editor.Note3 = 0;

    recalcChordLength(modEntry);
}

void edNote4UpButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note4 += 12;
    else
        editor.Note4++;

    if (editor.Note4 > 36)
        editor.Note4 = 36;

    recalcChordLength(modEntry);
}

void edNote4DownButton(MODULE *modEntry)
{
    if (input.mouse.rightButtonPressed)
        editor.Note4 -= 12;
    else
        editor.Note4--;

    if (editor.Note4 < 0)
        editor.Note4 = 0;

    recalcChordLength(modEntry);
}

void edPosUpButton(MODULE *modEntry, int8_t fast)
{
    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            editor.SamplePos += 544; // 50Hz/60Hz scaled value
        else
            editor.SamplePos += 16;
    }
    else
    {
        if (fast)
            editor.SamplePos += 37; // 50Hz/60Hz scaled value
        else
            editor.SamplePos++;
    }

    if (editor.SamplePos > modEntry->samples[editor.currentSample].length)
        editor.SamplePos = modEntry->samples[editor.currentSample].length;
}

void edPosDownButton(int8_t fast)
{
    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            editor.SamplePos -= 544; // 50Hz/60Hz scaled value
        else
            editor.SamplePos -= 16;
    }
    else
    {
        if (fast)
            editor.SamplePos -= 37; // 50Hz/60Hz scaled value
        else
            editor.SamplePos--;
    }

    if (editor.SamplePos < 0)
        editor.SamplePos = 0;
}

void edModUpButton(void)
{
    if (input.mouse.rightButtonPressed)
        editor.ModulateSpeed += 10;
    else
        editor.ModulateSpeed++;

    if (editor.ModulateSpeed > 127)
        editor.ModulateSpeed = 127;
}

void edModDownButton(void)
{
    if (input.mouse.rightButtonPressed)
        editor.ModulateSpeed -= 10;
    else
        editor.ModulateSpeed--;

    if (editor.ModulateSpeed < -128)
        editor.ModulateSpeed = -128;
}

void edVolUpButton(void)
{
    if (input.mouse.rightButtonPressed)
        editor.SampleVol += 10;
    else
        editor.SampleVol++;

    if (editor.SampleVol > 999)
        editor.SampleVol = 999;
}

void edVolDownButton(void)
{
    if (input.mouse.rightButtonPressed)
        editor.SampleVol -= 10;
    else
        editor.SampleVol--;

    if (editor.SampleVol < 0)
        editor.SampleVol = 0;
}

void sampleUpButton(MODULE *modEntry)
{
    if (editor.SampleZero)
    {
        editor.SampleZero    = false;
        editor.currentSample = 0;
    }
    else if (editor.currentSample < 30)
    {
        editor.currentSample++;
    }

    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    recalcChordLength(modEntry);
}

void sampleDownButton(MODULE *modEntry)
{
    if (!editor.SampleZero && (editor.currentSample > 0))
    {
        editor.currentSample--;

        setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
        recalcChordLength(modEntry);
    }
}

void sampleFineTuneUpButton(MODULE *modEntry)
{
    int8_t finetune;

    finetune = modEntry->samples[editor.currentSample].fineTune & 0x0F;
    if (finetune != 7)
        modEntry->samples[editor.currentSample].fineTune = (finetune + 1) & 0x0F;

    if (input.mouse.rightButtonPressed)
        modEntry->samples[editor.currentSample].fineTune = 0;

    recalcChordLength(modEntry);
}

void sampleFineTuneDownButton(MODULE *modEntry)
{
    int8_t finetune;

    finetune = modEntry->samples[editor.currentSample].fineTune & 0x0F;
    if (finetune != 8)
        modEntry->samples[editor.currentSample].fineTune = (finetune - 1) & 0x0F;

    if (input.mouse.rightButtonPressed)
        modEntry->samples[editor.currentSample].fineTune = 0;

    recalcChordLength(modEntry);
}

void sampleVolumeUpButton(MODULE *modEntry)
{
    int8_t val;

    val = modEntry->samples[editor.currentSample].volume;

    if (input.mouse.rightButtonPressed)
        val += 16;
    else
        val++;

    if (val > 64)
        val = 64;

    modEntry->samples[editor.currentSample].volume = (uint8_t)(val);
}

void sampleVolumeDownButton(MODULE *modEntry)
{
    int8_t val;

    val = modEntry->samples[editor.currentSample].volume;

    if (input.mouse.rightButtonPressed)
        val -= 16;
    else
        val--;

    if (val < 0)
        val = 0;

    modEntry->samples[editor.currentSample].volume = (uint8_t)(val);
}

void sampleLengthUpButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    val = modEntry->samples[editor.currentSample].length;

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val += 64;
        else
            val += 16;
    }
    else
    {
        if (fast)
            val += 10;
        else
            val += 2;
    }

    if (val > 0x0001FFFE)
        val = 0x0001FFFE;

    modEntry->samples[editor.currentSample].length = val;
}

void sampleLengthDownButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;

    MODULE_SAMPLE *s;

    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

    val = modEntry->samples[editor.currentSample].length;

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val -= 64;
        else
            val -= 16;
    }
    else
    {
        if (fast)
            val -= 10;
        else
            val -= 2;
    }

    if (val < 0)
        val = 0;

    s = &modEntry->samples[editor.currentSample];

    s->length = val;
    if (s->loopLength > 2)
    {
        if (s->length < (s->loopStart + s->loopLength))
            s->length =  s->loopStart + s->loopLength;
    }
}

void sampleRepeatUpButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;
    int32_t loopLen;
    int32_t len;

    val     = modEntry->samples[editor.currentSample].loopStart;
    loopLen = modEntry->samples[editor.currentSample].loopLength;
    len     = modEntry->samples[editor.currentSample].length;

    if (len == 0)
    {
        modEntry->samples[editor.currentSample].loopStart = 0;

        return;
    }

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val += 64;
        else
            val += 16;
    }
    else
    {
        if (fast)
            val += 10;
        else
            val += 2;
    }

    if (val > (len - loopLen))
        val =  len - loopLen;

    modEntry->samples[editor.currentSample].loopStart = val;

    UpdateVoiceParameters(modEntry);
    SetLoopSprites();
}

void sampleRepeatDownButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;
    int32_t len;

    val = modEntry->samples[editor.currentSample].loopStart;
    len = modEntry->samples[editor.currentSample].length;

    if (len == 0)
    {
        modEntry->samples[editor.currentSample].loopStart = 0;

        return;
    }

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val -= 64;
        else
            val -= 16;
    }
    else
    {
        if (fast)
            val -= 10;
        else
            val -= 2;
    }

    if (val < 0)
        val = 0;

    modEntry->samples[editor.currentSample].loopStart = val;

    UpdateVoiceParameters(modEntry);
    SetLoopSprites();
}

void sampleRepeatLengthUpButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;
    int32_t loopStart;
    int32_t len;

    val       = modEntry->samples[editor.currentSample].loopLength;
    loopStart = modEntry->samples[editor.currentSample].loopStart;
    len       = modEntry->samples[editor.currentSample].length;

    if (len == 0)
    {
        modEntry->samples[editor.currentSample].loopLength = 2;

        return;
    }

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val += 64;
        else
            val += 16;
    }
    else
    {
        if (fast)
            val += 10;
        else
            val += 2;
    }

    if (val > (len - loopStart))
        val =  len - loopStart;

    modEntry->samples[editor.currentSample].loopLength = val;

    UpdateVoiceParameters(modEntry);
    SetLoopSprites();
}

void sampleRepeatLengthDownButton(MODULE *modEntry, int8_t fast)
{
    int32_t val;
    int32_t len;

    val = modEntry->samples[editor.currentSample].loopLength;
    len = modEntry->samples[editor.currentSample].length;

    if (len == 0)
    {
        modEntry->samples[editor.currentSample].loopLength = 2;
        return;
    }

    if (input.mouse.rightButtonPressed)
    {
        if (fast)
            val -= 64;
        else
            val -= 16;
    }
    else
    {
        if (fast)
            val -= 10;
        else
            val -= 2;
    }

    if (val < 2)
        val = 2;

    modEntry->samples[editor.currentSample].loopLength = val;

    UpdateVoiceParameters(modEntry);
    SetLoopSprites();
}

void tempoUpButton(MODULE *modEntry)
{
    int16_t val;

    if (editor.timingMode == TEMPO_MODE_VBLANK)
        return;

    val = modEntry->currBPM;

    if (input.mouse.rightButtonPressed)
        val += 10;
    else
        val++;

    if (val > 255)
        val = 255;

    modEntry->currBPM = val;
    modSetTempo(modEntry->currBPM);
}

void tempoDownButton(MODULE *modEntry)
{
    int16_t val;

    if (editor.timingMode == TEMPO_MODE_VBLANK)
        return;

    val = modEntry->currBPM;

    if (input.mouse.rightButtonPressed)
        val -= 10;
    else
        val--;

    if (val < 32)
        val = 32;

    modEntry->currBPM = val;
    modSetTempo(modEntry->currBPM);
}

void songLengthUpButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->head.orderCount;

    if (input.mouse.rightButtonPressed)
        val += 10;
    else
        val++;

    if (val > 127)
        val = 127;

    modEntry->head.orderCount = (uint8_t)(val);

    val = modEntry->currOrder;
    if (val > (modEntry->head.orderCount - 1))
        val =  modEntry->head.orderCount - 1;

    editor.currentPosEdPattDisp = &modEntry->head.order[val];
}

void songLengthDownButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->head.orderCount;

    if (input.mouse.rightButtonPressed)
        val -= 10;
    else
        val--;

    if (val < 1)
        val = 1;

    modEntry->head.orderCount = (uint8_t)(val);

    val = modEntry->currOrder;
    if (val > (modEntry->head.orderCount - 1))
        val =  modEntry->head.orderCount - 1;

    editor.currentPosEdPattDisp = &modEntry->head.order[val];
}

void patternUpButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->head.order[modEntry->currOrder];

    if (input.mouse.rightButtonPressed)
        val += 10;
    else
        val++;

    if (val > (MAX_PATTERNS - 1))
        val =  MAX_PATTERNS - 1;

    modEntry->head.order[modEntry->currOrder] = (uint8_t)(val);
}

void patternDownButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->head.order[modEntry->currOrder];

    if (input.mouse.rightButtonPressed)
        val -= 10;
    else
        val--;

    if (val < 0)
        val = 0;

    modEntry->head.order[modEntry->currOrder] = (uint8_t)(val);
}

void positionUpButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->currOrder;

    if (input.mouse.rightButtonPressed)
        val += 10;
    else
        val++;

    if (val > 126)
        val = 126;

    modSetPos(val, DONT_SET_ROW);
}

void positionDownButton(MODULE *modEntry)
{
    int16_t val;

    val = modEntry->currOrder;

    if (input.mouse.rightButtonPressed)
        val -= 10;
    else
        val--;

    if (val < 0)
        val = 0;

    modSetPos(val, DONT_SET_ROW);
}

void diskOpLoadFile(uint32_t fileEntryRow, MODULE *modEntry)
{
    char *dirPath;
    char *filePath;

    MODULE *newMod;

    if (diskOpEntryIsDir(fileEntryRow))
    {
        dirPath = diskOpGetEntryPath(fileEntryRow);
        if (dirPath != NULL)
        {
            if (dirCanBeOpened(dirPath))
                diskOpSetPath(dirPath, DISKOP_CACHE);
            else
                displayErrorMsg("CAN'T OPEN DIR !");
        }
    }
    else
    {
        filePath = diskOpGetEntryPath(fileEntryRow);
        if (filePath != NULL)
        {
            if (editor.diskop.mode == DISKOP_MODE_MOD)
            {
                newMod = modLoad(filePath);
                if (newMod != NULL)
                {
                    editor.ui.diskOpScreenShown = false;

                    modStop();

                    modEntry->moduleLoaded = false;
                    modFree(modEntry);

                    modEntry = newMod;
                    setPlayerModSource(modEntry);
                    setMainModSource(modEntry);

                    modSetSpeed(6);

                    if (modEntry->head.initBPM > 0)
                        modSetTempo(modEntry->head.initBPM);
                    else
                        modSetTempo(125);

                    editor.oldBPM = modEntry->currBPM;

                    modSetPos(0, 0);

                    modEntry->moduleLoaded = true;

                    editor.currentMode = MODE_IDLE;
                    pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                    setStatusMessage("ALL RIGHT", DO_CARRY);
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
            else if (editor.diskop.mode == DISKOP_MODE_SMP)
            {
                loadSample(modEntry, filePath, diskOpGetEntry(fileEntryRow));
            }
        }
    }
}

void handleSamplerVolumeBox(MODULE *modEntry)
{
    int8_t *sampleData;

    int16_t sample;
    int16_t sampleVol;

    int32_t sampleIndex;
    int32_t sampleLength;
    int32_t tmpSample;

    MODULE_SAMPLE *s;

    if (input.mouse.rightButtonPressed)
    {
        if (editor.ui.getLineFlag)
            exitGetTextLine(modEntry, EDIT_TEXT_NO_UPDATE);
        else
            editor.ui.samplerVolBoxShown = false;

        return;
    }

    if (editor.ui.getLineFlag)
        return;

    // check buttons
    if (input.mouse.leftButtonPressed)
    {
        // DRAG BOXES
        if ((input.mouse.x >=  72) && (input.mouse.x <= 173) &&
            (input.mouse.y >= 154) && (input.mouse.y <= 175))
        {
            volBoxBarPressed(MOUSE_BUTTON_NOT_HELD);
            return;
        }

        // FROM NUM
        if ((input.mouse.x >= 174) && (input.mouse.x <= 207) &&
            (input.mouse.y >= 154) && (input.mouse.y <= 164))
        {
            editor.ui.tmpDisp16 = editor.Vol1;
            editor.Vol1Disp = &editor.ui.tmpDisp16;

            editor.ui.editID      = PTB_SA_VOL_FROM_NUM;
            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
            editor.ui.numLen      = 3;
            editor.ui.numBits     = 8;
            editor.ui.digitStart  = 0;
            editor.ui.editTextPos = 6342; // (y * 40) + x

            getNumLine(TEXT_EDIT_DECIMAL);

            return;
        }

        // TO NUM
        if ((input.mouse.x >= 174) && (input.mouse.x <= 207) &&
            (input.mouse.y >= 165) && (input.mouse.y <= 175))
        {
            editor.ui.tmpDisp16 = editor.Vol2;
            editor.Vol2Disp = &editor.ui.tmpDisp16;

            editor.ui.editID      = PTB_SA_VOL_TO_NUM;
            editor.ui.numPtr16    = &editor.ui.tmpDisp16;
            editor.ui.numLen      = 3;
            editor.ui.numBits     = 8;
            editor.ui.digitStart  = 0;
            editor.ui.editTextPos = 6782; // (y * 40) + x

            getNumLine(TEXT_EDIT_DECIMAL);

            return;
        }

        // NORMALIZE
        if ((input.mouse.x >= 101) && (input.mouse.x <= 143) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            s = &modEntry->samples[editor.currentSample];
            if (s->length == 0)
            {
                displayErrorMsg("SAMPLE IS EMPTY");
                return;
            }

            sampleData = &modEntry->sampleData[s->offset];

            if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
            {
                sampleData  += editor.MarkStartOfs;
                sampleLength = (editor.MarkEndOfs - editor.MarkStartOfs) + 1;
            }
            else
            {
                sampleLength = s->length;
            }

            sampleVol   = 0;
            sampleIndex = 0;

            while (sampleIndex < sampleLength)
            {
                sample = *sampleData++;

                if (sample < 0)
                    sample = 0 - sample; // abs()

                if (sampleVol < sample)
                    sampleVol = sample;

                sampleIndex++;
            }

            if ((sampleVol <= 0) || (sampleVol > 127))
            {
                editor.Vol1 = 100;
                editor.Vol2 = 100;
            }
            else if (sampleVol < 64)
            {
                editor.Vol1 = 200;
                editor.Vol2 = 200;
            }
            else
            {
                editor.Vol1 = 12700 / sampleVol;
                editor.Vol2 = 12700 / sampleVol;
            }

            return;
        }

        // RAMP DOWN
        if ((input.mouse.x >= 144) && (input.mouse.x <= 153) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            editor.Vol1 = 100;
            editor.Vol2 = 0;

            return;
        }

        // RAMP UP
        if ((input.mouse.x >= 154) && (input.mouse.x <= 163) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            editor.Vol1 = 0;
            editor.Vol2 = 100;

            return;
        }

        // RAMP UNITY
        if ((input.mouse.x >= 164) && (input.mouse.x <= 173) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            editor.Vol1 = 100;
            editor.Vol2 = 100;

            return;
        }

        // CANCEL
        if ((input.mouse.x >= 174) && (input.mouse.x <= 207) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            editor.ui.samplerVolBoxShown = false;
            return;
        }

        // RAMP
        if ((input.mouse.x >=  72) && (input.mouse.x <= 100) &&
            (input.mouse.y >= 176) && (input.mouse.y <= 186))
        {
            s = &modEntry->samples[editor.currentSample];
            if (s->length == 0)
            {
                displayErrorMsg("SAMPLE IS EMPTY");
                return;
            }

            if ((editor.Vol1 == 100) && (editor.Vol2 == 100))
            {
                editor.ui.samplerVolBoxShown = false;
                return;
            }

            sampleData = &modEntry->sampleData[s->offset];

            if ((editor.MarkStartOfs >= 0) && ((editor.MarkEndOfs - editor.MarkStartOfs) != 0))
            {
                sampleData  += editor.MarkStartOfs;
                sampleLength = (editor.MarkEndOfs - editor.MarkStartOfs) + 1;
            }
            else
            {
                sampleLength = s->length;
            }

            sampleIndex = 0;
            while (sampleIndex < sampleLength)
            {
                tmpSample = ((sampleIndex * editor.Vol2) / sampleLength) + ((sampleLength - sampleIndex) * editor.Vol1) / sampleLength;
                tmpSample = (*sampleData * tmpSample) / 100;

                *sampleData++ = (int8_t)(CLAMP(tmpSample, -128, 127));
                sampleIndex++;
            }

            editor.ui.samplerVolBoxShown = false;

            UpdateSample();
            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return;
        }
    }
}
