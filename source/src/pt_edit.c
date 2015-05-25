/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#include <SDL/SDL.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif
#include "pt_header.h"
#include "pt_helpers.h"
#include "pt_tables.h"
#include "pt_audio.h"
#include "pt_helpers.h"
#include "pt_palette.h"
#include "pt_diskop.h"
#include "pt_buttons.h"
#include "pt_sampler.h"
#include "pt_visuals.h"

void setPattern(int16_t pattern); // pt_modplayer.c

void jamAndPlaceSample(SDLKey keyEntry, MODULE *modEntry, int8_t normalModes);
uint8_t QuantizeCheck(mod_channel *ch, uint8_t row);
uint8_t handleSpecialKeys(SDLKey keyEntry, MODULE *modEntry);
int8_t keyToNote(SDLKey keyEntry);

void exitGetTextLine(MODULE *modEntry, uint8_t updateValue)
{
    uint8_t i;

    int16_t posEdPos;

    MODULE_SAMPLE *s;

    // if user updated the disk op path text
    if (editor.ui.diskOpScreenShown && (editor.ui.editTextPos == 1043))
    {
        if (dirCanBeOpened(editor.displayPath))
        {
            diskOpSetPath(editor.displayPath, DISKOP_CACHE);
        }
        else
        {
            memset(editor.tempPath,    0, PATH_MAX_LEN + 1); // must be wiped in case getcwd() doesn't insert null termination
            memset(editor.displayPath, 0, PATH_MAX_LEN + 1); // must be wiped because of display method

            getcwd(editor.tempPath, PATH_MAX_LEN);
            strcpy(editor.displayPath, editor.tempPath);

            displayErrorMsg("CAN'T OPEN DIR !");
        }
    }

    editor.ui.getLineFlag = false;
    editor.ui.lineCurX    = 0;
    editor.ui.lineCurY    = 0;
    editor.ui.editPos     = NULL;
    editor.ui.dstPos      = 0;

    if (editor.ui.getLineType == TEXT_EDIT_STRING)
    {
        if (editor.ui.dstOffset != NULL)
            *editor.ui.dstOffset = '\0';

        if (editor.ui.editTextPos == 4133) // module name
        {
            // A..Z -> a..z
            for (i = 0; i < 20; ++i)
            {
                if ((modEntry->head.moduleTitle[i] >= 'A') && (modEntry->head.moduleTitle[i] <= 'Z'))
                     modEntry->head.moduleTitle[i] += 32;
            }
        }

        if (editor.ui.editTextPos != 1043) // special case for disk op. right mouse button
        {
            pointerSetPreviousMode();

            if (!editor.MixFlag)
                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
        }
    }
    else
    {
        // set back display pointers and update values (if requested)

        s = &modEntry->samples[editor.currentSample];
        switch (editor.ui.editID)
        {
            case PTB_SA_VOL_FROM_NUM:
            {
                editor.Vol1Disp = &editor.Vol1;

                if (updateValue)
                {
                    editor.Vol1 = editor.ui.tmpDisp16;
                    if (editor.Vol1 > 200)
                        editor.Vol1 = 200;
                }
            }
            break;

            case PTB_SA_VOL_TO_NUM:
            {
                editor.Vol2Disp = &editor.Vol2;

                if (updateValue)
                {
                    editor.Vol2 = editor.ui.tmpDisp16;
                    if (editor.Vol2 > 200)
                        editor.Vol2 = 200;
                }
            }
            break;

            case PTB_EO_VOL_NUM:
            {
                editor.SampleVolDisp = &editor.SampleVol;

                if (updateValue)
                    editor.SampleVol = editor.ui.tmpDisp16;
            }
            break;

            case PTB_EO_POS_NUM:
            {
                editor.SamplePosDisp = &editor.SamplePos;

                if (updateValue)
                {
                    editor.SamplePos = editor.ui.tmpDisp32;
                    if (editor.SamplePos > modEntry->samples[editor.currentSample].length)
                        editor.SamplePos = modEntry->samples[editor.currentSample].length;
                }
            }
            break;

            case PTB_EO_QUANTIZE:
            {
                editor.QuantizeValueDisp = &editor.QuantizeValue;

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > 63)
                        editor.ui.tmpDisp16 = 63;

                    editor.QuantizeValue = editor.ui.tmpDisp16;
                }
            }
            break;

            case PTB_EO_METRO_1: // metronome speed
            {
                editor.MetroSpeedDisp = &editor.MetroSpeed;

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > 64)
                        editor.ui.tmpDisp16 = 64;

                    editor.MetroSpeed = editor.ui.tmpDisp16;
                }
            }
            break;

            case PTB_EO_METRO_2: // metronome channel
            {
                editor.MetroChannelDisp = &editor.MetroChannel;

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > 4)
                        editor.ui.tmpDisp16 = 4;

                    editor.MetroChannel = editor.ui.tmpDisp16;
                }
            }
            break;

            case PTB_EO_FROM_NUM:
            {
                editor.SampleFromDisp = &editor.SampleFrom;

                if (updateValue)
                {
                    editor.SampleFrom = editor.ui.tmpDisp8;

                    // signed check + normal check
                    if ((editor.SampleFrom < 0x00) || (editor.SampleFrom > 0x1F))
                        editor.SampleFrom = 0x1F;
                }
            }
            break;

            case PTB_EO_TO_NUM:
            {
                editor.SampleToDisp = &editor.SampleTo;

                if (updateValue)
                {
                    editor.SampleTo = editor.ui.tmpDisp8;

                    // signed check + normal check
                    if ((editor.SampleTo < 0x00) || (editor.SampleTo > 0x1F))
                        editor.SampleTo = 0x1F;
                }
            }
            break;

            case PTB_PE_PATT:
            {
                posEdPos = modEntry->currOrder;
                if (posEdPos > (modEntry->head.orderCount - 1))
                    posEdPos =  modEntry->head.orderCount - 1;

                editor.currentPosEdPattDisp = &modEntry->head.order[posEdPos];

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > (MAX_PATTERNS - 1))
                        editor.ui.tmpDisp16 =  MAX_PATTERNS - 1;

                    modEntry->head.order[posEdPos] = editor.ui.tmpDisp16;

                    calcRealModSize(modEntry);
                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_POSS:
            {
                editor.currentPosDisp = &modEntry->currOrder;

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > 126)
                        editor.ui.tmpDisp16 = 126;

                    modEntry->currOrder = editor.ui.tmpDisp16;
                }
            }
            break;

            case PTB_PATTERNS:
            {
                editor.currentPatternDisp = &modEntry->head.order[modEntry->currOrder];

                if (updateValue)
                {
                    if (editor.ui.tmpDisp16 > (MAX_PATTERNS - 1))
                        editor.ui.tmpDisp16 =  MAX_PATTERNS - 1;

                    modEntry->head.order[modEntry->currOrder] = editor.ui.tmpDisp16;

                    calcRealModSize(modEntry);
                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_LENGTHS:
            {
                editor.currentLengthDisp = &modEntry->head.orderCount;

                if (updateValue)
                {
                    modEntry->head.orderCount = CLAMP(editor.ui.tmpDisp16, 1, 127);

                    posEdPos = modEntry->currOrder;
                    if (posEdPos > (modEntry->head.orderCount - 1))
                        posEdPos =  modEntry->head.orderCount - 1;

                    editor.currentPosEdPattDisp = &modEntry->head.order[posEdPos];

                    calcRealModSize(modEntry);
                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_PATTDATA:
            {
                editor.currentEditPatternDisp = &modEntry->currPattern;

                if (updateValue)
                    setPattern(editor.ui.tmpDisp16);
            }
            break;

            case PTB_SAMPLES:
            {
                editor.currentSampleDisp = &editor.currentSample;

                if (updateValue)
                {
                    editor.currentSample = editor.ui.tmpDisp8;

                    if (editor.currentSample < 0x00) // (signed) if >0x7F was entered, clamp to 0x1F
                        editor.currentSample = 0x1F;

                    editor.currentSample = CLAMP(editor.currentSample, 0x01, 0x1F) - 1;

                    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    recalcChordLength(modEntry);
                }
            }
            break;

            case PTB_SVOLUMES:
            {
                s->volumeDisp = &s->volume;

                if (updateValue)
                {
                    s->volume = editor.ui.tmpDisp8;

                    // signed check + normal check
                    if ((s->volume < 0x00) || (s->volume > 0x40))
                        s->volume = 0x40;

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_SLENGTHS:
            {
                s->lengthDisp = &s->length;

                if (updateValue)
                {
                    mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

                    s->length = editor.ui.tmpDisp32 & 0xFFFFFFFE; // even'ify

                    if (s->length > 0x0001FFFE)
                        s->length = 0x0001FFFE;

                    if (s->loopLength > 2)
                    {
                        if (s->length < (s->loopStart + s->loopLength))
                            s->length =  s->loopStart + s->loopLength;
                    }

                    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    UpdateVoiceParameters(modEntry);
                    calcRealModSize(modEntry);
                    recalcChordLength(modEntry);

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_SREPEATS:
            {
                s->loopStartDisp = &s->loopStart;

                if (updateValue)
                {
                    s->loopStart = editor.ui.tmpDisp32 & 0xFFFFFFFE; // even'ify

                    if (s->loopStart > 0x0001FFFE)
                        s->loopStart = 0x0001FFFE;

                    if (s->length >= s->loopLength)
                    {
                        if ((s->loopStart + s->loopLength) > s->length)
                            s->loopStart = s->length - s->loopLength;
                    }
                    else
                    {
                        s->loopStart = 0;
                    }

                    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    UpdateVoiceParameters(modEntry);

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            case PTB_SREPLENS:
            {
                s->loopLengthDisp = &s->loopLength;

                if (updateValue)
                {
                    s->loopLength = editor.ui.tmpDisp32 & 0xFFFFFFFE; // even'ify

                    if (s->loopLength > 0x0001FFFE)
                        s->loopLength = 0x0001FFFE;

                    if (s->length >= s->loopStart)
                    {
                        if ((s->loopStart + s->loopLength) > s->length)
                            s->loopLength = s->length - s->loopStart;
                    }
                    else
                    {
                        s->loopLength = 2;
                    }

                    if (s->loopLength < 2)
                        s->loopLength = 2;

                    setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
                    UpdateVoiceParameters(modEntry);

                    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                }
            }
            break;

            default: break;
        }

        pointerSetPreviousMode();
    }

    editor.ui.getLineType = 0;
}

void getTextLine(void)
{
    pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

    editor.ui.lineCurY    =  (editor.ui.editTextPos / 40) + 5;
    editor.ui.lineCurX    = ((editor.ui.editTextPos % 40) * FONT_CHAR_W) + 4;
    editor.ui.dstPtr      = editor.ui.showTextPtr;
    editor.ui.editPos     = editor.ui.showTextPtr;
    editor.ui.dstPos      = 0;
    editor.ui.getLineFlag = true;
    editor.ui.getLineType = TEXT_EDIT_STRING;

    if (editor.ui.dstOffset != NULL)
        *editor.ui.dstOffset = '\0';

    if (editor.MixFlag)
    {
        moveCharRight();
        moveCharRight();
        moveCharRight();
        moveCharRight();
    }
}

void getNumLine(uint8_t type)
{
    pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);

    editor.ui.lineCurY    =  (editor.ui.editTextPos / 40) + 5;
    editor.ui.lineCurX    = ((editor.ui.editTextPos % 40) * FONT_CHAR_W) + 4;
    editor.ui.dstPos      = 0;
    editor.ui.getLineFlag = true;
    editor.ui.getLineType = type;
}

void handleEditKeys(SDLKey keyEntry, MODULE *modEntry, int8_t normalMode)
{
    int8_t key;
    int8_t hexKey;
    int8_t numberKey;

    modnote_t *note;

    if (editor.ui.getLineFlag)
        return;

    if (editor.ui.samplerScreenShown || (editor.currentMode == MODE_IDLE) || (editor.currentMode == MODE_PLAY))
    {
        // it will only jam, not place it
        jamAndPlaceSample(keyEntry, modEntry, normalMode);
        return;
    }

    if ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD))
    {
        if (handleSpecialKeys(keyEntry, modEntry))
        {
            if (editor.currentMode != MODE_RECORD)
                modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 63);

            return;
        }
    }

    // are we editing a note, or other stuff?
    if (editor.cursor.mode != CURSOR_NOTE)
    {
        if ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD))
        {
            numberKey = ((keyEntry > 47) && (keyEntry <  58)) ? ((int8_t)(keyEntry) - 48) : -1;
            hexKey    = ((keyEntry > 96) && (keyEntry < 103)) ? ((int8_t)(keyEntry) - 87) : -1;
            key       = -1;

            if (numberKey != -1)
            {
                if (key == -1)
                    key = 0;

                key += numberKey;
            }

            if (hexKey != -1)
            {
                if (key == -1)
                    key = 0;

                key += hexKey;
            }

            note = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];

            switch (editor.cursor.mode)
            {
                case CURSOR_SAMPLE1:
                {
                    if ((key != -1) && (key < 2))
                    {
                        note->sample = (uint8_t)((note->sample % 0x10) | (key << 4));

                        if (editor.currentMode != MODE_RECORD)
                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                }
                break;

                case CURSOR_SAMPLE2:
                {
                    if ((key != -1) && (key < 16))
                    {
                        note->sample = (uint8_t)((note->sample & 16) | key);

                        if (editor.currentMode != MODE_RECORD)
                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                }
                break;

                case CURSOR_CMD:
                {
                    if ((key != -1) && (key < 16))
                    {
                        note->command = (uint8_t)(key);

                        if (editor.currentMode != MODE_RECORD)
                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                }
                break;

                case CURSOR_PARAM1:
                {
                    if ((key != -1) && (key < 16))
                    {
                        note->param = (uint8_t)((note->param % 0x10) | (key << 4));

                        if (editor.currentMode != MODE_RECORD)
                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                }
                break;

                case CURSOR_PARAM2:
                {
                    if ((key != -1) && (key < 16))
                    {
                        note->param = (uint8_t)((note->param & 0xF0) | key);

                        if (editor.currentMode != MODE_RECORD)
                            modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                        updateWindowTitle(modEntry, MOD_IS_MODIFIED);
                    }
                }
                break;

                default: break;
            }
        }
    }
    else
    {
        if (keyEntry == SDLK_DELETE)
        {
            if ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD))
            {
                note = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];

                if (!input.keyb.altKeyDown)
                {
                    note->sample = 0;
                    note->period = 0;
                }

                if (input.keyb.shiftKeyDown || input.keyb.altKeyDown)
                {
                    note->command = 0;
                    note->param   = 0;
                }

                if (editor.currentMode != MODE_RECORD)
                    modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 63);

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
        }
        else
        {
            jamAndPlaceSample(keyEntry, modEntry, normalMode);
        }
    }
}

uint8_t handleSpecialKeys(SDLKey keyEntry, MODULE *modEntry)
{
    modnote_t *note;
    modnote_t *prevNote;

    if (input.keyb.altKeyDown)
    {
        note     = &modEntry->patterns[modEntry->currPattern][(modEntry->currRow * PAULA_CHANNELS) + editor.cursor.channel];
        prevNote = &modEntry->patterns[modEntry->currPattern][(((modEntry->currRow - 1) & 0x3F) * PAULA_CHANNELS) + editor.cursor.channel];

        if ((keyEntry >= SDLK_1) && (keyEntry <= SDLK_9))
        {
            // insert stored effect (buffer[0..8])
            note->command = editor.EffectMacros[keyEntry - SDLK_1] >> 8;
            note->param   = editor.EffectMacros[keyEntry - SDLK_1] & 0xFF;

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return (true);
        }
        else if (keyEntry == SDLK_0)
        {
            // insert stored effect (buffer[9])
            note->command = editor.EffectMacros[9] >> 8;
            note->param   = editor.EffectMacros[9] & 0xFF;

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return (true);
        }

        // copy command+effect from above into current command+effect
        if (keyEntry == SDLK_BACKSLASH)
        {
            note->command = prevNote->command;
            note->param   = prevNote->param;

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return (true);
        }

        // copy command+(effect + 1) from above into current command+effect
        if ((keyEntry == SDLK_EQUALS) || (keyEntry == SDLK_WORLD_0)) // SDLK_WORLD_0 on OS X (weird)
        {
            note->command = prevNote->command;
            note->param   = prevNote->param + 1; // wraps 0x00..0xFF

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return (true);
        }

        // copy command+(effect - 1) from above into current command+effect
        if ((keyEntry == SDLK_MINUS) || (keyEntry == SDLK_PLUS)) // SDLK_PLUS on OS X (weird)
        {
            note->command = prevNote->command;
            note->param   = prevNote->param - 1; // wraps 0x00..0xFF

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            return (true);
        }
    }

    return (false);
}

void jamAndPlaceSample(SDLKey keyEntry, MODULE *modEntry, int8_t normalMode)
{
    int8_t noteVal;

    int16_t tempPeriod;
    uint16_t cleanPeriod;

    MODULE_SAMPLE *s;

    modnote_t *note;

    mod_channel *chn;

    chn  = &modEntry->channels[editor.cursor.channel];
    note = &modEntry->patterns[modEntry->currPattern][(QuantizeCheck(chn, modEntry->currRow) * PAULA_CHANNELS) + editor.cursor.channel];

    noteVal = normalMode ? keyToNote(keyEntry) : pnotetable[editor.currentSample];
    if (noteVal >= 0)
    {
        s                      = &modEntry->samples[editor.currentSample];
        tempPeriod             = PeriodTable[(37 * s->fineTune) + noteVal];
        cleanPeriod            = PeriodTable[noteVal];
        editor.currentPlayNote = noteVal;

        // play current sample
        if (!editor.muted[editor.cursor.channel])
        {
            // don't play sample if we quantized to another row (will be played in modplayer instead)
            if (!((editor.currentMode == MODE_RECORD) && chn->DidQuantize))
            {
                if (s->length == 0)
                {
                    mixerKillVoice(editor.cursor.channel);
                    chn->scopeEnabled = false;
                    chn->scopeTrigger = false;
                }
                else
                {
                    chn->sample          = editor.currentSample + 1;
                    chn->tempPeriod      = tempPeriod;
                    chn->period          = tempPeriod;
                    chn->volume          = s->volume;
                    chn->scopeEnabled    = true;
                    chn->scopeKeepPeriod = false;
                    chn->scopeKeepVolume = false;
                    chn->scopeVolume     = 0 - (s->volume / 2);
                    chn->scopePeriod     = tempPeriod / 2;
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
                    mixerSetChDelta(editor.cursor.channel, tempPeriod);
                    mixerSetChVol(editor.cursor.channel, s->volume);
                }
            }
        }

        if (normalMode || (!normalMode && editor.pnoteflag == 2))
        {
            // insert note and sample number
            if (!editor.ui.samplerScreenShown && ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD)))
            {
                note->sample = editor.SampleZero ? 0 : (editor.currentSample + 1);
                note->period = cleanPeriod;

                if (editor.AutoInsFlag)
                {
                    note->command = editor.EffectMacros[1] >> 8;
                    note->param   = editor.EffectMacros[1] & 0xFF;
                }

                if (editor.currentMode != MODE_RECORD)
                    modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }

            if (editor.MultiFlag)
            {
                editor.cursor.channel = (editor.MultiModeNext[editor.cursor.channel] - 1) & 3;
                editor.cursor.pos     = editor.cursor.channel * 6;
            }
        }

        // update spectrum bars if neeeded
        if ((editor.ui.visualizerMode == VISUAL_SPECTRUM) && !editor.muted[editor.cursor.channel])
            updateSpectrumAnalyzer(tempPeriod, s->volume);
    }
    else if (noteVal == -2)
    {
        // delete note and sample if illegal note (= -2, -1 = ignore) key was entered

        if (normalMode || (!normalMode && editor.pnoteflag == 2))
        {
            if (!editor.ui.samplerScreenShown && ((editor.currentMode == MODE_EDIT) || (editor.currentMode == MODE_RECORD)))
            {
                note->period = 0;
                note->sample = 0;

                if (editor.currentMode != MODE_RECORD)
                    modSetPos(DONT_SET_ORDER, (modEntry->currRow + editor.EditMoveAdd) & 0x3F);

                updateWindowTitle(modEntry, MOD_IS_MODIFIED);
            }
        }
    }
}

uint8_t QuantizeCheck(mod_channel *ch, uint8_t row)
{
    uint8_t tempRow;
    uint8_t quantize;

    quantize = (uint8_t)(editor.QuantizeValue);

    ch->DidQuantize = false;

    if (editor.currentMode == MODE_RECORD)
    {
        if (quantize == 0)
        {
            return (row);
        }
        else if (quantize == 1)
        {
            if (editor.modTick > (editor.modSpeed / 2))
            {
                row = (row + 1) & 0x3F;
                ch->DidQuantize = true;
            }
        }
        else
        {
            tempRow = ((((quantize / 2) + row) & 0x3F) / quantize) * quantize;
            if (tempRow > row)
                ch->DidQuantize = true;

            return (tempRow);
        }
    }

    return (row);
}

void SaveUndo(MODULE *modEntry)
{
    memcpy(editor.UndoBuffer, modEntry->patterns[modEntry->currPattern], MOD_ROWS * PAULA_CHANNELS * sizeof (modnote_t));
}

void UndoLastChange(MODULE *modEntry)
{
    uint16_t i;

    modnote_t data;

    for (i = 0; i < (MOD_ROWS * PAULA_CHANNELS); ++i)
    {
        data                 = editor.UndoBuffer[i];
        editor.UndoBuffer[i] = modEntry->patterns[modEntry->currPattern][i];

        modEntry->patterns[modEntry->currPattern][i] = data;
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void CopySampleTrack(MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;

    uint32_t tmpOffset;

    modnote_t *noteSrc;

    MODULE_SAMPLE *smpFrom;
    MODULE_SAMPLE *smpTo;

    if (editor.TrackPattFlag == 2)
    {
        // copy from one sample slot to another

        // never attempt to swap if from and/or to is 0
        if ((editor.SampleFrom == 0) || (editor.SampleTo == 0))
        {
            displayErrorMsg("FROM/TO = 0 !");
            return;
        }

        smpTo   = &modEntry->samples[editor.SampleTo   - 1];
        smpFrom = &modEntry->samples[editor.SampleFrom - 1];

        mixerKillVoiceIfReadingSample(modEntry, editor.SampleTo - 1);

        // copy
        tmpOffset     = smpTo->offset;
        *smpTo        = *smpFrom;
        smpTo->offset = tmpOffset;

        // update the copied sample display pointers (important)
        smpTo->volumeDisp     = &smpTo->volume;
        smpTo->lengthDisp     = &smpTo->length;
        smpTo->loopStartDisp  = &smpTo->loopStart;
        smpTo->loopLengthDisp = &smpTo->loopLength;

        // copy sample data
        memcpy(&modEntry->sampleData[smpTo->offset], &modEntry->sampleData[smpFrom->offset], 0x0001FFFE);

        setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
        calcRealModSize(modEntry);
    }
    else
    {
        // copy sample number in track/pattern
        if (editor.TrackPattFlag == 0)
        {
            for (i = 0; i < MOD_ROWS; ++i)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];
                if (noteSrc->sample == editor.SampleFrom)
                    noteSrc->sample = editor.SampleTo;
            }
        }
        else
        {
            for (i = 0; i < PAULA_CHANNELS; ++i)
            {
                for (j = 0; j < MOD_ROWS; ++j)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];
                    if (noteSrc->sample == editor.SampleFrom)
                        noteSrc->sample = editor.SampleTo;
                }
            }
        }
    }

    editor.SamplePos = 0;

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void ExchSampleTrack(MODULE *modEntry)
{
    int8_t smp;
    uint8_t i;
    uint8_t j;

    uint32_t k;
    uint32_t tmpOffset;

    MODULE_SAMPLE *smpFrom;
    MODULE_SAMPLE *smpTo;
    MODULE_SAMPLE smpTmp;

    modnote_t *noteSrc;

    if (editor.TrackPattFlag == 2)
    {
        // exchange sample slots

        // never attempt to swap if from and/or to is 0
        if ((editor.SampleFrom == 0) || (editor.SampleTo == 0))
        {
            displayErrorMsg("FROM/TO = 0 !");
            return;
        }

        smpTo   = &modEntry->samples[editor.SampleTo   - 1];
        smpFrom = &modEntry->samples[editor.SampleFrom - 1];

        mixerKillVoiceIfReadingSample(modEntry, editor.SampleFrom - 1);
        mixerKillVoiceIfReadingSample(modEntry, editor.SampleTo   - 1);

        // swap offsets first so that the next swap will leave offsets intact
        tmpOffset       = smpFrom->offset;
        smpFrom->offset = smpTo->offset;
        smpTo->offset   = tmpOffset;

        // swap sample (now offsets are left as before)
        smpTmp   = *smpFrom;
        *smpFrom = *smpTo;
        *smpTo   = smpTmp;

        // update the swapped sample display pointers (important)
        smpFrom->volumeDisp     = &smpFrom->volume;
        smpFrom->lengthDisp     = &smpFrom->length;
        smpFrom->loopStartDisp  = &smpFrom->loopStart;
        smpFrom->loopLengthDisp = &smpFrom->loopLength;
        smpTo->volumeDisp       = &smpTo->volume;
        smpTo->lengthDisp       = &smpTo->length;
        smpTo->loopStartDisp    = &smpTo->loopStart;
        smpTo->loopLengthDisp   = &smpTo->loopLength;

        // swap sample data
        for (k = 0; k < 0x0001FFFE; ++k)
        {
            smp = modEntry->sampleData[smpFrom->offset + k];

            modEntry->sampleData[smpFrom->offset + k] = modEntry->sampleData[smpTo->offset + k];
            modEntry->sampleData[smpTo->offset   + k] = smp;
        }

        editor.SampleZero = false;

        setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
    }
    else
    {
        // exchange sample number in track/pattern
        if (editor.TrackPattFlag == 0)
        {
            for (i = 0; i < MOD_ROWS; ++i)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

                     if (noteSrc->sample == editor.SampleFrom) noteSrc->sample = editor.SampleTo;
                else if (noteSrc->sample == editor.SampleTo)   noteSrc->sample = editor.SampleFrom;
            }
        }
        else
        {
            for (i = 0; i < PAULA_CHANNELS; ++i)
            {
                for (j = 0; j < MOD_ROWS; ++j)
                {
                    noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];

                         if (noteSrc->sample == editor.SampleFrom) noteSrc->sample = editor.SampleTo;
                    else if (noteSrc->sample == editor.SampleTo)   noteSrc->sample = editor.SampleFrom;
                }
            }
        }
    }

    editor.SamplePos = 0;

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void DelSampleTrack(MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;

    modnote_t *noteSrc;

    SaveUndo(modEntry);

    if (editor.TrackPattFlag == 0)
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
    }
    else
    {
        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            for (j = 0; j < MOD_ROWS; ++j)
            {
                noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];
                if (noteSrc->sample == (editor.currentSample + 1))
                {
                    noteSrc->period  = 0;
                    noteSrc->sample  = 0;
                    noteSrc->command = 0;
                    noteSrc->param   = 0;
                }
            }
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void TrackNoteUp(uint8_t sampleAllFlag, uint8_t from, uint8_t to, MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    if (from > to)
    {
        j    = from;
        from = to;
        to   = j;
    }

    SaveUndo(modEntry);

    for (i = from; i <= to; ++i)
    {
        noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

        if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
            continue;

        if (noteSrc->period)
        {
            // period -> note
            for (j = 0; j < 36; ++j)
            {
                if (noteSrc->period >= PeriodTable[j])
                    break;
            }

            noteDeleted = false;
            if (++j > 35)
            {
                j = 35;

                if (editor.TransDelFlag)
                {
                    noteSrc->period = 0;
                    noteSrc->sample = 0;
                    noteDeleted     = true;
                }
            }

            if (!noteDeleted)
                noteSrc->period = PeriodTable[j];
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void TrackNoteDown(uint8_t sampleAllFlag, uint8_t from, uint8_t to, MODULE *modEntry)
{
    int8_t j;
    uint8_t i;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    if (from > to)
    {
        j    = from;
        from = to;
        to   = j;
    }

    SaveUndo(modEntry);

    for (i = from; i <= to; ++i)
    {
        noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

        if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
            continue;

        if (noteSrc->period)
        {
            // period -> note
            for (j = 0; j < 36; ++j)
            {
                if (noteSrc->period >= PeriodTable[j])
                    break;
            }

            noteDeleted = false;
            if (--j < 0)
            {
                j = 0;

                if (editor.TransDelFlag)
                {
                    noteSrc->period = 0;
                    noteSrc->sample = 0;
                    noteDeleted     = true;
                }
            }

            if (!noteDeleted)
                noteSrc->period = PeriodTable[j];
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void TrackOctaUp(uint8_t sampleAllFlag, uint8_t from, uint8_t to, MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    if (from > to)
    {
        j    = from;
        from = to;
        to   = j;
    }

    SaveUndo(modEntry);

    for (i = from; i <= to; ++i)
    {
        noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

        if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
            continue;

        if (noteSrc->period)
        {
            // period -> note
            for (j = 0; j < 36; ++j)
            {
                if (noteSrc->period >= PeriodTable[j])
                    break;
            }

            noteDeleted = false;
            if (((j + 12) > 35) && editor.TransDelFlag)
            {
                noteSrc->period = 0;
                noteSrc->sample = 0;
                noteDeleted     = true;
            }

            if (j <= 23)
                j += 12;

            if (!noteDeleted)
                noteSrc->period = PeriodTable[j];
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void TrackOctaDown(uint8_t sampleAllFlag, uint8_t from, uint8_t to, MODULE *modEntry)
{
    int8_t j;
    uint8_t i;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    if (from > to)
    {
        j    = from;
        from = to;
        to   = j;
    }

    SaveUndo(modEntry);

    for (i = from; i <= to; ++i)
    {
        noteSrc = &modEntry->patterns[modEntry->currPattern][(i * PAULA_CHANNELS) + editor.cursor.channel];

        if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
            continue;

        if (noteSrc->period)
        {
            // period -> note
            for (j = 0; j < 36; ++j)
            {
                if (noteSrc->period >= PeriodTable[j])
                    break;
            }

            noteDeleted = false;
            if (((j - 12) < 0) && editor.TransDelFlag)
            {
                noteSrc->period = 0;
                noteSrc->sample = 0;
                noteDeleted     = true;
            }

            if (j >= 12)
                j -= 12;

            if (!noteDeleted)
                noteSrc->period = PeriodTable[j];
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void PattNoteUp(uint8_t sampleAllFlag, MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    SaveUndo(modEntry);

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        for (j = 0; j < MOD_ROWS; ++j)
        {
            noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];

            if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
                continue;

            if (noteSrc->period)
            {
                // period -> note
                for (k = 0; k < 36; ++k)
                {
                    if (noteSrc->period >= PeriodTable[k])
                        break;
                }

                noteDeleted = false;
                if (++k > 35)
                {
                    k = 35;

                    if (editor.TransDelFlag)
                    {
                        noteSrc->period = 0;
                        noteSrc->sample = 0;
                        noteDeleted     = true;
                    }
                }

                if (!noteDeleted)
                    noteSrc->period = PeriodTable[k];
            }
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void PattNoteDown(uint8_t sampleAllFlag, MODULE *modEntry)
{
    int8_t k;
    uint8_t i;
    uint8_t j;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    SaveUndo(modEntry);

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        for (j = 0; j < MOD_ROWS; ++j)
        {
            noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];

            if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
                continue;

            if (noteSrc->period)
            {
                // period -> note
                for (k = 0; k < 36; ++k)
                {
                    if (noteSrc->period >= PeriodTable[k])
                        break;
                }

                noteDeleted = false;
                if (--k < 0)
                {
                    k = 0;

                    if (editor.TransDelFlag)
                    {
                        noteSrc->period = 0;
                        noteSrc->sample = 0;
                        noteDeleted     = true;
                    }
                }

                if (!noteDeleted)
                    noteSrc->period = PeriodTable[k];
            }
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void PattOctaUp(uint8_t sampleAllFlag, MODULE *modEntry)
{
    uint8_t i;
    uint8_t j;
    uint8_t k;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    SaveUndo(modEntry);

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        for (j = 0; j < MOD_ROWS; ++j)
        {
            noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];

            if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
                continue;

            if (noteSrc->period)
            {
                // period -> note
                for (k = 0; k < 36; ++k)
                {
                    if (noteSrc->period >= PeriodTable[k])
                        break;
                }

                noteDeleted = false;
                if (((k + 12) > 35) && editor.TransDelFlag)
                {
                    noteSrc->period = 0;
                    noteSrc->sample = 0;
                    noteDeleted     = true;
                }

                if (k <= 23)
                    k += 12;

                if (!noteDeleted)
                    noteSrc->period = PeriodTable[k];
            }
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

void PattOctaDown(uint8_t sampleAllFlag, MODULE *modEntry)
{
    int8_t k;
    uint8_t i;
    uint8_t j;
    uint8_t noteDeleted;

    modnote_t *noteSrc;

    SaveUndo(modEntry);

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        for (j = 0; j < MOD_ROWS; ++j)
        {
            noteSrc = &modEntry->patterns[modEntry->currPattern][(j * PAULA_CHANNELS) + i];

            if (!sampleAllFlag && (noteSrc->sample != (editor.currentSample + 1)))
                continue;

            if (noteSrc->period)
            {
                // period -> note
                for (k = 0; k < 36; ++k)
                {
                    if (noteSrc->period >= PeriodTable[k])
                        break;
                }

                noteDeleted = false;
                if (((k - 12) < 0) && editor.TransDelFlag)
                {
                    noteSrc->period = 0;
                    noteSrc->sample = 0;
                    noteDeleted     = true;
                }

                if (k >= 12)
                    k -= 12;

                if (!noteDeleted)
                    noteSrc->period = PeriodTable[k];
            }
        }
    }

    updateWindowTitle(modEntry, MOD_IS_MODIFIED);
}

int8_t keyToNote(SDLKey keyEntry)
{
    int8_t noteFound;
    uint8_t rawNote;
    uint8_t rawKey;
    uint8_t i;
    uint8_t keyTransition;

    rawNote   = 0;
    rawKey    = 0;
    noteFound = false;

    // look up note by raw key
    for (i = 0; i < sizeof (UnshiftedKeymap); ++i)
    {
        if ((uint16_t)(keyEntry) == UnshiftedKeymap[i])
        {
            rawKey = i + 1;
            break;
        }
    }

    for (i = 0; i < sizeof (RawKeyScaleTable); ++i)
    {
        if (rawKey == RawKeyScaleTable[i])
        {
            rawNote   = i;
            noteFound = true;

            break;
        }
    }

    // was a note found in range?
    if (noteFound)
    {
        keyTransition = KbdTransTable[editor.keyOctave][rawNote];

        if (keyTransition <  36) return (keyTransition);
        if (keyTransition == 37) return (-2); // delete note/sample (only for pattern editing/record)
    }

    return (-1); // not a note key
}
