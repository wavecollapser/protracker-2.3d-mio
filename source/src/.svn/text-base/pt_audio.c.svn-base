/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#define _USE_MATH_DEFINES

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <SDL/SDL.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits.h>
#include "pt_header.h"
#include "pt_helpers.h"
#include "pt_blep.h"
#include "pt_config.h"
#include "pt_tables.h"
#include "pt_palette.h"
#include "pt_textout.h"

enum
{
    FILTER_LP_ENABLED  = 1,
    FILTER_LED_ENABLED = 2
};

typedef struct lossyIntegrator_t
{
    float buffer[2];
    float coefficient[2];

} lossyIntegrator_t;

typedef struct LEDFilter_t
{
    float LED[4];
} LEDFilter_t;

typedef struct LEDFilterCoeff_t
{
    float LED;
    float LEDFb;
} LEDFilterCoeff_t;

typedef struct voice_data
{
    const int8_t *newData;
    const int8_t *data;
    int8_t newSample;
    int8_t swapSampleFlag;
    int8_t loopFlag;
    int8_t newLoopFlag;

    int32_t length;
    int32_t loopBegin;
    int32_t loopEnd;
    int32_t newLength;
    int32_t newLoopBegin;
    int32_t newLoopEnd;
    int32_t newSampleOffset;
    int32_t index;
    int32_t loopQuirk;

    float vol;
    float panL;
    float panR;
    float fraction;
    float delta;

    // must store state because samples are read out ahead of applying delta
    // changes to the output depend upon state[-1] (from 1 sample ago)
    // TODO: read samples inside the check (fraction >= 1.0)
    float lastFraction;
    float lastDelta;
} Voice;

static volatile int8_t filterFlags = FILTER_LP_ENABLED;
static int8_t amigaPanFlag         = false;
static int8_t defStereoSep         = 25;
int8_t forceMixerOff               = false;

static uint16_t PAN1;
static uint16_t PAN2;
static uint16_t PAN3;
static uint16_t PAN4;

int32_t samplesPerTick;
static int32_t sampleCounter;
static int32_t renderSampleCounter;

static float *mixBufferL;
static float *mixBufferR;

static BLEP blep[PAULA_CHANNELS];
static BLEP blepVol[PAULA_CHANNELS];
static lossyIntegrator_t filterLo;
static lossyIntegrator_t filterHi;
static LEDFilterCoeff_t filterLEDC;
static LEDFilter_t filterLED;
static Voice voice[PAULA_CHANNELS];

extern MODULE *mod; // pt_main.c

int8_t processTick(void);      // defined in pt_modplayer.c
void storeTempVariables(void); // defined in pt_modplayer.c

void calcMod2WavTotalRows(void);

void setLEDFilter(uint8_t state)
{
    editor.useLEDFilter = state;

    if (editor.useLEDFilter)
        filterFlags |= FILTER_LED_ENABLED;
    else
        filterFlags &= ~FILTER_LED_ENABLED;
}

void toggleLEDFilter(void)
{
    editor.useLEDFilter ^= 1;

    if (editor.useLEDFilter)
        filterFlags |= FILTER_LED_ENABLED;
    else
        filterFlags &= ~FILTER_LED_ENABLED;
}

static float calcCoeffLED(float sr, float hz)
{
    if (hz >= (sr / 2.0f))
        return (1.0f);

    return ((2.0f * 3.141592f) * hz / sr);
} 

static void calcCoeffLossyIntegrator(float sr, float hz, lossyIntegrator_t *filter)
{
    filter->coefficient[0] = tanf((float)(M_PI) * hz / sr);
    filter->coefficient[1] = 1.0f / (1.0f + filter->coefficient[0]);
}

static void clearLEDFilter(LEDFilter_t *filter)
{
    filter->LED[0] = 0.0f;
    filter->LED[1] = 0.0f;
    filter->LED[2] = 0.0f;
    filter->LED[3] = 0.0f;
}

static void clearLossyIntegrator(lossyIntegrator_t *filter)
{
    filter->buffer[0] = 0.0f;
    filter->buffer[1] = 0.0f;
}

static inline void lossyIntegratorLED(LEDFilterCoeff_t filterC, LEDFilter_t *filter, float *in, float *out)
{
    // left channel LED filter
    filter->LED[0] += (filterC.LED * (in[0] - filter->LED[0])
        + filterC.LEDFb * (filter->LED[0] - filter->LED[1]) + 1e-10f);
    filter->LED[1] += (filterC.LED * (filter->LED[0] - filter->LED[1]) + 1e-10f);
    out[0] = filter->LED[1];

    // right channel LED filter
    filter->LED[2] += (filterC.LED * (in[1] - filter->LED[2])
        + filterC.LEDFb * (filter->LED[2] - filter->LED[3]) + 1e-10f);
    filter->LED[3] += (filterC.LED * (filter->LED[2] - filter->LED[3]) + 1e-10f);
    out[1] = filter->LED[3];
}

static inline void lossyIntegrator(lossyIntegrator_t *filter, float *in, float *out)
{
    float output;

    // left channel low-pass
    output            = (filter->coefficient[0] * in[0] + filter->buffer[0]) * filter->coefficient[1];
    filter->buffer[0] = filter->coefficient[0] * (in[0] - output) + output + 1e-10f;
    out[0]            = output;

    // right channel low-pass
    output            = (filter->coefficient[0] * in[1] + filter->buffer[1]) * filter->coefficient[1];
    filter->buffer[1] = filter->coefficient[0] * (in[1] - output) + output + 1e-10f;
    out[1]            = output;
}

static inline void lossyIntegratorHighPass(lossyIntegrator_t *filter, float *in, float *out)
{
    float low[2];

    lossyIntegrator(filter, in, low);

    out[0] = in[0] - low[0];
    out[1] = in[1] - low[1];
}

static inline void updateScope(mod_channel *ch, Voice *v)
{
    ch->scopeLoopQuirk = false;
    ch->scopeLoopFlag  = v->loopFlag;
    ch->scopePos       = v->newSampleOffset + v->index;
    ch->scopeEnd       = v->newSampleOffset + v->length;
    ch->scopeLoopBegin = v->newSampleOffset + v->loopBegin;
    ch->scopeLoopEnd   = v->newSampleOffset + v->loopEnd;
}

static inline void updateSampleData(Voice *v)
{
    v->loopBegin    = v->newLoopBegin;
    v->loopEnd      = v->newLoopEnd;
    v->loopFlag     = v->newLoopFlag;
    v->data         = v->newData;
    v->length       = v->newLength;
    v->fraction     = 0.0f;
    v->lastFraction = 0.0f;
}

void mixerSwapChSource(uint8_t ch, const int8_t *src, int32_t length, int32_t loopStart, int32_t loopLength, int32_t newSampleOffset)
{
    Voice *v;

    // If you swap sample in-realtime in PT
    // without putting a period, the current
    // sample will play through first (either
    // >len or >loop_len), then when that is
    // reached you change to the new sample you
    // put earlier (if new sample is looped)

    if (length > 0x0001FFFE)
        return;

    if ((loopLength > 2) && (((loopStart + loopLength) > length) || (loopStart >= length)))
        return;

    v = &voice[ch];

    v->loopQuirk       = false;
    v->newSampleOffset = newSampleOffset;
    v->swapSampleFlag  = true;
    v->newData         = src;
    v->newLoopFlag     = (loopStart + loopLength) > 2;
    v->newLength       = length;
    v->newLoopBegin    = loopStart;
    v->newLoopEnd      = loopStart + loopLength;

    // if the mixer was already shut down earlier after a non-loop swap,
    // force swap again, but only if the new sample has loop enabled (ONLY!)
    if ((v->data == NULL) && v->newLoopFlag)
    {
        updateSampleData(v);

        // we need to wrap here for safety reasons
        while (v->index >= v->loopEnd)
            v->index = v->loopBegin + (v->index - v->loopEnd);

        updateScope(&mod->channels[ch], v);
        mod->channels[ch].scopeTrigger = true;
    }
}

void mixerSetChSource(uint8_t ch, const int8_t *src, int32_t length, int32_t loopStart, int32_t loopLength, int32_t offset)
{
    Voice *v;

    v = &voice[ch];

    // PT quirk: LENGTH >65534 + effect 9xx (any number) = shut down voice
    if ((length > 65534) && (offset > 0))
    {
        v->data  = NULL;
        return;
    }

    if ((length > 0x0001FFFE) || (loopStart >= length) || ((loopStart + loopLength) > length))
        return;

    v->loopQuirk      = false;
    v->swapSampleFlag = false;
    v->data           = src;
    v->index          = offset;
    v->fraction       = 0.0f;
    v->lastFraction   = 0.0f;
    v->length         = length;
    v->loopFlag       = (loopStart + loopLength) > 2;
    v->loopBegin      = loopStart;
    v->loopEnd        = loopStart + loopLength;

    // Check external 9xx usage (Set Sample Offset)
    if (v->loopFlag)
    {
        if (offset >= v->loopEnd)
            v->index = v->loopBegin;
    }
    else
    {
        if (offset >= v->length)
            v->data = NULL;
    }

    if ((loopLength > 2) && (loopStart == 0))
    {
        v->loopQuirk = v->loopEnd;
        v->loopEnd   = v->length;
    }
}

// adejr: these sin/cos approximations both use a 0..1
// parameter range and have 'normalized' (1/2 = 0db) coefficients
//
// the coefficients are for LERP(x, x * x, 0.224) * sqrt(2)
// max_error is minimized with 0.224 = 0.0013012886

static float sinApx(float x)
{
    x = x * (2.0f - x);
    return (x * 1.09742972f + x * x * 0.31678383f);
}

static float cosApx(float x)
{
    x = (1.0f - x) * (1.0f + x);
    return (x * 1.09742972f + x * x * 0.31678383f);
}

static void mixerSetChPan(uint8_t ch, uint16_t pan) // pan = 0..256
{
    float p;

    // proper 'normalized' equal-power panning is (assuming pan left to right):
    // L = cos(p * pi * 1/2) * sqrt(2);
    // R = sin(p * pi * 1/2) * sqrt(2);

    p = pan * (1.0f / 256.0f);

    voice[ch].panL = cosApx(p);
    voice[ch].panR = sinApx(p);
}

void mixerKillVoice(uint8_t ch)
{
    float tmpDelta;
    float tmpVol;
    float tmpPanL;
    float tmpPanR;

    SDL_LockAudio();

    tmpDelta = voice[ch].delta;
    tmpVol   = voice[ch].vol;
    tmpPanL  = voice[ch].panL;
    tmpPanR  = voice[ch].panR;

    memset(&voice[ch],   0, sizeof (Voice));
    memset(&blep[ch],    0, sizeof (BLEP));
    memset(&blepVol[ch], 0, sizeof (BLEP));

    voice[ch].delta = tmpDelta;
    voice[ch].vol   = tmpVol;
    voice[ch].panL  = tmpPanL;
    voice[ch].panR  = tmpPanR;

    SDL_UnlockAudio();
}

void mixerKillAllVoices(void)
{
    SDL_LockAudio();

    memset(voice,   0, sizeof (voice));
    memset(blep,    0, sizeof (blep));
    memset(blepVol, 0, sizeof (blepVol));

    clearLossyIntegrator(&filterLo);
    clearLossyIntegrator(&filterHi);
    clearLEDFilter(&filterLED);

    editor.tuningFlag = false;

    mixerSetChPan(0, PAN1);
    mixerSetChPan(1, PAN2);
    mixerSetChPan(2, PAN3);
    mixerSetChPan(3, PAN4);

    SDL_UnlockAudio();
}

void mixerKillVoiceIfReadingSample(MODULE *modEntry, uint8_t sample)
{
    uint8_t i;

    mod_channel *ch;

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        if (voice[i].data == &modEntry->sampleData[modEntry->samples[sample].offset])
        {
            mixerKillVoice(i);

            ch = &modEntry->channels[i];

            ch->scopeEnabled    = false;
            ch->scopeTrigger    = false;
            ch->scopeLoopFlag   = false;
            ch->scopeKeepPeriod = true;
            ch->scopeKeepVolume = true;
        }
    }
}

void mixerSetChVol(uint8_t ch, int8_t vol)
{
    voice[ch].vol = (float)(vol) * (1.0f / 64.0f);
}

void mixerSetChDelta(uint8_t ch, uint16_t period)
{
    Voice *v;

    if ((period >= 78) && (period <= 937))
    {
        v = &voice[ch];

        v->delta = editor.isSMPRendering ? (editor.pat2SmpHQ ? deltaTable28kHz[period] : deltaTable22kHz[period]) : deltaTable[period];
        if (v->lastDelta == 0.0f)
            v->lastDelta = v->delta;
    }
}

static inline void insertEndingBlep(BLEP *b, Voice *v)
{
    if (b->lastValue != 0.0f)
    {
        if ((v->lastDelta > 0.0f) && (v->lastDelta > v->lastFraction))
            blepAdd(b, v->lastFraction / v->lastDelta, b->lastValue);

        b->lastValue = 0.0f;
    }
}

void UpdateVoiceParameters(MODULE *modEntry)
{
    uint8_t i;

    MODULE_SAMPLE *s;

    mod_channel *ch;

    Voice *v;

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        if (voice[i].data == &modEntry->sampleData[modEntry->samples[editor.currentSample].offset])
        {
            s  = &modEntry->samples[editor.currentSample];
            ch = &modEntry->channels[i];
            v  = &voice[i];

            v->loopBegin       = s->loopStart;
            v->loopEnd         = s->loopStart + s->loopLength;
            v->length          = s->length;
            ch->scopeLoopQuirk = false;
            ch->scopeLoopBegin = s->offset + v->loopBegin;
            ch->scopeLoopEnd   = s->offset + v->loopEnd;
            ch->scopeEnd       = s->offset + s->length;
        }
    }
}

void toggleLowPassFilter(void)
{
    if (filterFlags & FILTER_LP_ENABLED)
    {
        filterFlags &= ~FILTER_LP_ENABLED;

        displayMsg("FILTER MOD: A1200");
    }
    else
    {
        filterFlags |= FILTER_LP_ENABLED;
        clearLossyIntegrator(&filterLo);

        input.mouse.buttonErrorMsgActive = true;
        input.mouse.errorMsgCounter = 24; // medium short flash

        displayMsg("FILTER MOD: A500");
    }
}

static void outputAudio(int16_t *target, int32_t numSamples)
{
    uint8_t i;

    int16_t *outStream;

    int32_t j;

    float tempSample;
    float tempVolume;
    float out[2];

    BLEP *bSmp;
    BLEP *bVol;

    Voice *v;

    mod_channel *ch;

    memset(mixBufferL, 0, numSamples * sizeof (float));
    memset(mixBufferR, 0, numSamples * sizeof (float));

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        if (voice[i].data)
        {
            v    = &voice[i];
            bSmp = &blep[i];
            bVol = &blepVol[i];
            ch   = &mod->channels[i];

            j = 0;
            for (; j < numSamples; ++j)
            {
                tempSample = v->data ? ((float)(v->data[v->index]) * (1.0f / 128.0f)) : 0.0f;
                tempVolume = v->data ? v->vol : 0.0f;

                // if sample data changes, anti-alias the step
                // this code assumes index can't change >1 per sample
                if (tempSample != bSmp->lastValue)
                {
                    if ((v->lastDelta > 0.0f) && (v->lastDelta > v->lastFraction))
                        blepAdd(bSmp, v->lastFraction / v->lastDelta, bSmp->lastValue - tempSample);

                    bSmp->lastValue = tempSample;
                }

                // if volume data changes, anti-alias the step
                if (tempVolume != bVol->lastValue)
                {
                    PT_ASSERT(tempVolume <= 1.0f);

                    blepAdd(bVol, 0.0f, bVol->lastValue - tempVolume);
                    bVol->lastValue = tempVolume;
                }

                // add sample anti-alias data to sample value and scale by volume
                if (bSmp->samplesLeft) tempSample += blepRun(bSmp);

                // add volume anti-alias pulse data to volume value
                if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                tempSample    *= tempVolume;
                mixBufferL[j] += (tempSample * v->panL);
                mixBufferR[j] += (tempSample * v->panR);

                if (v->data)
                {
                    v->fraction += v->delta;

                    if (v->fraction >= 1.0f)
                    {
                        v->index++;
                        v->fraction -= 1.0f;

                        // store last updated state for blep fraction
                        v->lastFraction = v->fraction;
                        v->lastDelta    = v->delta;

                        PT_ASSERT(v->lastDelta > v->lastFraction);

                        if (v->loopFlag)
                        {
                            if (v->index >= v->loopEnd)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;
 
                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++; // we inserted an ending blep sample

                                        break;
                                    }
 
                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    if (v->loopQuirk)
                                    {
                                        v->loopEnd   = v->loopQuirk;
                                        v->loopQuirk = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (v->index >= v->length)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;

                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++; // we inserted an ending blep sample

                                        break;
                                    }

                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    v->data = NULL;

                                    insertEndingBlep(bSmp, v);
                                    j++; // we inserted an ending blep sample

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            // sometimes a channel might be empty,
            // but the blep-buffer still contains data.
            if ((j < numSamples) && (v->data == NULL) && (bSmp->samplesLeft || bVol->samplesLeft))
            {
                for (; j < numSamples; ++j)
                {
                    tempSample = bSmp->lastValue;
                    tempVolume = bVol->lastValue;

                    if (bSmp->samplesLeft) tempSample += blepRun(bSmp);
                    if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                    tempSample    *= tempVolume;
                    mixBufferL[j] += (tempSample * v->panL);
                    mixBufferR[j] += (tempSample * v->panR);
                }
            }
        }
    }

    outStream = target;
    for (j = 0; j < numSamples; ++j)
    {
        out[0] = mixBufferL[j];
        out[1] = mixBufferR[j];

        if (filterFlags & FILTER_LP_ENABLED)
            lossyIntegrator(&filterLo, out, out);

        if (filterFlags & FILTER_LED_ENABLED)
            lossyIntegratorLED(filterLEDC, &filterLED, out, out);

        lossyIntegratorHighPass(&filterHi, out, out);

        // - Highpass doubles channel peak
        // - Panning adds sqrt(2) at 50% and 100% wide pans
        // - Minimal correlation means mixing channels can result in sqrt(2), not 2
        // - All this taken into account makes the best-case level of 2 okay,
        // although 8*sqrt(2)*little from LED filter overshoot is possible worst-case!
        // - Even in the best case without pan and without high-pass, the level for
        // chiptunes using amp 64 pulse waves will be 4. With pan, 2 * sqrt(2) (2.83).
        // - It is likely that the majority of modules will be peaking around 3.
        // - Also, the signal should be phase inverted, hence -32767.0f
        out[0] *= (-32767.0f / 3.0f);
        out[1] *= (-32767.0f / 3.0f);

        // clamp in case of overflow
        out[0] = CLAMP(out[0], -32768.0f, 32767.0f);
        out[1] = CLAMP(out[1], -32768.0f, 32767.0f);

        // convert to int and send to output
        *outStream++ = (int16_t)(out[0]);
        *outStream++ = (int16_t)(out[1]);
    }
}

void audioCallback(void *userdata, Uint8 *stream, int len) // non-stdint to make SDL happy
{
    int16_t *out;

    int32_t sampleBlock;
    int32_t samplesTodo;

    (void)(userdata); // make compiler happy

    if (forceMixerOff) // For .MOD EEx + Dxx/Bxx quirk
        return;

    out         = (int16_t *)(stream);
    sampleBlock = len / 4;

    while (sampleBlock)
    {
        samplesTodo = (sampleBlock < sampleCounter) ? sampleBlock : sampleCounter;
        if (samplesTodo > 0)
        {
            outputAudio(out, samplesTodo);

            out           += (2 * samplesTodo); // 2 channels (stereo)
            sampleBlock   -= samplesTodo;
            sampleCounter -= samplesTodo;
        }
        else
        {
            if (editor.songPlaying)
                processTick();

            sampleCounter = samplesPerTick;
        }
    }
}

static float filterRCtoHZ(float R, float C)
{
    return (1.0f / (2.0f * (float)(M_PI) * R * C));
}

static float filterHZandRtoC(float R, float HZ)
{
    return (1.0f / (HZ * 2.0f * (float)(M_PI) * R));
}

static void calculateFilterCoefficients(uint32_t soundFrequency)
{
    float low_R;
    float high_R;
    float low_C;
    float high_C;
    float low_hz;
    float high_hz;

    // Amiga 500 filter emulation
    //
    // First comes a static low-pass 6dB formed by the supply current
    // from the Paula's mixture of channels A+B / C+D into the opamp with
    // 100nF capacitor and 360 ohm resistor feedback in inverting mode biased by
    // dac vRef (used to center the output).
    //
    // R = 360 ohm
    // C = 100nF
    // Low Hz = 4420.97~ = 1 / (2pi * 360 * 0.0000001)
    //
    // Under spice simulation the circuit yields -3dB = 4400Hz.
    // In the A1200, the cutoff is 26000Hz, so the static filter is disabled.
    //
    // Next comes a bog-standard Sallen-Key filter with:
    // R  = 10k ohm
    // C0 = 6800pF
    // C1 = 3900pF
    // Q ~= 1/sqrt(2)
    //
    // This filter is optionally bypassed by an MPF-102 JFET when
    // the LED filter is disabled.
    //
    // Under spice simulation the circuit yields -3dB = 2800Hz.
    // 90 degrees phase = 3000Hz (so, should oscillate at 3kHz!)
    //
    // The buffered output of the Sallen-Key passes into an RC high-pass with:
    // C = 22uF (also C = 330nF, for improved high-frequency)
    // R = 1.39k ohm
    //
    // High Hz = 5.2~ = 1 / (2pi * 0.000022 * 1390)
    // Under spice simulation the circuit yields -3dB = 5.2Hz.

    filterLEDC.LED   = calcCoeffLED((float)(soundFrequency), 3090.0f);
    filterLEDC.LEDFb = 0.125f + 0.125f / (1.0f - filterLEDC.LED); // Fb = 0.125 : Q ~= 1/sqrt(2) (Butterworth)

    low_R  = 750.0f;
    high_R = 2000.0f;

    low_C  = filterHZandRtoC(low_R,  5000.0f); // 5kHz is actually closer to what a typical A500/A4000 sounds like
    high_C = filterHZandRtoC(high_R, 5.2f);
    low_hz  = filterRCtoHZ(low_R,  low_C);
    high_hz = filterRCtoHZ(high_R, high_C);

    calcCoeffLossyIntegrator((float)(soundFrequency),  low_hz, &filterLo);
    calcCoeffLossyIntegrator((float)(soundFrequency), high_hz, &filterHi);
}

void mixerCalcChPans(uint8_t stereoSeparation)
{
    uint8_t scaledPanPos;

    scaledPanPos = (stereoSeparation * 128) / 100;

    PAN1 = 128 - scaledPanPos;
    PAN2 = 128 + scaledPanPos;
    PAN3 = 128 + scaledPanPos;
    PAN4 = 128 - scaledPanPos;

    mixerSetChPan(0, PAN1);
    mixerSetChPan(1, PAN2);
    mixerSetChPan(2, PAN3);
    mixerSetChPan(3, PAN4);
}

int8_t createFrequencyTables(uint32_t soundFrequency)
{
    uint16_t i;

    float per;
    float freq;

    audio.tempoTimerVal = PT_STD_BPM * soundFrequency;
    freq                = (float)(editor.outputFreq);

    editor.mod2WavBuffer = (int16_t *)(malloc(sizeof (int16_t) * ((uint32_t)(freq / 12.8f) * 2)));
    if (editor.mod2WavBuffer == NULL)
        return (false);

    // PT doesn't clamp 4xx (vib) period, so
    // 78..937 is the new range.
    // Paula can barely do anything below period 120,
    // but let's not make this too complex.

    deltaTable = (float *)(malloc(sizeof (float) * 938));
    if (deltaTable == NULL)
        return (false);

    deltaTable22kHz = (float *)(malloc(sizeof (float) * 938));
    if (deltaTable22kHz == NULL)
        return (false);

    deltaTable28kHz = (float *)(malloc(sizeof (float) * 938));
    if (deltaTable28kHz == NULL)
        return (false);

    deltaTable[0]      = PAL_AMIGA_PAULA_CC_f / freq;
    deltaTable22kHz[0] = PAL_AMIGA_PAULA_CC_f / 22168.0f;
    deltaTable28kHz[0] = PAL_AMIGA_PAULA_CC_f / 28836.0f;

    per = 1.0f;
    for (i = 1; i < 938; ++i)
    {
        deltaTable[i]      = (PAL_AMIGA_PAULA_CC_f / per) / freq;
        deltaTable22kHz[i] = (PAL_AMIGA_PAULA_CC_f / per) / 22168.0f;
        deltaTable28kHz[i] = (PAL_AMIGA_PAULA_CC_f / per) / 28836.0f;

        per += 1.0f;
    }

    return (true);
}

int8_t setupAudio(const PTCONFIG *config)
{
    SDL_AudioSpec fmt;

    editor.outputFreq = config->soundFrequency;

    mixerCalcChPans(config->stereoSeparation);
    defStereoSep = config->stereoSeparation;

    if (config->a4000LowPassFilter)
        filterFlags = FILTER_LP_ENABLED;
    else
        filterFlags = 0;

    fmt.freq     = config->soundFrequency;
    fmt.format   = AUDIO_S16SYS;
    fmt.channels = 2;
    fmt.callback = audioCallback;
    fmt.userdata = NULL;
    fmt.samples  = 1024; // should be 2^n for compatibility with all sound cards

    if (!createFrequencyTables(config->soundFrequency))
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    mixBufferL = (float *)(calloc(fmt.samples, sizeof (float)));
    if (mixBufferL == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    mixBufferR = (float *)(calloc(fmt.samples, sizeof (float)));
    if (mixBufferR == NULL)
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    calculateFilterCoefficients(config->soundFrequency);

    if (SDL_OpenAudio(&fmt, NULL) < 0)
    {
        fprintf(stderr, "ERROR: Unable to open audio: %s\n", SDL_GetError());
        return (false);
    }

    samplesPerTick = 0;
    sampleCounter  = 0;

    SDL_PauseAudio(false);

    return (true);
}

void audioClose(void)
{
    editor.songPlaying = false;

    mixerKillAllVoices();
    SDL_PauseAudio(true);

    SDL_CloseAudio();

    if (mixBufferL != NULL)
    {
        free(mixBufferL);
        mixBufferL = NULL;
    }

    if (mixBufferR != NULL)
    {
        free(mixBufferR);
        mixBufferR = NULL;
    }

    if (editor.mod2WavBuffer != NULL)
    {
        free(editor.mod2WavBuffer);
        editor.mod2WavBuffer = NULL;
    }

    if (deltaTable != NULL)
    {
        free(deltaTable);
        deltaTable = NULL;
    }

    // these two are for PAT2SMP
    if (deltaTable22kHz != NULL)
    {
        free(deltaTable22kHz);
        deltaTable22kHz = NULL;
    }

    if (deltaTable28kHz != NULL)
    {
        free(deltaTable28kHz);
        deltaTable28kHz = NULL;
    }
    // -------------------------
}

void mixerSetSamplesPerTick(int32_t val)
{
    samplesPerTick = val;
}

void toggleAmigaPanMode(void)
{
    amigaPanFlag ^= 1;

    if (!amigaPanFlag)
    {
        mixerCalcChPans(defStereoSep);

        input.mouse.buttonErrorMsgActive = true;
        input.mouse.errorMsgCounter = 24; // medium short flash

        setStatusMessage("AMIGA PANNING OFF", NO_CARRY);
    }
    else
    {
        mixerCalcChPans(100);

        input.mouse.buttonErrorMsgActive = true;
        input.mouse.errorMsgCounter = 24; // medium short flash

        setStatusMessage("AMIGA PANNING ON", NO_CARRY);
    }
}

// PAT2SMP RELATED STUFF
void outputAudioToSample(int32_t numSamples)
{
    uint8_t i;

    int32_t j;

    float tempSample;
    float tempVolume;
    float out[2];

    BLEP *bSmp;
    BLEP *bVol;

    Voice *v;

    mod_channel *ch;

    memset(mixBufferL, 0, numSamples * sizeof (float));

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        if (voice[i].data)
        {
            v    = &voice[i];
            bSmp = &blep[i];
            bVol = &blepVol[i];
            ch   = &mod->channels[i];

            j = 0;
            for (; j < numSamples; ++j)
            {
                tempSample = v->data ? ((float)(v->data[v->index]) * (1.0f / 128.0f)) : 0.0f;
                tempVolume = v->data ? v->vol : 0.0f;

                if (tempSample != bSmp->lastValue)
                {
                    if ((v->lastDelta > 0.0f) && (v->lastDelta > v->lastFraction))
                        blepAdd(bSmp, v->lastFraction / v->lastDelta, bSmp->lastValue - tempSample);

                    bSmp->lastValue = tempSample;
                }

                if (tempVolume != bVol->lastValue)
                {
                    blepAdd(bVol, 0.0f, bVol->lastValue - tempVolume);
                    bVol->lastValue = tempVolume;
                }

                if (bSmp->samplesLeft) tempSample += blepRun(bSmp);
                if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                tempSample    *= tempVolume;
                mixBufferL[j] += tempSample;

                if (v->data)
                {
                    v->fraction += v->delta;

                    if (v->fraction >= 1.0f)
                    {
                        v->index    += (int32_t)(v->fraction);
                        v->fraction -= (int32_t)(v->fraction);

                        v->lastFraction = v->fraction;
                        v->lastDelta    = v->delta;

                        if (v->loopFlag)
                        {
                            if (v->index >= v->loopEnd)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;

                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++;

                                        break;
                                    }

                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    if (v->loopQuirk)
                                    {
                                        v->loopEnd   = v->loopQuirk;
                                        v->loopQuirk = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (v->index >= v->length)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;

                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++;

                                        break;
                                    }

                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    v->data = NULL;

                                    insertEndingBlep(bSmp, v);
                                    j++;

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if ((j < numSamples) && (v->data == NULL) && (bSmp->samplesLeft || bVol->samplesLeft))
            {
                for (; j < numSamples; ++j)
                {
                    tempSample = bSmp->lastValue;
                    tempVolume = bVol->lastValue;

                    if (bSmp->samplesLeft) tempSample += blepRun(bSmp);
                    if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                    tempSample    *= tempVolume;
                    mixBufferL[j] += tempSample;
                }
            }
        }
    }

    for (j = 0; j < numSamples; ++j)
    {
        out[0] = mixBufferL[j];
        out[1] = mixBufferL[j];

        lossyIntegratorHighPass(&filterHi, out, out);

        out[0] = CLAMP(out[0] * (32767.0f / 3.0f), -32768.0f, 32767.0f);

        if (editor.pat2SmpPos < 0x0001FFFE)
        {
            editor.pat2SmpBuf[editor.pat2SmpPos] = (int16_t)(out[0]);
            editor.pat2SmpPos++;
        }
    }
}

// MOD2WAV RELATED STUFF
static void outputAudioToFile(FILE *fOut, int32_t numSamples)
{
    uint8_t i;

    int16_t *ptr;
    int16_t renderL;
    int16_t renderR;

    int32_t j;

    float tempSample;
    float tempVolume;
    float out[2];

    BLEP *bSmp;
    BLEP *bVol;

    Voice *v;

    mod_channel *ch;

    memset(mixBufferL, 0, numSamples * sizeof (float));
    memset(mixBufferR, 0, numSamples * sizeof (float));

    ptr = editor.mod2WavBuffer;

    for (i = 0; i < PAULA_CHANNELS; ++i)
    {
        if (voice[i].data)
        {
            v    = &voice[i];
            bSmp = &blep[i];
            bVol = &blepVol[i];
            ch   = &mod->channels[i];

            j = 0;
            for (; j < numSamples; ++j)
            {
                tempSample = v->data ? ((float)(v->data[v->index]) * (1.0f / 128.0f)) : 0.0f;
                tempVolume = v->data ? v->vol : 0.0f;

                if (tempSample != bSmp->lastValue)
                {
                    PT_ASSERT(v->lastDelta > 0.0f);
                    PT_ASSERT(v->lastDelta > v->lastFraction);

                    if (v->lastDelta > 0.0f)
                        blepAdd(bSmp, v->lastFraction / v->lastDelta, bSmp->lastValue - tempSample);

                    bSmp->lastValue = tempSample;
                }

                if (tempVolume != bVol->lastValue)
                {
                    PT_ASSERT(tempVolume <= 1.0f);

                    blepAdd(bVol, 0.0f, bVol->lastValue - tempVolume);
                    bVol->lastValue = tempVolume;
                }

                if (bSmp->samplesLeft) tempSample += blepRun(bSmp);
                if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                tempSample    *= tempVolume;
                mixBufferL[j] += (tempSample * v->panL);
                mixBufferR[j] += (tempSample * v->panR);

                if (v->data)
                {
                    v->fraction += v->delta;

                    if (v->fraction >= 1.0f)
                    {
                        v->index++;
                        v->fraction -= 1.0f;

                        v->lastFraction = v->fraction;
                        v->lastDelta    = v->delta;

                        PT_ASSERT(v->lastDelta > v->lastFraction);

                        if (v->loopFlag)
                        {
                            if (v->index >= v->loopEnd)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;

                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++;

                                        break;
                                    }

                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    if (v->loopQuirk)
                                    {
                                        v->loopEnd   = v->loopQuirk;
                                        v->loopQuirk = false;
                                    }
                                }
                            }
                        }
                        else
                        {
                            if (v->index >= v->length)
                            {
                                if (v->swapSampleFlag)
                                {
                                    v->swapSampleFlag = false;

                                    if (!v->newLoopFlag)
                                    {
                                        v->data           = NULL;
                                        ch->scopeLoopFlag = false;

                                        insertEndingBlep(bSmp, v);
                                        j++;

                                        break;
                                    }

                                    updateSampleData(v);

                                    while (v->index >= v->loopEnd)
                                        v->index = v->loopBegin + (v->index - v->loopEnd);

                                    ch->scopeLoopFlag = true;
                                    updateScope(ch, v);
                                }
                                else
                                {
                                    v->data = NULL;

                                    insertEndingBlep(bSmp, v);
                                    j++;

                                    break;
                                }
                            }
                        }
                    }
                }
            }

            if ((j < numSamples) && (v->data == NULL) && (bSmp->samplesLeft || bVol->samplesLeft))
            {
                for (; j < numSamples; ++j)
                {
                    tempSample = bSmp->lastValue;
                    tempVolume = bVol->lastValue;

                    if (bSmp->samplesLeft) tempSample += blepRun(bSmp);
                    if (bVol->samplesLeft) tempVolume += blepRun(bVol);

                    tempSample    *= tempVolume;
                    mixBufferL[j] += (tempSample * v->panL);
                    mixBufferR[j] += (tempSample * v->panR);
                }
            }
        }
    }

    for (j = 0; j < numSamples; ++j)
    {
        out[0] = mixBufferL[j];
        out[1] = mixBufferR[j];

        if (filterFlags & FILTER_LP_ENABLED)
            lossyIntegrator(&filterLo, out, out);

        if (filterFlags & FILTER_LED_ENABLED)
            lossyIntegratorLED(filterLEDC, &filterLED, out, out);

        lossyIntegratorHighPass(&filterHi, out, out);

        out[0] = CLAMP(out[0] * (-32767.0f / 3.0f), -32768.0f, 32767.0f);
        out[1] = CLAMP(out[1] * (-32767.0f / 3.0f), -32768.0f, 32767.0f);

        if (bigEndian)
        {
            renderL = SWAP16((int16_t)(out[0]));
            renderR = SWAP16((int16_t)(out[1]));
        }
        else
        {
            renderL = (int16_t)(out[0]);
            renderR = (int16_t)(out[1]);
        }

        // convert to int and send to output
        *ptr++ = renderL;
        *ptr++ = renderR;
    }

    fwrite(editor.mod2WavBuffer, sizeof (int16_t), 2 * numSamples, fOut);
}

int32_t mod2WavThreadFunc(void *ptr)
{
    int32_t renderSampleBlock;
    int32_t renderSamplesTodo;
    uint32_t totalSampleCounter;

    FILE *fOut;

    WAV_HEADER wave;

    fOut = (FILE *)(ptr);
    if (fOut == NULL)
        return (1);

    // skip wav header place, render data first
    fseek(fOut, sizeof (WAV_HEADER), SEEK_SET);

    pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
    setStatusMessage("CALCING SONG LEN", NO_CARRY);

    editor.isWAVRendering = true;

    calcMod2WavTotalRows();
    storeTempVariables();
    restartSong();

    editor.BlockMarkFlag = false;

    pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
    setStatusMessage("RENDERING MOD...", NO_CARRY);

    totalSampleCounter    = 0;
    renderSampleCounter   = 0;

    while (editor.isWAVRendering)
    {
        renderSampleBlock = editor.outputFreq / PAL_AMIGA_VBL_RATE;
        while (renderSampleBlock)
        {
            renderSamplesTodo = (renderSampleBlock < renderSampleCounter) ? renderSampleBlock : renderSampleCounter;
            if (renderSamplesTodo > 0)
            {
                outputAudioToFile(fOut, renderSamplesTodo);

                renderSampleBlock   -= renderSamplesTodo;
                renderSampleCounter -= renderSamplesTodo;
                totalSampleCounter  += renderSamplesTodo;
            }
            else
            {
                if (editor.songPlaying)
                {
                    if (!processTick())
                        editor.songPlaying = false;
                }
                else
                {
                    resetSong();
                    editor.isWAVRendering = false;
                    break; // rendering is done
                }

                renderSampleCounter = samplesPerTick;
            }
        }
    }

    // go back and fill the missing WAV header
    fseek(fOut, 0, SEEK_SET);

    wave.Format        = bigEndian ? SWAP32(0x45564157) : 0x45564157; // "WAVE"
    wave.ChunkID       = bigEndian ? SWAP32(0x46464952) : 0x46464952; // "RIFF"
    wave.Subchunk1ID   = bigEndian ? SWAP32(0x20746D66) : 0x20746D66; // "fmt "
    wave.Subchunk2ID   = bigEndian ? SWAP32(0x61746164) : 0x61746164; // "data"
    wave.Subchunk1Size = bigEndian ? SWAP32(16) : 16;
    wave.Subchunk2Size = bigEndian ? SWAP32(totalSampleCounter *  4) : (totalSampleCounter *  4); // 16-bit stereo = * 4
    wave.ChunkSize     = bigEndian ? SWAP32(wave.Subchunk2Size + 36) : (wave.Subchunk2Size + 36);
    wave.AudioFormat   = bigEndian ? SWAP16(1) : 1;
    wave.NumChannels   = bigEndian ? SWAP16(2) : 2;
    wave.BitsPerSample = bigEndian ? SWAP16(16) : 16;
    wave.SampleRate    = bigEndian ? SWAP32(editor.outputFreq) : editor.outputFreq;
    wave.ByteRate      = bigEndian ? SWAP32(wave.SampleRate * wave.NumChannels * wave.BitsPerSample / 8)
                            : (wave.SampleRate * wave.NumChannels * wave.BitsPerSample / 8);
    wave.BlockAlign    = bigEndian ? SWAP16(wave.NumChannels * wave.BitsPerSample / 8)
                            : (wave.NumChannels * wave.BitsPerSample / 8);

    fwrite(&wave, sizeof (WAV_HEADER), 1, fOut);
    fclose(fOut);

    pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);

    displayMsg("MOD RENDERED !");
    setMsgPointer();

    return (1);
}

int8_t renderToWav(char *filename, int8_t checkIfFileExist)
{
    FILE *fOut;

    struct stat statBuffer;

    if (checkIfFileExist)
    {
        if (stat(filename, &statBuffer) == 0)
        {
            editor.ui.askScreenShown = true;
            editor.ui.askScreenType  = ASK_MOD2WAV_OVERWRITE;

            pointerSetMode(POINTER_MODE_MSG1, NO_CARRY);
            setStatusMessage("OVERWRITE FILE?", NO_CARRY);

            return (false);
        }
    }

    if (editor.ui.askScreenShown)
    {
        editor.ui.answerNo       = false;
        editor.ui.answerYes      = false;
        editor.ui.askScreenShown = false;
    }

    fOut = fopen(filename, "wb");
    if (fOut == NULL)
    {
        displayErrorMsg("FILE I/O ERROR");
        return (false);
    }

    editor.mod2WavThread = SDL_CreateThread(mod2WavThreadFunc, fOut);

    return (true);
}

// for MOD2WAV
void calcMod2WavTotalRows(void)
{
    int8_t patternLoopCounter[PAULA_CHANNELS];
    int8_t patternLoopRow[PAULA_CHANNELS];
    uint8_t modRow;
    uint8_t PBreakFlag;
    uint8_t PosJumpAssert;
    uint8_t PBreakPosition;
    uint8_t calcingRows;
    uint8_t ch;
    uint8_t pos;

    int16_t modOrder;
    uint16_t modPattern;

    modnote_t *note;

    memset(patternLoopCounter, 0, sizeof (patternLoopCounter));
    memset(patternLoopRow,     0, sizeof (patternLoopRow));

    mod->rowsCounter       = 0;
    mod->rowsInTotal       = 0;
    modOrder               = 0;
    modPattern             = mod->head.order[0];
    modRow                 = 0;
    PBreakPosition         = 0;
    PosJumpAssert          = false;
    PBreakFlag             = false;
    calcingRows            = true;

    memset(editor.rowVisitTable, 0, MOD_ORDERS * MOD_ROWS);
    while (calcingRows)
    {
        editor.rowVisitTable[(modOrder * MOD_ROWS) + modRow] = true;

        for (ch = 0; ch < PAULA_CHANNELS; ++ch)
        {
            note = &mod->patterns[modPattern][(modRow * PAULA_CHANNELS) + ch];

            // Bxx - Position Jump
            if (note->command == 0x0B)
            {
                modOrder = note->param - 1;

                PBreakPosition = 0;
                PosJumpAssert  = true;
            }

            // Dxx - Pattern Break
            else if (note->command == 0x0D)
            {
                pos = (((note->param >> 4) * 10) + (note->param & 0x0F));

                if (pos > 63)
                    PBreakPosition = 0;
                else
                    PBreakPosition = pos;

                PosJumpAssert = true;
            }

            // E6x - Pattern Loop
            else if ((note->command == 0x0E) && ((note->param >> 4) == 0x06))
            {
                pos = note->param & 0x0F;
                if (pos == 0)
                {
                    patternLoopRow[ch] = modRow;
                }
                else
                {
                    // this is so ugly
                    if (patternLoopCounter[ch] == 0)
                    {
                        patternLoopCounter[ch] = pos;

                        PBreakPosition = patternLoopRow[ch];
                        PBreakFlag     = true;

                        for (pos = PBreakPosition; pos <= modRow; ++pos)
                            editor.rowVisitTable[(modOrder * MOD_ROWS) + pos] = false;
                    }
                    else
                    {
                        if (--patternLoopCounter[ch] != 0)
                        {
                            PBreakPosition = patternLoopRow[ch];
                            PBreakFlag     = true;

                            for (pos = PBreakPosition; pos <= modRow; ++pos)
                                editor.rowVisitTable[(modOrder * MOD_ROWS) + pos] = false;
                        }
                    }
                }
            }
        }

        modRow++;
        mod->rowsInTotal++;

        if (PBreakFlag)
        {
            PBreakFlag     = false;
            modRow         = PBreakPosition;
            PBreakPosition = 0;
        }

        if ((modRow >= MOD_ROWS) || PosJumpAssert)
        {
            modRow         = PBreakPosition;
            PBreakPosition = 0;
            PosJumpAssert  = false;

            if (++modOrder >= mod->head.orderCount)
            {
                calcingRows = false;
                break;
            }

            modPattern = mod->head.order[modOrder];
            if (modPattern > (MAX_PATTERNS - 1))
                modPattern =  MAX_PATTERNS - 1;
        }

        if (editor.rowVisitTable[(modOrder * MOD_ORDERS) + modRow])
        {
            // row has been visited before, we're now done!
            calcingRows = false;
            break;
        }
    }
}
