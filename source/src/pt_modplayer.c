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
#include "pt_header.h"
#include "pt_audio.h"
#include "pt_helpers.h"
#include "pt_palette.h"
#include "pt_tables.h"
#include "pt_modloader.h"
#include "pt_config.h"
#include "pt_sampler.h"
#include "pt_visuals.h"

extern int8_t forceMixerOff; // pt_audio.c

static int8_t tempVolume;
static int8_t pattBreakFlag;
static int8_t pattDelayFlag;
static int8_t pattBreakBugPos;
static int8_t forceEffectsOff;
static int8_t oldRow;
static uint8_t tempFlags;
static uint8_t PBreakFlag;
static uint8_t PosJumpAssert;
static uint8_t PattDelayTime;
static uint8_t PattDelayTime2;
static uint8_t PBreakPosition;
static uint8_t modHasBeenPlayed = false;
static uint8_t oldSpeed;

static int16_t modOrder;
static int16_t modPattern;
static int16_t tempPeriod;
static int16_t oldPattern;
static int16_t oldOrder;
static uint16_t modBPM;
static uint16_t oldBPM;

static MODULE *source;

void storeTempVariables(void)
{
    oldRow     = source->currRow;
    oldPattern = source->currPattern;
    oldOrder   = source->currOrder;
    oldSpeed   = source->currSpeed;
    oldBPM     = source->currBPM;
}

static int8_t periodToNote(int16_t period)
{
    uint8_t i;

    for (i = 0; i < 36; ++i)
    {
        if (period >= PeriodTable[i])
            break;
    }

    return (i);
}

enum
{
    FLAG_NOTE        = 1,
    FLAG_SAMPLE      = 2,
    FLAG_NEWSAMPLE   = 4,
    FLAG_SET_VUMETER = 8
};

enum
{
    TEMPFLAG_START       = 1,
    TEMPFLAG_DELAY       = 2,
    TEMPFLAG_SET_VUMETER = 4,
    TEMPFLAG_NEW_SAMPLE  = 8
};


// Normal effects
static void fxArpeggio(mod_channel *ch);
static void fxPortamentoSlideUp(mod_channel *ch);
static void fxPortamentoSlideDown(mod_channel *ch);
static void fxGlissando(mod_channel *ch);
static void fxVibrato(mod_channel *ch);
static void fxGlissandoVolumeSlide(mod_channel *ch);
static void fxVibratoVolumeSlide(mod_channel *ch);
static void fxTremolo(mod_channel *ch);
static void fxNotInUse(mod_channel *ch);
static void fxSampleOffset(mod_channel *ch);
static void fxVolumeSlide(mod_channel *ch);
static void fxPositionJump(mod_channel *ch);
static void fxSetVolume(mod_channel *ch);
static void fxPatternBreak(mod_channel *ch);
static void fxExtended(mod_channel *ch);
static void fxSetTempo(mod_channel *ch);

// Extended effects
static void efxSetLEDFilter(mod_channel *ch);
static void efxFinePortamentoSlideUp(mod_channel *ch);
static void efxFinePortamentoSlideDown(mod_channel *ch);
static void efxGlissandoControl(mod_channel *ch);
static void efxVibratoControl(mod_channel *ch);
static void efxSetFineTune(mod_channel *ch);
static void efxPatternLoop(mod_channel *ch);
static void efxTremoloControl(mod_channel *ch);
static void efxKarplusStrong(mod_channel *ch);
static void efxRetrigNote(mod_channel *ch);
static void efxFineVolumeSlideUp(mod_channel *ch);
static void efxFineVolumeSlideDown(mod_channel *ch);
static void efxNoteCut(mod_channel *ch);
static void efxNoteDelay(mod_channel *ch);
static void efxPatternDelay(mod_channel *ch);
static void efxInvertLoop(mod_channel *ch);

typedef void (*effect_routine)(mod_channel *);

// Normal effects
static effect_routine fxRoutines[16] =
{
    fxArpeggio,
    fxPortamentoSlideUp,
    fxPortamentoSlideDown,
    fxGlissando,
    fxVibrato,
    fxGlissandoVolumeSlide,
    fxVibratoVolumeSlide,
    fxTremolo,
    fxNotInUse,
    fxSampleOffset,
    fxVolumeSlide,
    fxPositionJump,
    fxSetVolume,
    fxPatternBreak,
    fxExtended,
    fxSetTempo
};

// Extended effects
static effect_routine efxRoutines[16] =
{
    efxSetLEDFilter,
    efxFinePortamentoSlideUp,
    efxFinePortamentoSlideDown,
    efxGlissandoControl,
    efxVibratoControl,
    efxSetFineTune,
    efxPatternLoop,
    efxTremoloControl,
    efxKarplusStrong,
    efxRetrigNote,
    efxFineVolumeSlideUp,
    efxFineVolumeSlideDown,
    efxNoteCut,
    efxNoteDelay,
    efxPatternDelay,
    efxInvertLoop
};

void setPlayerModSource(MODULE *modEntry)
{
    source = modEntry;
}

static void processInvertLoop(mod_channel *ch)
{
    if (ch->invertLoopSpeed > 0)
    {
        ch->invertLoopDelay += FunkTable[ch->invertLoopSpeed];
        if (ch->invertLoopDelay >= 128)
        {
            ch->invertLoopDelay = 0;

            if (ch->invertLoopPtr != NULL) // PT doesn't do this, but we're more sane than that.
            {
                if (++ch->invertLoopPtr >= (ch->invertLoopStart + ch->invertLoopLength))
                      ch->invertLoopPtr  =  ch->invertLoopStart;

                *ch->invertLoopPtr = -1 - *ch->invertLoopPtr;
            }
        }
    }
}

static void efxSetLEDFilter(mod_channel *ch)
{
    if (editor.modTick == 0)
        setLEDFilter(!(ch->param & 1));
}

static void efxFinePortamentoSlideUp(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if (tempPeriod > 0)
        {
            ch->period -= (ch->param & 0x0F);
            if (ch->period < 113)
                ch->period = 113;

            tempPeriod = ch->period;
        }
    }
}

static void efxFinePortamentoSlideDown(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if (tempPeriod > 0)
        {
            ch->period += (ch->param & 0x0F);
            if (ch->period > 856)
                ch->period = 856;

            tempPeriod = ch->period;
        }
    }
}

static void efxGlissandoControl(mod_channel *ch)
{
    if (editor.modTick == 0)
        ch->glissandoControl = ch->param & 0x0F;
}

static void efxVibratoControl(mod_channel *ch)
{
    if (editor.modTick == 0)
        ch->waveControl = (ch->waveControl & 0xF0) | (ch->param & 0x0F);
}

static void efxSetFineTune(mod_channel *ch)
{
    if (editor.modTick == 0)
        ch->fineTune = ch->param & 0x0F;
}

static void efxPatternLoop(mod_channel *ch)
{
    uint8_t tempParam;

    if (editor.modTick == 0)
    {
        tempParam = ch->param & 0x0F;
        if (tempParam == 0)
        {
            ch->patternLoopRow = source->row;

            return;
        }

        if (ch->patternLoopCounter == 0)
        {
            ch->patternLoopCounter = tempParam;
        }
        else
        {
            if (--ch->patternLoopCounter == 0)
                return;
        }

        PBreakPosition = ch->patternLoopRow;
        PBreakFlag     = true;

        if (editor.isWAVRendering)
        {
            for (tempParam = PBreakPosition; tempParam <= source->row; ++tempParam)
                editor.rowVisitTable[(modOrder * MOD_ROWS) + tempParam] = false;
        }
    }
}

static void efxTremoloControl(mod_channel *ch)
{
    if (editor.modTick == 0)
        ch->waveControl = (ch->param << 4) | (ch->waveControl & 0x0F);
}

static void efxKarplusStrong(mod_channel *ch)
{
    // Karplus-Strong, my enemy...
    // 1) Almost no one used it as Karplus-Strong
    // 2) It trashes sample data
    // 3) Often used as effect syncing, so we don't want to destroy sample(s) when playing

    (void)(ch);
}

static void efxRetrigNote(mod_channel *ch)
{
    uint8_t retrigTick;

    retrigTick = ch->param & 0x0F;
    if (retrigTick > 0)
    {
        if ((editor.modTick % retrigTick) == 0)
            tempFlags |= TEMPFLAG_START;
    }
}

static void efxFineVolumeSlideUp(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        ch->volume += (ch->param & 0x0F);
        if (ch->volume > 64)
            ch->volume = 64;

        tempVolume = ch->volume;
    }
}

static void efxFineVolumeSlideDown(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        ch->volume -= (ch->param & 0x0F);
        if (ch->volume < 0)
            ch->volume = 0;

        tempVolume = ch->volume;
    }
}

static void efxNoteCut(mod_channel *ch)
{
    if (editor.modTick == (ch->param & 0x0F))
    {
        ch->volume      = 0;
        tempVolume      = 0;
        ch->scopeVolume = 0;
    }
}

static void efxNoteDelay(mod_channel *ch)
{
    uint8_t delayTick;

    delayTick = ch->param & 0x0F;

    if (editor.modTick == 0)
        ch->tempFlagsBackup = tempFlags;

         if (editor.modTick  < delayTick) tempFlags = TEMPFLAG_DELAY;
    else if (editor.modTick == delayTick) tempFlags = ch->tempFlagsBackup;
}

static void efxPatternDelay(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if (PattDelayTime2 == 0)
        {
            pattDelayFlag = true;
            PattDelayTime = (ch->param & 0x0F) + 1;
        }
    }
}

static void efxInvertLoop(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        ch->invertLoopSpeed = ch->param & 0x0F;

        if (ch->invertLoopSpeed > 0)
            processInvertLoop(ch);
    }
}

void setupGlissando(mod_channel *ch, int16_t period)
{
    uint8_t i;

    int16_t *tablePointer;

    tablePointer = &PeriodTable[37 * ch->fineTune];

    i = 0;
    while (true)
    {
        if (period >= tablePointer[i])
            break;

        if (++i >= 37)
        {
            i = 35;
            break;
        }
    }

    if ((ch->fineTune & 8) && i) i--;

    ch->wantedperiod  = tablePointer[i];
    ch->toneportdirec = 0;

         if (ch->period == ch->wantedperiod) ch->wantedperiod  = 0; // don't do any more slides
    else if (ch->period >  ch->wantedperiod) ch->toneportdirec = 1;
}

static void processGlissando(mod_channel *ch)
{
    uint8_t i;

    int16_t *tablePointer;

    if ((ch->wantedperiod > 0) && (tempPeriod > 0))
    {
        if (ch->toneportdirec == 0)
        {
            // downwards
            ch->period += ch->toneportspeed;
            if (ch->period >= ch->wantedperiod)
            {
                ch->period       = ch->wantedperiod;
                ch->wantedperiod = 0; // don't do any more sliding
            }
        }
        else
        {
            // upwards
            ch->period -= ch->toneportspeed;
            if (ch->period <= ch->wantedperiod)
            {
                ch->period       = ch->wantedperiod;
                ch->wantedperiod = 0; // don't do any more sliding
            }
        }

        if (ch->glissandoControl == 0)
        {
            // smooth sliding (default)
            tempPeriod = ch->period;
        }
        else 
        {
            // semitone sliding
            tablePointer = &PeriodTable[37 * ch->fineTune];

            i = 0;
            while (true)
            {
                if (ch->period >= tablePointer[i])
                    break;

                if (++i >= 37)
                {
                    i = 35;
                    break;
                }
            }

            tempPeriod = tablePointer[i];
        }
    }
}

static void processVibrato(mod_channel *ch)
{
    uint8_t vibratoTemp;

    int16_t vibratoData;

    if (tempPeriod > 0)
    {
        vibratoTemp = (ch->vibratoPos & 0x7C) >> 2;
        vibratoData = ch->waveControl & 0x03;

        if (vibratoData == 0)
        {
            vibratoData = VibratoTable[vibratoTemp];
        }
        else
        {
            if (vibratoData == 1)
            {
                if (ch->vibratoPos < 0)
                    vibratoData = 255 - (vibratoTemp * 8);
                else
                    vibratoData = vibratoTemp * 8;
            }
            else
            {
                vibratoData = 255;
            }
        }

        vibratoData = (vibratoData * (ch->vibratoCmd & 0x0F)) / 128;

        // PT doesn't clamp this!
        if (ch->vibratoPos >= 0)
            tempPeriod += vibratoData; // max end result: 937
        else
            tempPeriod -= vibratoData; // min end result: 78

        // for quadrascope
        ch->scopePeriod     = tempPeriod / 2;
        ch->scopeKeepPeriod = false;

        ch->vibratoPos += ((ch->vibratoCmd & 0xF0) >> 2);
    }
}

static void processTremolo(mod_channel *ch)
{
    uint8_t tremoloTemp;
    uint8_t tremoloData;

    tremoloTemp = (ch->tremoloPos & 0x7C) >> 2;
    tremoloData = (ch->waveControl >> 4) & 0x03;

    if (tremoloData == 0)
    {
        tremoloData = VibratoTable[tremoloTemp];
    }
    else
    {
        if (tremoloData == 1)
        {
            if (ch->vibratoPos < 0) // PT typo/bug
                tremoloData = 255 - (tremoloTemp * 8);
            else
                tremoloData = tremoloTemp * 8;
        }
        else
        {
            tremoloData = 255;
        }
    }

    tremoloData = (tremoloData * (ch->tremoloCmd & 0x0F)) / 64;

    if (ch->tremoloPos >= 0)
    {
        tempVolume += tremoloData;
        if (tempVolume > 64)
            tempVolume = 64;
    }
    else
    {
        tempVolume -= tremoloData;
        if (tempVolume < 0)
            tempVolume = 0;
    }

    ch->scopeVolume     = 0 - (tempVolume / 2);
    ch->scopeKeepVolume = false;
    ch->tremoloPos     += ((ch->tremoloCmd & 0xF0) >> 2);
}

static void fxArpeggio(mod_channel *ch)
{
    uint8_t i;
    uint8_t noteToAdd;
    uint8_t arpeggioTick;

    int16_t *tablePointer;

    if (tempPeriod > 0)
    {
        noteToAdd = 0; // make compiler happy. (jerk)

        arpeggioTick = editor.modTick % 3;
        if (arpeggioTick == 0)
        {
            tempPeriod = ch->period;
            return;
        }
        else if (arpeggioTick == 1)
        {
            noteToAdd = ch->param >> 4;
        }
        else if (arpeggioTick == 2)
        {
            noteToAdd = ch->param & 0x0F;
        }

        tablePointer = &PeriodTable[37 * ch->fineTune];
        for (i = 0; i < 36; ++i)
        {
            if (ch->period >= tablePointer[i])
            {
                tempPeriod = tablePointer[i + noteToAdd];

                // for quadrascope
                ch->scopePeriod     = tempPeriod / 2;
                ch->scopeKeepPeriod = false;

                break;
            }
        }
    }
}

static void fxPortamentoSlideUp(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        if (tempPeriod > 0)
        {
            ch->period -= ch->param;
            if (ch->period < 113)
                ch->period = 113;

            tempPeriod = ch->period;
        }
    }
}

static void fxPortamentoSlideDown(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        if (tempPeriod > 0)
        {
            ch->period += ch->param;
            if (ch->period > 856)
                ch->period = 856;

            tempPeriod = ch->period;
        }
    }
}

static void fxGlissando(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        if (ch->param != 0)
        {
            ch->toneportspeed = ch->param;
            ch->param = 0;
        }

        processGlissando(ch);
    }
}

static void fxVibrato(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        if (ch->param != 0)
        {
            if (ch->param & 0x0F) ch->vibratoCmd = (ch->vibratoCmd & 0xF0) | (ch->param & 0x0F);
            if (ch->param & 0xF0) ch->vibratoCmd = (ch->param & 0xF0) | (ch->vibratoCmd & 0x0F);
        }

        processVibrato(ch);
    }
}

static void fxGlissandoVolumeSlide(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        processGlissando(ch);
        fxVolumeSlide(ch);
    }
}

static void fxVibratoVolumeSlide(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        processVibrato(ch);
        fxVolumeSlide(ch);
    }
}

static void fxTremolo(mod_channel *ch)
{
    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        if (ch->param != 0)
        {
            if (ch->param & 0x0F) ch->tremoloCmd = (ch->tremoloCmd & 0xF0) | (ch->param & 0x0F);
            if (ch->param & 0xF0) ch->tremoloCmd = (ch->param & 0xF0) | (ch->tremoloCmd & 0x0F);
        }

        processTremolo(ch);
    }
}

static void fxNotInUse(mod_channel *ch)
{
    // This is an empty effect slot not used in
    // ProTracker... often used for demo efx syncing
    //
    // Used for panning in Fasttracker and other .MOD trackers

    (void)(ch); // make compiler happy
}

static void fxSampleOffset(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if (ch->param > 0)
            ch->offsetTemp = 256 * (int32_t)(ch->param);

        ch->offset += ch->offsetTemp;

        if (!ch->noNote)
            ch->offsetBugNotAdded = false;
    }
}

static void fxVolumeSlide(mod_channel *ch)
{
    uint8_t hiNybble;
    uint8_t loNybble;

    if ((editor.modTick > 0) || (PattDelayTime2 > 0)) // all ticks while EDx (weird)
    {
        hiNybble = ch->param >> 4;
        loNybble = ch->param & 0x0F;

        if (hiNybble == 0)
        {
            ch->volume -= loNybble;
            if (ch->volume < 0)
                ch->volume = 0;
        }
        else
        {
            ch->volume += hiNybble;
            if (ch->volume > 64)
                ch->volume = 64;
        }

        tempVolume = ch->volume;
    }
}

static void fxPositionJump(mod_channel *ch)
{
    if (editor.isSMPRendering)
    {
        modHasBeenPlayed = true;
    }
    else
    {
        if (editor.modTick == 0)
        {
            if (editor.isWAVRendering)
            {
                modOrder = ch->param - 1;
            }
            else
            {
                if (editor.playMode != PLAY_MODE_PATTERN)
                {
                    modOrder = (ch->param & 0x7F) - 1;
                    if (modOrder == -1)
                        modOrder = source->head.orderCount - 1;
                }
            }

            PBreakPosition  = 0;
            PosJumpAssert   = true;
            pattBreakBugPos = 0;
            pattBreakFlag   = true;
        }
    }
}

static void fxSetVolume(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if (ch->param > 64)
            ch->param = 64;

        ch->volume = ch->param;
        tempVolume = ch->param;
    }
}

static void fxPatternBreak(mod_channel *ch)
{
    uint8_t pos;

    if (editor.isSMPRendering)
    {
        modHasBeenPlayed = true;
    }
    else
    {
        if (editor.modTick == 0)
        {
            pos = (((ch->param >> 4) * 10) + (ch->param & 0x0F));

            if (pos > 63)
                PBreakPosition = 0;
            else
                PBreakPosition = pos;

            pattBreakBugPos = PBreakPosition;
            pattBreakFlag   = true;
            PosJumpAssert   = true;
        }
    }
}

static void fxExtended(mod_channel *ch)
{
    efxRoutines[ch->param >> 4](ch);
}

static void fxSetTempo(mod_channel *ch)
{
    if (editor.modTick == 0)
    {
        if ((ch->param < 32) || (editor.timingMode == TEMPO_MODE_VBLANK))
        {
            if (ch->param == 0)
            {
                if (editor.isWAVRendering)
                {
                    modHasBeenPlayed = true;
                    return;
                }

                editor.songPlaying = false;

                editor.currentMode = MODE_IDLE;
                pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);
                setStatusMessage("ALL RIGHT", DO_CARRY);
            }
            else
            {
                modSetSpeed(ch->param);
            }
        }
        else
        {
            modSetTempo(ch->param);
        }
    }
}

static void processEffects(mod_channel *ch)
{
    if (editor.modTick > 0)
        processInvertLoop(ch);

    if ((!ch->command && !ch->param) == 0)
        fxRoutines[ch->command](ch);
}

static void fetchPatternData(mod_channel *ch)
{
    int8_t tempNote;

    modnote_t *note;

    note = &source->patterns[modPattern][(source->row * PAULA_CHANNELS) + ch->chanIndex];
    if ((note->sample >= 1) && (note->sample <= 31))
    {
        if (ch->sample != note->sample)
            ch->flags |= FLAG_NEWSAMPLE;

        ch->sample   = note->sample;
        ch->flags   |= FLAG_SAMPLE;
        ch->fineTune = source->samples[ch->sample - 1].fineTune;
    }

    ch->command = note->command;
    ch->param   = note->param;

    if (note->period > 0)
    {
        // set finetune if E5x is called w/ a note
        if (ch->command == 0x0E)
        {
            if ((ch->param >> 4) == 0x05)
                ch->fineTune = ch->param & 0x0F;
        }

        tempNote = periodToNote(note->period);
        if (tempNote == 36) // outside of 113..856 range
        {
            ch->noNote         = true;
            ch->scopeLoopQuirk = false;
            ch->scopeEnabled   = false;
            ch->scopeTrigger   = false;

            mixerKillVoice(ch->chanIndex);
        }
        else
        {
            ch->noNote     = false;
            ch->tempPeriod = PeriodTable[(37 * ch->fineTune) + tempNote];
            ch->flags     |= FLAG_NOTE;
        }

        if ((ch->command == 0x03) || (ch->command == 0x05))
            setupGlissando(ch, note->period);
    }
    else
    {
        ch->noNote = true;
    }

    // Do the metronome!
    if (editor.MetroChannel && editor.MetroFlag)
    {
        if ((ch->chanIndex == (editor.MetroChannel - 1)) && ((source->row % editor.MetroSpeed) == 0))
        {
            ch->sample   = 0x1F;
            ch->fineTune = source->samples[ch->sample - 1].fineTune;

            // set finetune if E5x is called w/ a note
            if (ch->command == 0x0E)
            {
                if ((ch->param >> 4) == 0x05)
                    ch->fineTune = ch->param & 0x0F;
            }

            // This seems to be what PT2.3D does. Not easy without the PT2.3D source code..
            // The PT source code (PT1.2A.asm) never had the PT2.3D styled metronome.
            tempNote = periodToNote((((source->row / editor.MetroSpeed) % editor.MetroSpeed) == 0) ? 160 : 214);
            ch->tempPeriod = PeriodTable[(37 * ch->fineTune) + tempNote];

            ch->flags |= FLAG_SAMPLE;
            ch->flags |= FLAG_NOTE;

            ch->noNote = false;
        }
    }
}

static void processChannel(mod_channel *ch)
{
    MODULE_SAMPLE *s;

    tempFlags = 0;

    if (editor.modTick == 0)
    {
        if (PattDelayTime2 == 0)
            fetchPatternData(ch);

        // In PT, the spectrum analyzer doesn't use the last volume affected
        // by volume effects. It just uses the last s->sample value for
        // the current channel.
        // So we use a separate variable for this, and update it before
        // we update the spectrum analyzer for correct visual results.
        if (ch->flags & FLAG_SAMPLE)
        {
            if (ch->sample != 0)
                ch->rawVolume = source->samples[ch->sample - 1].volume;
        }

        if (ch->flags & FLAG_NOTE)
        {
            if ((ch->command != 0x03) && (ch->command != 0x05))
            {
                if (ch->sample != 0)
                    tempFlags |= TEMPFLAG_START;

                ch->period = ch->tempPeriod; // ranges 113..856 at this stage

                // update spectrum bars if neeeded
                if (!editor.isWAVRendering && !editor.isSMPRendering && (editor.ui.visualizerMode == VISUAL_SPECTRUM) && !editor.muted[ch->chanIndex])
                    updateSpectrumAnalyzer(ch->period, ch->rawVolume);
            }

            ch->tempFlagsBackup = 0;

            if ((ch->waveControl & 0x04) == 0) ch->vibratoPos = 0;
            if ((ch->waveControl & 0x40) == 0) ch->tremoloPos = 0;
        }

        if (ch->flags & FLAG_SAMPLE)
        {
            ch->flags &= ~FLAG_SAMPLE;

            if (ch->sample != 0)
            {
                s = &source->samples[ch->sample - 1];

                ch->volume           = s->volume;
                ch->invertLoopPtr    = &source->sampleData[s->offset + s->loopStart];
                ch->invertLoopStart  = ch->invertLoopPtr;
                ch->invertLoopLength = s->loopLength;

                if ((ch->command != 0x03) && (ch->command != 0x05))
                {
                    ch->offset            = 0;
                    ch->offsetBugNotAdded = true;
                }

                if (ch->flags & FLAG_NEWSAMPLE)
                {
                    ch->flags &= ~FLAG_NEWSAMPLE;

                    // sample swap (even during note delays - PT logics)
                    if (ch->sample != 0)
                    {
                        s = &source->samples[ch->sample - 1];

                        if (!editor.muted[ch->chanIndex])
                        {
                            mixerSwapChSource(ch->chanIndex, source->sampleData + s->offset, s->length, s->loopStart, s->loopLength, s->offset);
                        }
                        else
                        {
                            ch->scopeLoopQuirk = false;
                            ch->scopeEnabled   = false;
                            ch->scopeTrigger   = false;

                            mixerKillVoice(ch->chanIndex);
                        }
                    }
                }
            }
        }
    }

    tempPeriod          = ch->period;
    tempVolume          = ch->volume;
    ch->scopeKeepPeriod = true;
    ch->scopeKeepVolume = true;

    if (!forceEffectsOff)
        processEffects(ch);

    if (editor.modTick == 0)
    {
        if (ch->flags & FLAG_NOTE)
        {
            ch->flags &= ~FLAG_NOTE;

            if (!editor.isWAVRendering && !editor.muted[ch->chanIndex])
                editor.vuMeterVolumes[ch->chanIndex] = VUmeterHeights[tempVolume];
        }
    }

    mixerSetChVol(ch->chanIndex, tempVolume); // update volume even if note delay (PT behavior)

    if (!(tempFlags & TEMPFLAG_DELAY))
    {
        if (tempFlags & TEMPFLAG_START)
        {
            if (ch->sample != 0)
            {
                s = &source->samples[ch->sample - 1];

                if ((s->length > 0) && !editor.muted[ch->chanIndex])
                {
                    if (!editor.isWAVRendering)
                        ch->scopeTrigger = true;

                    if (ch->offset > 0)
                    {
                        mixerSetChSource(ch->chanIndex, source->sampleData + s->offset, s->length, s->loopStart, s->loopLength, ch->offset);

                        if (!editor.isWAVRendering && !editor.isSMPRendering)
                        {
                            ch->scopeChangePos = true;
                            if (s->loopLength > 2)
                            {
                                if (ch->offset >= (s->loopStart + s->loopLength))
                                    ch->scopePos = s->offset + (s->loopStart + s->loopLength);
                                else
                                    ch->scopePos = s->offset + ch->offset;
                            }
                            else
                            {
                                if (ch->offset >= s->length)
                                {
                                    ch->scopeEnabled = false;
                                    ch->scopeTrigger = false;
                                }
                                else
                                {
                                    ch->scopePos = s->offset + ch->offset;
                                }
                            }

                            // PT bug: Sample len >65535 + 9xx efx = stop sample (scopes this time)
                            if ((s->length > 65535) && (ch->offset > 0))
                            {
                                ch->scopeEnabled = false;
                                ch->scopeTrigger = false;
                            }
                        }

                        if (!ch->offsetBugNotAdded)
                        {
                            ch->offset += ch->offsetTemp; // emulates add sample offset bug (fixes some quirky MODs)
                            if (ch->offset > 0xFFFF)
                                ch->offset = 0xFFFF;

                            ch->offsetBugNotAdded = true;
                        }
                    }
                    else
                    {
                        mixerSetChSource(ch->chanIndex, source->sampleData + s->offset, s->length, s->loopStart, s->loopLength, 0);
                    }
                }
                else
                {
                    ch->scopeEnabled = false;
                    ch->scopeTrigger = false;

                    mixerKillVoice(ch->chanIndex);
                }
            }
        }

        if (tempPeriod != 0)
            mixerSetChDelta(ch->chanIndex, tempPeriod);
        else
            mixerSetChVol(ch->chanIndex, 0); // arp override bugfix
    }
}

void doStopIt(void)
{
    uint8_t i;

    editor.playMode    = PLAY_MODE_NORMAL;
    editor.currentMode = MODE_IDLE;
    editor.songPlaying = false;
    PattDelayTime      = 0;
    PattDelayTime2     = 0;

    pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        source->channels[i].waveControl        = 0;
        source->channels[i].glissandoControl   = 0;
        source->channels[i].invertLoopSpeed    = 0;
        source->channels[i].fineTune           = 0;
        source->channels[i].patternLoopCounter = 0;
    }
}

void setPattern(int16_t pattern)
{
    modPattern = pattern;
    if (modPattern > (MAX_PATTERNS - 1))
        modPattern =  MAX_PATTERNS - 1;

    source->currPattern = modPattern;
}

static void nextPosition(void)
{
    source->row    = PBreakPosition;
    PBreakPosition = 0;
    PosJumpAssert  = false;

    if ((editor.playMode != PLAY_MODE_PATTERN) ||
        ((editor.currentMode == MODE_RECORD) && (editor.RecordMode != RECORD_PATT)))
    {
        if (editor.stepPlayEnabled)
        {
            doStopIt();

            editor.stepPlayEnabled   = false;
            editor.stepPlayBackwards = false;

            if (!editor.isWAVRendering && !editor.isSMPRendering)
                source->currRow = source->row;

            return;
        }

        if (++modOrder >= source->head.orderCount)
        {
            modOrder         = 0;
            modHasBeenPlayed = true;
        }

        modPattern = source->head.order[modOrder];
        if (modPattern > (MAX_PATTERNS - 1))
            modPattern =  MAX_PATTERNS - 1;

        if (!editor.isWAVRendering && !editor.isSMPRendering)
        {
            source->currPattern = modPattern;

            editor.currentPatternDisp   = &source->head.order[modOrder];
            editor.currentPosEdPattDisp = &source->head.order[modOrder];
        }
    }
}

int8_t processTick(void)
{
    if (!editor.songPlaying)
        return (false);

    if ((editor.isSMPRendering || editor.isWAVRendering) && modHasBeenPlayed)
    {
        modHasBeenPlayed = false;
        return (false);
    }

    // EEx + Dxx/Bxx quirk simulation
    if (editor.modTick == 0)
    {
        if (forceEffectsOff)
        {
            if (source->row != pattBreakBugPos)
            {
                forceEffectsOff = false;
                pattBreakBugPos = -1;
            }
        }
    }

    if (editor.isWAVRendering)
    {
        if (editor.modTick == 0)
            editor.rowVisitTable[(((modOrder < 0) ? (source->head.orderCount - 1) : modOrder) * MOD_ROWS) + source->row] = true;
    }

    processChannel(source->channels);
    processChannel(source->channels + 1);
    processChannel(source->channels + 2);
    processChannel(source->channels + 3);

    // EEx + Dxx/Bxx quirk simulation
    if (editor.modTick == 0)
    {
        if (pattBreakFlag && pattDelayFlag)
            forceEffectsOff = true;
    }

    if (!editor.stepPlayEnabled)
        editor.modTick++;

    if ((editor.modTick >= editor.modSpeed) || editor.stepPlayEnabled)
    {
        editor.modTick = 0;
        pattBreakFlag  = false;
        pattDelayFlag  = false;

        if (!editor.stepPlayBackwards)
        {
            source->row++;
            source->rowsCounter++;
        }

        if (PattDelayTime > 0)
        {
            PattDelayTime2 = PattDelayTime;
            PattDelayTime  = 0;
        }

        if (PattDelayTime2 > 0)
        {
            if (--PattDelayTime2 > 0)
                source->row--;
        }

        if (PBreakFlag)
        {
            PBreakFlag     = false;
            source->row    = PBreakPosition;
            PBreakPosition = 0;
        }

        if (editor.stepPlayEnabled)
        {
            doStopIt();

            editor.stepPlayEnabled   = false;
            editor.stepPlayBackwards = false;
            source->currRow          = source->row & 0x3F;

            return (true);
        }

        if ((source->row >= MOD_ROWS) || PosJumpAssert)
        {
            if (editor.isSMPRendering)
                return (false);

            nextPosition();
        }

        if (editor.isWAVRendering && !PattDelayTime2 && editor.rowVisitTable[(modOrder * MOD_ROWS) + source->row])
        {
            modHasBeenPlayed = true;
            return (false);
        }

        if (!editor.isWAVRendering && !editor.isSMPRendering)
        {
            source->currRow             = source->row;

            if (editor.playMode != PLAY_MODE_PATTERN)
            {
                source->currOrder           = modOrder;
                editor.currentPatternDisp   = &source->head.order[modOrder];
                editor.currentPosEdPattDisp = &source->head.order[modOrder];
            }
        }
    }

    return (true);
}

void modSetPos(int16_t order, int16_t row)
{
    uint8_t i;

    int16_t posEdPos;

    if (row != -1)
    {
        row = CLAMP(row, 0, 63);

        source->row     = (int8_t)(row);
        source->currRow = (int8_t)(row);
        PBreakPosition  = 0;
        PosJumpAssert   = false;
        editor.modTick  = 0;
    }

    if (order != -1)
    {
        if (order >= 0)
        {
            for (i = 0; i < PAULA_CHANNELS; ++i)
            {
                source->channels[i].patternLoopRow     = 0;
                source->channels[i].patternLoopCounter = 0;
            }

            modOrder          = order;
            source->currOrder = order;

            if ((editor.currentMode == MODE_PLAY) && (editor.playMode == PLAY_MODE_NORMAL))
            {
                modPattern = source->head.order[order];
                if (modPattern > (MAX_PATTERNS - 1))
                    modPattern =  MAX_PATTERNS - 1;

                source->currPattern = modPattern;
                PBreakPosition      = 0;
                PosJumpAssert       = false;
            }

            editor.currentPatternDisp   = &source->head.order[modOrder];

            posEdPos = source->currOrder;
            if (posEdPos > (source->head.orderCount - 1))
                posEdPos =  source->head.orderCount - 1;

            editor.currentPosEdPattDisp = &source->head.order[posEdPos];
        }
    }
}

void modSetSpeed(uint8_t speed)
{
    editor.modSpeed   = speed;
    source->currSpeed = speed;
    editor.modTick    = 0;
}

void modSetTempo(uint16_t bpm)
{
    // never feed 0 to this function!

    modBPM = bpm;

    if (!editor.isWAVRendering)
        source->currBPM = bpm;

    if (editor.isSMPRendering)
        mixerSetSamplesPerTick(((editor.pat2SmpHQ ? 28836 : 22168) * PT_STD_BPM) / (bpm * PAL_AMIGA_VBL_RATE));
    else
        mixerSetSamplesPerTick(audio.tempoTimerVal / (bpm * PAL_AMIGA_VBL_RATE));
}

void modStop(void)
{
    uint8_t i;

    mod_channel *ch;

    editor.songPlaying = false;

    mixerKillAllVoices();

    // don't reset sample, volume or command memory/flags
    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        ch = &source->channels[i];

        ch->chanIndex          = i;
        ch->command            = 0;
        ch->fineTune           = 0;
        ch->flags              = 0;
        ch->noNote             = true;
        ch->offset             = 0;
        ch->offsetBugNotAdded  = true;
        ch->param              = 0;
        ch->patternLoopCounter = 0;
        ch->patternLoopRow     = 0;
        ch->period             = 0;
        // -- these two gets zeroed anyways --
        ch->glissandoControl   = 0;
        ch->waveControl        = 0;
        // -----------------------------------
        ch->scopeLoopQuirk     = false;
        ch->scopeEnabled       = false;
        ch->scopeTrigger       = false;
        ch->scopeLoopFlag      = false;
        ch->scopeKeepPeriod    = true;
        ch->scopeKeepVolume    = true;
        ch->tempFlags          = 0;
        ch->tempFlagsBackup    = 0;
        ch->tempPeriod         = 0;
    }

    tempFlags        = 0;
    pattBreakBugPos  = -1;
    pattBreakFlag    = false;
    pattDelayFlag    = false;
    forceEffectsOff  = false;
    PattDelayTime    = 0;
    PattDelayTime2   = 0;
    PBreakPosition   = 0;
    PosJumpAssert    = false;
    modHasBeenPlayed = true;
}

void playPattern(int8_t startRow)
{
    uint8_t i;

    for (i = 0; i < PAULA_CHANNELS; ++i)
        source->channels[i].chanIndex = i;

    source->row        = startRow & 0x3F;
    source->currRow    = source->row;
    editor.modTick     = 0;
    tempFlags          = 0;
    editor.playMode    = PLAY_MODE_PATTERN;
    editor.currentMode = MODE_PLAY;

    if (!editor.stepPlayEnabled)
        pointerSetMode(POINTER_MODE_PLAY, DO_CARRY);

    editor.songPlaying = true;
}

void incPatt(void)
{
    if (++modPattern > (MAX_PATTERNS - 1))
          modPattern = 0;

    source->currPattern = modPattern;
}

void decPatt(void)
{
    if (--modPattern < 0)
          modPattern = MAX_PATTERNS - 1;

    source->currPattern = modPattern;
}

void modPlay(int16_t patt, int16_t order, int8_t row)
{
    // this routine is kinda spaghetti...

    uint8_t i;

    mod_channel *ch;

    if (row != -1)
    {
        if ((row >= 0) && (row <= 63))
        {
            source->row     = row;
            source->currRow = row;
        }
    }
    else
    {
        source->row     = 0;
        source->currRow = 0;
    }

    if (editor.playMode != PLAY_MODE_PATTERN)
    {
        if (modOrder >= source->head.orderCount)
        {
            modOrder          = 0;
            source->currOrder = 0;
        }

        if ((order >= 0) && (order < source->head.orderCount))
        {
            modOrder          = order;
            source->currOrder = order;
        }
    }

    if ((patt >= 0) && (patt <= (MAX_PATTERNS - 1)))
    {
        modPattern          = patt;
        source->currPattern = patt;
    }
    else
    {
        modPattern          = source->head.order[modOrder];
        source->currPattern = source->head.order[modOrder];
    }

    editor.currentPatternDisp   = &source->head.order[modOrder];
    editor.currentPosEdPattDisp = &source->head.order[modOrder];

    mixerKillAllVoices();

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        ch = &source->channels[i];

        ch->chanIndex          = i;
        ch->fineTune           = 0;
        ch->flags              = 0;
        ch->noNote             = true;
        ch->period             = 0;
        ch->sample             = 0;
        ch->wantedperiod       = 0;
        ch->offset             = 0;
        ch->volume             = 0;
        ch->offsetBugNotAdded  = true;
        ch->patternLoopCounter = 0;
        ch->glissandoControl   = 0;
        ch->waveControl        = 0;
        ch->scopeLoopQuirk     = false;
        ch->scopeEnabled       = false;
        ch->scopeTrigger       = false;
        ch->scopeLoopFlag      = false;
        ch->scopeKeepPeriod    = true;
        ch->scopeKeepVolume    = true;
        ch->tempFlags          = 0;
        ch->tempFlagsBackup    = 0;
        ch->tempPeriod         = 0;
        ch->DidQuantize        = false;
    }

    if (editor.playMode == PLAY_MODE_NORMAL)
    {
        editor.ticks50Hz = 0;
        editor.playTime  = 0;
    }

    editor.modTick     = 0;
    tempFlags          = 0;
    tempPeriod         = 0;
    PBreakPosition     = 0;
    PattDelayTime      = 0;
    PattDelayTime2     = 0;
    pattBreakBugPos    = -1;
    PosJumpAssert      = false;
    pattBreakFlag      = false;
    pattDelayFlag      = false;
    PBreakFlag         = false;
    PosJumpAssert      = false;
    forceEffectsOff    = false;
    modHasBeenPlayed   = false;
    editor.songPlaying = true;
}

void clearSong(void)
{
    uint8_t i;

    if (source != NULL)
    {
        memset(editor.ui.pattNames,      0, MAX_PATTERNS * 16);
        memset(source->head.order,       0, sizeof (source->head.order));
        memset(source->head.moduleTitle, 0, sizeof (source->head.moduleTitle));

        editor.muted[0]           = false;
        editor.muted[1]           = false;
        editor.muted[2]           = false;
        editor.muted[3]           = false;
        source->head.orderCount   = 1;
        source->head.patternCount = 1;
        editor.ticks50Hz          = 0;
        editor.playTime           = 0;
        editor.currentSample      = 0;
        editor.BlockMarkFlag      = false;
        editor.swapChannelFlag    = false;
        editor.EditMoveAdd        = 1;
        editor.F6pos              = 0;
        editor.F7pos              = 16;
        editor.F8pos              = 32;
        editor.F9pos              = 48;
        editor.F10pos             = 63;

        for (i = 0; i < MAX_PATTERNS; ++i)
            memset(source->patterns[i], 0, (MOD_ROWS * PAULA_CHANNELS) * sizeof (modnote_t));

        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            source->channels[i].waveControl        = 0;
            source->channels[i].patternLoopCounter = 0;
            source->channels[i].fineTune           = 0;
            source->channels[i].invertLoopSpeed    = 0;
            source->channels[i].glissandoControl   = 0;
        }

        modSetPos(0, 0);

        source->currPattern         = 0;
        source->currOrder           = 0;
        editor.currentPatternDisp   = &source->head.order[0];
        editor.currentPosEdPattDisp = &source->head.order[0];

        modSetTempo(editor.initialTempo);
        modSetSpeed(editor.initialSpeed);

        setLEDFilter(false); // real PT doesn't do this there, but that's insane

        calcRealModSize(source);
        updateWindowTitle(source, MOD_IS_MODIFIED);

        editor.ui.editOpScreenShown = false;
        editor.ui.aboutScreenShown  = false;
    }
}

void clearSamples(void)
{
    uint8_t i;

    if (source != NULL)
    {
        for (i = 0; i < MOD_SAMPLES; ++i)
        {
            source->samples[i].fineTune   = 0;
            source->samples[i].length     = 0;
            source->samples[i].loopLength = 2;
            source->samples[i].loopStart  = 0;
            source->samples[i].volume     = 0;

            memset(source->samples[i].text, 0, sizeof (source->samples[i].text));
        }

        memset(source->sampleData, 0, MOD_SAMPLES * 0x0001FFFE);

        editor.currentSample        = 0;
        editor.keypadSampleOffset   = 0;
        editor.SampleZero           = false;
        editor.ui.editOpScreenShown = false;
        editor.ui.aboutScreenShown  = false;
        editor.BlockMarkFlag        = false;

        setupSampler(source, REMOVE_SAMPLE_MARKING);

        calcRealModSize(source);
        updateWindowTitle(source, MOD_IS_MODIFIED);
    }
}

void clearAll(void)
{
    if (source != NULL)
    {
        clearSamples();
        clearSong();
    }
}

void modFree(MODULE* s)
{
    uint8_t i;

    if (s != NULL)
    {
        for (i = 0; i < MAX_PATTERNS; ++i)
        {
            if (s->patterns[i] != NULL)
            {
                free(s->patterns[i]);
                s->patterns[i] = NULL;
            }
        }

        if (s->sampleData != NULL)
        {
            free(s->sampleData);
            s->sampleData = NULL;
        }

        free(s);
        s = NULL;
    }
}

uint8_t getSongProgressInPercentage(void)
{
    return (uint8_t)((((float)(source->rowsCounter) / (float)(source->rowsInTotal)) * 100.0f));
}

void restartSong(void)
{
    if (editor.songPlaying)
        modStop();

    editor.BlockMarkFlag = false;

    forceMixerOff = true;

    mixerKillAllVoices();

    editor.playMode     = PLAY_MODE_NORMAL;
    source->row         = 0;
    source->currRow     = 0;
    source->rowsCounter = 0;

    memset(editor.rowVisitTable, 0, MOD_ORDERS * MOD_ROWS); // for MOD2WAV

    if (editor.isSMPRendering)
    {
        modPlay(DONT_SET_PATTERN, DONT_SET_ORDER, DONT_SET_ROW);
    }
    else
    {
        source->currBPM = 125;
        modSetSpeed(6);
        modSetTempo(125);

        modPlay(DONT_SET_PATTERN, 0, 0);
    }
}

// this function is meant for MOD2WAV or PAT2SMP
void resetSong(void) // only call this after setTempVariables() has been called!
{
    int8_t i;

    editor.songPlaying = false;
    editor.playMode    = PLAY_MODE_NORMAL;
    editor.currentMode = MODE_IDLE;

    mixerKillAllVoices();

    forceMixerOff = false;

    modStop();

    memset((int8_t *)(editor.vuMeterVolumes),  0, sizeof (editor.vuMeterVolumes));
    memset((int8_t *)(editor.spectrumVolumes), 0, sizeof (editor.spectrumVolumes));

    memset(source->channels, 0, sizeof (source->channels));
    for (i = 0; i < PAULA_CHANNELS; ++i)
        source->channels[i].chanIndex = i;

    modOrder            = oldOrder;
    modPattern          = oldPattern;
    source->row         = oldRow;
    source->currRow     = oldRow;
    source->currOrder   = oldOrder;
    source->currPattern = oldPattern;
    source->currBPM     = oldBPM;

    editor.currentEditPatternDisp = &source->currPattern;
    editor.currentPosDisp         = &source->currOrder;
    editor.currentPatternDisp     = &source->head.order[source->currOrder];
    editor.currentPosEdPattDisp   = &source->head.order[source->currOrder];

    modSetSpeed(oldSpeed);
    modSetTempo(oldBPM);

    editor.modTick   = 0;
    tempFlags        = 0;
    PBreakPosition   = 0;
    PosJumpAssert    = false;
    pattBreakBugPos  = -1;
    pattBreakFlag    = false;
    pattDelayFlag    = false;
    forceEffectsOff  = false;
    PattDelayTime    = 0;
    PattDelayTime2   = 0;
    PBreakFlag       = false;
    modHasBeenPlayed = false;
}
