/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** The OpenGL parts of this file are NOT in the public domain,
** please read licenses.txt.
** (code taken from dosbox_svn->trunk->src->gui->sdlmain.cpp and modified)
**
** The rest is considered 'public domain', do whatever you want with it.
**
*/

#include <stdint.h>
#include <math.h>
#ifdef USE_OPENGL
#include <SDL/SDL_opengl.h>
#endif
#include "pt_header.h"
#include "pt_buttons.h"
#include "pt_mouse.h"
#include "pt_audio.h"
#include "pt_palette.h"
#include "pt_helpers.h"
#include "pt_textout.h"
#include "pt_tables.h"
#include "pt_modloader.h"
#include "pt_sampleloader.h"
#include "pt_patternviewer.h"
#include "pt_sampler.h"
#include "pt_diskop.h"
#include "pt_visuals.h"
#include "pt_helpers.h"

extern int8_t forceMixerOff;       // pt_audio.c
extern uint32_t *tempScreenBuffer; // pt_main.c
extern SDL_Surface *screen;        // pt_main.c
extern SDL_Surface *iconSurface;   // pt_main.c
extern SDL_VideoInfo tmpVidInfo;   // pt_main.c
extern MODULE *mod;                // pt_main.c

#ifdef USE_OPENGL
    const char *gl_ext;
    int32_t pixel_buffer_object;
    int32_t updating;
    GLuint buffer;
    GLuint displaylist;
    extern GLuint texture;                 // pt_main.c
#else
    extern uint32_t *frameBuffer;          // pt_main.c
    extern SDL_Surface *tempScreenSurface; // pt_main.c
#endif

#if USE_OPENGL
#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GL_ARB_pixel_buffer_object
#define GL_ARB_pixel_buffer_object 1
#define GL_PIXEL_PACK_BUFFER_ARB           0x88EB
#define GL_PIXEL_UNPACK_BUFFER_ARB         0x88EC
#define GL_PIXEL_PACK_BUFFER_BINDING_ARB   0x88ED
#define GL_PIXEL_UNPACK_BUFFER_BINDING_ARB 0x88EF
#endif

#ifndef GL_ARB_vertex_buffer_object
#define GL_ARB_vertex_buffer_object 1
typedef void (APIENTRYP PFNGLGENBUFFERSARBPROC)(GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBINDBUFFERARBPROC)(GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSARBPROC)(GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAARBPROC)(GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
typedef GLvoid *(APIENTRYP PFNGLMAPBUFFERARBPROC)(GLenum target, GLenum access);
typedef GLboolean (APIENTRYP PFNGLUNMAPBUFFERARBPROC)(GLenum target);
#endif

PFNGLGENBUFFERSARBPROC glGenBuffersARB       = NULL;
PFNGLBINDBUFFERARBPROC glBindBufferARB       = NULL;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB = NULL;
PFNGLBUFFERDATAARBPROC glBufferDataARB       = NULL;
PFNGLMAPBUFFERARBPROC glMapBufferARB         = NULL;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB     = NULL;
#endif

int8_t processTick(void);                     // pt_modplayer.c
void outputAudioToSample(int32_t numSamples); // pt_audio.c
extern int32_t samplesPerTick;                // pt_audio.c
void storeTempVariables(void);                // pt_modplayer.c
uint8_t getSongProgressInPercentage(void);    // pt_modplayer.c

void videoResize(uint32_t width, uint32_t height, int8_t fullscreen);

void blitFrame(void)
{
#ifndef USE_OPENGL
    // this routine was optimized by Raylight^PwL and capisce
    const uint32_t *src;
    uint32_t *dst;
    uint32_t *dstLine;
    uint32_t y;
    uint32_t x;

    dst = frameBuffer;
    src = tempScreenBuffer;

    if (editor.ui.scale3x)
    {
        for (y = 0; y < SCREEN_H; ++y)
        {
            dstLine = dst;

            for (x = 0; x < SCREEN_W; ++x)
            {
                dstLine[(x * 3) + 0] = *src;
                dstLine[(x * 3) + 1] = *src;
                dstLine[(x * 3) + 2] = *src++;
            }

            memcpy(dst + (SCREEN_W * 3) * 1, dst, (SCREEN_W * 3) * sizeof (int32_t));
            memcpy(dst + (SCREEN_W * 3) * 2, dst, (SCREEN_W * 3) * sizeof (int32_t));

            dst += ((SCREEN_W * 3) * 3);
        }
    }
    else
    {
        for (y = 0; y < SCREEN_H; ++y)
        {
            dstLine = dst;

            for (x = 0; x < SCREEN_W; ++x)
            {
                dstLine[(x * 2) + 0] = *src;
                dstLine[(x * 2) + 1] = *src++;
            }

            memcpy(dst + (SCREEN_W * 2), dst, (SCREEN_W * 2) * sizeof (int32_t));

            dst += ((SCREEN_W * 2) * 2);
        }
    }

    SDL_Flip(screen);
#else
    if (!updating)
        return;

    updating = false;

    if (pixel_buffer_object)
    {
        glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_W, SCREEN_H, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, 0);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    }
    else
    {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexEnvi(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_W, SCREEN_H, GL_BGRA_EXT, GL_UNSIGNED_INT_8_8_8_8_REV, tempScreenBuffer);
    }

    glCallList(displaylist);
    SDL_GL_SwapBuffers();
#endif
}

void updateSpectrumAnalyzer(int16_t period, int8_t volume)
{
    int8_t scaledVol;

    int16_t scaledNote;

    scaledVol  = (int8_t)((volume * 256) / 682);    //   682 = (64 * 256) / 24    (64 = max sample vol)
    scaledNote = 743 - (period - 113);              //   743 = 856 - 113          (856 = C-1 period, 113 = B-3 period)
    scaledNote = (scaledNote * scaledNote) / 25093; // 25093 = (743 * 743) / 22   (22 = num of spectrum bars-1)

    // scaledNote now ranges 0..22, no need to clamp

    // increment main spectrum bar
    editor.spectrumVolumes[scaledNote] += scaledVol;
    if (editor.spectrumVolumes[scaledNote] > SPECTRUM_BAR_HEIGHT)
        editor.spectrumVolumes[scaledNote] = SPECTRUM_BAR_HEIGHT;

    // increment left side of spectrum bar with half volume
    if (scaledNote > 0)
    {
        editor.spectrumVolumes[scaledNote - 1] += (scaledVol / 2);
        if (editor.spectrumVolumes[scaledNote - 1] > SPECTRUM_BAR_HEIGHT)
            editor.spectrumVolumes[scaledNote - 1] = SPECTRUM_BAR_HEIGHT;
    }

    // increment right side of spectrum bar with half volume
    if (scaledNote < (SPECTRUM_BAR_NUM - 1))
    {
        editor.spectrumVolumes[scaledNote + 1] += (scaledVol / 2);
        if (editor.spectrumVolumes[scaledNote + 1] > SPECTRUM_BAR_HEIGHT)
            editor.spectrumVolumes[scaledNote + 1] = SPECTRUM_BAR_HEIGHT;
    }
}

void updateQuadrascope(MODULE *modEntry)
{
    uint8_t i;

    MODULE_SAMPLE *s;

    mod_channel *ch;

    for (i = 0; i < PAULA_CHANNELS; i++)
    {
        ch = &modEntry->channels[i];
        if (ch->scopeTrigger)
        {
            ch->scopeTrigger = false;
            ch->scopeEnabled = false;

            s = &modEntry->samples[ch->sample - 1];
            if (s->length > 0)
            {
                ch->scopeEnabled   = true;
                ch->scopeLoopFlag  = (s->loopStart + s->loopLength) > 2;
                ch->scopeEnd       = s->offset + s->length;
                ch->scopeLoopBegin = s->offset + s->loopStart;
                ch->scopeLoopEnd   = ch->scopeLoopBegin + s->loopLength;

                // one-shot loop simulation (real PT didn't show this in the scopes...)
                ch->scopeLoopQuirk = false;
                if ((s->loopLength > 2) && (s->loopStart == 0))
                {
                    ch->scopeLoopQuirk = ch->scopeLoopEnd;
                    ch->scopeLoopEnd   = ch->scopeEnd;
                }

                if (ch->scopeChangePos)
                {
                    ch->scopeChangePos = false;

                    // ch->scopePos was externally modified

                    if (ch->scopeLoopFlag)
                    {
                        while (ch->scopePos >= ch->scopeLoopEnd)
                               ch->scopePos  = ch->scopeLoopBegin + (ch->scopePos - ch->scopeLoopEnd);
                    }
                    else
                    {
                        if (ch->scopePos >= ch->scopeEnd)
                            ch->scopeEnabled = false;
                    }
                }
                else
                {
                    ch->scopePos = s->offset;
                }
            }
        }
        else
        {
            if (ch->scopeKeepVolume) ch->scopeVolume = 0 - (ch->volume / 2);
            if (ch->scopeKeepPeriod) ch->scopePeriod = ch->period / 2;
            if (ch->scopePeriod > 0) ch->scopePos   += (35469 / ch->scopePeriod); // 35469 = round((AMIGA_PAULA_PAL_CLK / 50Hz) / 2)

            if (ch->scopeLoopFlag)
            {
                if (ch->scopePos >= ch->scopeLoopEnd)
                {
                    while (ch->scopePos >= ch->scopeLoopEnd)
                           ch->scopePos  = ch->scopeLoopBegin + (ch->scopePos - ch->scopeLoopEnd);

                    if (ch->scopeLoopQuirk)
                    {
                        ch->scopeLoopEnd   = ch->scopeLoopQuirk;
                        ch->scopeLoopQuirk = false;
                    }
                }
            }
            else
            {
                if (ch->scopePos >= ch->scopeEnd)
                    ch->scopeEnabled = false;
            }
        }
    }
}

uint32_t _50HzCallBack(uint32_t interval, void *param)
{
    uint8_t i;

    if (!forceMixerOff && !editor.isWAVRendering)
        updateQuadrascope(mod);

    // sink VU meters and spectrumanalyzer bars
    for (i = 0; i < SPECTRUM_BAR_NUM; ++i)
    {
        if (i < PAULA_CHANNELS) // these are the VU meters
        {
            if (editor.vuMeterVolumes[i] > 0)
                editor.vuMeterVolumes[i]--;
        }

        if (editor.spectrumVolumes[i] > 0)
            editor.spectrumVolumes[i]--;
    }

    if ((editor.playMode != PLAY_MODE_PATTERN) ||
        ((editor.currentMode == MODE_RECORD) && (editor.RecordMode != RECORD_PATT)))
    {
        if (++editor.ticks50Hz == PAL_AMIGA_VBL_RATE)
        {
            editor.ticks50Hz = 0;

            // exactly one second has passed, let's increment the play timer

            if (!editor.isWAVRendering && editor.songPlaying)
            {
                if (++editor.playTime > 5999) // 5999 = 99:59
                      editor.playTime = 5999;
            }
        }
    }

    (void)(param); // make compiler happy

    return (interval);
}

void toggleFullscreen(void)
{
#ifdef USE_OPENGL
    if (pixel_buffer_object && (screen != NULL))
    {
        editor.fullScreenFlag ^= 1; // toggle

        SDL_LockSurface(screen);

        if (editor.fullScreenFlag)
        {
            SDL_GL_Lock();
            videoResize(tmpVidInfo.current_w, tmpVidInfo.current_h, OPENGL_FULLSCREEN);
            SDL_GL_Unlock();

            SDL_WarpMouse((uint16_t)((float)(SCREEN_W) * input.mouse.scale) / 2, (uint16_t)((float)(SCREEN_H) * input.mouse.scale) / 2);

            input.mouse.x = SCREEN_W / 2;
            input.mouse.y = SCREEN_H / 2;
        }
        else
        {
            SDL_GL_Lock();
            videoResize(SCREEN_W * (editor.ui.scale3x ? 3 : 2), SCREEN_H * (editor.ui.scale3x ? 3 : 2), OPENGL_WINDOWED);
            SDL_GL_Unlock();

            SDL_WarpMouse(((SCREEN_W * (editor.ui.scale3x ? 3 : 2)) / 2), ((SCREEN_H * (editor.ui.scale3x ? 3 : 2)) / 2));

            input.mouse.x = SCREEN_W / 2;
            input.mouse.y = SCREEN_H / 2;
        }

        SDL_UnlockSurface(screen);
    }
#else
    displayErrorMsg("NO OPENGL!");
#endif
}

int8_t setupVideo(void)
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0)
    {
        fprintf(stderr, "ERROR: Couldn't initialize SDL: %s\n", SDL_GetError());
        return (false);
    }

    SDL_WM_SetCaption("ProTracker v2.3D", "ProTracker v2.3D");

#ifdef USE_OPENGL
    tmpVidInfo = *SDL_GetVideoInfo();

    screen = SDL_SetVideoMode(SCREEN_W * (editor.ui.scale3x ? 3 : 2), SCREEN_H * (editor.ui.scale3x ? 3 : 2), 0, SDL_OPENGL);

    tempScreenBuffer = NULL;
    displaylist      = 0;
    texture          = 0;
    updating         = false;

#ifdef _MSC_VER
#pragma warning(disable:4055)
#endif
    glGenBuffersARB    =    (PFNGLGENBUFFERSARBPROC)(SDL_GL_GetProcAddress("glGenBuffersARB"));
    glBindBufferARB    =    (PFNGLBINDBUFFERARBPROC)(SDL_GL_GetProcAddress("glBindBufferARB"));
    glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)(SDL_GL_GetProcAddress("glDeleteBuffersARB"));
    glBufferDataARB    =    (PFNGLBUFFERDATAARBPROC)(SDL_GL_GetProcAddress("glBufferDataARB"));
    glMapBufferARB     =     (PFNGLMAPBUFFERARBPROC)(SDL_GL_GetProcAddress("glMapBufferARB"));
    glUnmapBufferARB   =   (PFNGLUNMAPBUFFERARBPROC)(SDL_GL_GetProcAddress("glUnmapBufferARB"));
#ifdef _MSC_VER
#pragma warning(error:4055)
#endif

    gl_ext = (const char *)(glGetString(GL_EXTENSIONS));
    if (gl_ext && *gl_ext)
    {
        pixel_buffer_object = (strstr(gl_ext, "GL_ARB_pixel_buffer_object") != NULL) &&
            glGenBuffersARB && glBindBufferARB && glDeleteBuffersARB && glBufferDataARB &&
            glMapBufferARB && glUnmapBufferARB;
    }
    else
    {
        pixel_buffer_object = false;
    }

    videoResize(SCREEN_W * (editor.ui.scale3x ? 3 : 2), SCREEN_H * (editor.ui.scale3x ? 3 : 2), OPENGL_WINDOWED);
#else
    screen = SDL_SetVideoMode(SCREEN_W * (editor.ui.scale3x ? 3 : 2), SCREEN_H * (editor.ui.scale3x ? 3 : 2), 32, SDL_SWSURFACE);
    tempScreenSurface = SDL_CreateRGBSurface(screen->flags, SCREEN_W, SCREEN_H,
        32, screen->format->Rmask, screen->format->Gmask,
            screen->format->Bmask, screen->format->Amask);

    tempScreenBuffer = (uint32_t *)(tempScreenSurface->pixels);
    frameBuffer      = (uint32_t *)(screen->pixels);
#endif

    if (screen == NULL)
    {
        fprintf(stderr, "ERROR: Couldn't set video mode: %s\n", SDL_GetError());
        return (false);
    }

    editor.ui.pixelFormat = screen->format;

    SDL_ShowCursor(SDL_DISABLE);

    iconSurface = SDL_CreateRGBSurface(screen->flags, 32, 32, 32,
        0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
    SDL_SetColorKey(iconSurface, SDL_SRCCOLORKEY, palette[PAL_COLORKEY]);

    // copy main icon to a buffer
    memcpy(iconSurface->pixels, iconBitmap, 32 * 32 * sizeof (int32_t));
    SDL_WM_SetIcon(iconSurface, NULL);

    return (true);
}

void renderFrame(void)
{
    const uint8_t *ptr8_src;
    uint8_t barLength;
    uint8_t percent;

    int16_t scopeData;
    int16_t posEdPosition;
    uint16_t x;
    uint16_t y;

    int32_t newPos;
    const uint32_t *ptr32_src;
    uint32_t *ptr32_dst;
    uint32_t *ptr32_dst_2;
    uint32_t pix;
    uint32_t i;

    MODULE_SAMPLE *currentSample;

    mod_channel *ch;

    // fetch a pointer to the current sample's sample struct
    currentSample = &mod->samples[editor.currentSample];

    startGfxUpdate(); // for OpenGL

    // blit tracker frame to screen
    memcpy(tempScreenBuffer, trackerFrameBitmap, 320 * 255 * sizeof (int32_t));

    if (!editor.ui.samplerScreenShown)
    {
        drawPatternData(tempScreenBuffer, mod);

        // DRAW VUMETERS
        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            ptr32_src = vuMeterBitmap;
            ptr32_dst = tempScreenBuffer + ((187 * SCREEN_W) + (55 + (i * 72)));

            y = editor.vuMeterVolumes[i];
            while (y--)
            {
                *(ptr32_dst + 0) = *(ptr32_src + 0);
                *(ptr32_dst + 1) = *(ptr32_src + 1);
                *(ptr32_dst + 2) = *(ptr32_src + 2);
                *(ptr32_dst + 3) = *(ptr32_src + 3);
                *(ptr32_dst + 4) = *(ptr32_src + 4);
                *(ptr32_dst + 5) = *(ptr32_src + 5);
                *(ptr32_dst + 6) = *(ptr32_src + 6);
                *(ptr32_dst + 7) = *(ptr32_src + 7);
                *(ptr32_dst + 8) = *(ptr32_src + 8);
                *(ptr32_dst + 9) = *(ptr32_src + 9);

                ptr32_src += 10;
                ptr32_dst -= SCREEN_W;
            }
        }

        // DRAW PATTERN CURSOR
        ptr32_src = patternCursorBitmap;
        ptr32_dst = tempScreenBuffer + ((188 * SCREEN_W) + CursorPosTable[editor.cursor.pos]);
        pix = palette[PAL_COLORKEY];

        y = 14;
        while (y--)
        {
            if (*(ptr32_src +  0) != pix) *(ptr32_dst +  0) = *(ptr32_src +  0);
            if (*(ptr32_src +  1) != pix) *(ptr32_dst +  1) = *(ptr32_src +  1);
            if (*(ptr32_src +  2) != pix) *(ptr32_dst +  2) = *(ptr32_src +  2);
            if (*(ptr32_src +  3) != pix) *(ptr32_dst +  3) = *(ptr32_src +  3);
            if (*(ptr32_src +  4) != pix) *(ptr32_dst +  4) = *(ptr32_src +  4);
            if (*(ptr32_src +  5) != pix) *(ptr32_dst +  5) = *(ptr32_src +  5);
            if (*(ptr32_src +  6) != pix) *(ptr32_dst +  6) = *(ptr32_src +  6);
            if (*(ptr32_src +  7) != pix) *(ptr32_dst +  7) = *(ptr32_src +  7);
            if (*(ptr32_src +  8) != pix) *(ptr32_dst +  8) = *(ptr32_src +  8);
            if (*(ptr32_src +  9) != pix) *(ptr32_dst +  9) = *(ptr32_src +  9);
            if (*(ptr32_src + 10) != pix) *(ptr32_dst + 10) = *(ptr32_src + 10);

            ptr32_src += 11;
            ptr32_dst += SCREEN_W;
        }
    }

    if (!editor.ui.diskOpScreenShown)
    {
        if (!editor.ui.editOpScreenShown && !editor.ui.posEdScreenShown)
        {
            // DRAW QUADRASCOPES OR SPECTRUM ANALYZER
            switch (editor.ui.visualizerMode)
            {
                case VISUAL_QUADRASCOPE:
                {
                    ptr32_dst = tempScreenBuffer + ((71 * SCREEN_W) + 128);

                    y = PAULA_CHANNELS;
                    while (y--)
                    {
                        ch = &mod->channels[(PAULA_CHANNELS - 1) - y];

                        x = SCOPE_WIDTH;
                        while (x--)
                        {
                            if (!ch->scopeEnabled || !ch->period)
                            {
                                ptr32_dst[x] = palette[PAL_QRDSCP];
                            }
                            else
                            {
                                newPos = ch->scopePos + x;
                                if (ch->scopeLoopFlag)
                                {
                                    while (newPos >= ch->scopeLoopEnd)
                                           newPos  = ch->scopeLoopBegin + (newPos - ch->scopeLoopEnd);
                                }
                                else
                                {
                                    if (newPos >= ch->scopeEnd)
                                    {
                                        ptr32_dst[x] = palette[PAL_QRDSCP];
                                        continue;
                                    }
                                }

                                if (newPos >= (0x0001FFFE * 31))
                                {
                                    // *never* read beyond sample data allocation
                                    scopeData = 0;
                                }
                                else
                                {
                                    scopeData = mod->sampleData[newPos] * ch->scopeVolume;

                                    // 68k simulation on signed number: ASR.W #8,D0
                                    if (scopeData < 0)
                                        scopeData = 0xFF00 | ((uint16_t)(scopeData) >> 8);
                                    else
                                        scopeData /= 256;
                                }

                                ptr32_dst[(scopeData * SCREEN_W) + x] = palette[PAL_QRDSCP];
                            }
                        }

                        ptr32_dst += (SCOPE_WIDTH + 8);
                    }
                }
                break;

                case VISUAL_SPECTRUM:
                {
                    // draw empty spectrum analyzer dialog
                    ptr32_src = spectrumVisualsBitmap;
                    ptr32_dst = tempScreenBuffer + (44 * SCREEN_W) + 120;

                    y = 55;
                    while (y--)
                    {
                        memcpy(ptr32_dst, ptr32_src, 200 * sizeof (int32_t));

                        ptr32_src += 200;
                        ptr32_dst += SCREEN_W;
                    }

                    // draw spectrum analyzer bars
                    for (i = 0; i < SPECTRUM_BAR_NUM; ++i)
                    {
                        ptr32_src = spectrumAnaBitmap;
                        ptr32_dst = tempScreenBuffer + ((94 * SCREEN_W) + (129 + (i * (SPECTRUM_BAR_WIDTH + 2))));

                        y = editor.spectrumVolumes[i];
                        while (y--)
                        {
                            *(ptr32_dst + 0) = *ptr32_src;
                            *(ptr32_dst + 1) = *ptr32_src;
                            *(ptr32_dst + 2) = *ptr32_src;
                            *(ptr32_dst + 3) = *ptr32_src;
                            *(ptr32_dst + 4) = *ptr32_src;
                            *(ptr32_dst + 5) = *ptr32_src++;

                            ptr32_dst -= SCREEN_W;
                        }
                    }
                }
                break;

                default: break;
            }

            if (editor.ui.aboutScreenShown)
            {
                ptr32_src = aboutScreenBitmap;
                ptr32_dst = tempScreenBuffer + (44 * SCREEN_W) + 120;

                y = 55;
                while (y--)
                {
                    memcpy(ptr32_dst, ptr32_src, 200 * sizeof (int32_t));

                    ptr32_src += 200;
                    ptr32_dst += SCREEN_W;
                }
            }
        }

        if (editor.ui.introScreenShown)
        {
            ptr32_src = introScreenBitmap;
            ptr32_dst = tempScreenBuffer + (44 * SCREEN_W) + 120;

            y = 55;
            while (y--)
            {
                memcpy(ptr32_dst, ptr32_src, 200 * sizeof (int32_t));

                ptr32_src += 200;
                ptr32_dst += SCREEN_W;
            }
        }

        // DRAW SOME TEXTS

        // song stuff
        printThreeDecimals(tempScreenBuffer, 72, 25, *editor.currentLengthDisp,  palette[PAL_GENTXT]);
        printThreeDecimals(tempScreenBuffer, 72,  3, *editor.currentPosDisp,     palette[PAL_GENTXT]);
        printTwoDecimals(tempScreenBuffer,   80, 14, *editor.currentPatternDisp, palette[PAL_GENTXT]);

        // sample stuff
        if (currentSample->fineTune >= 8)
        {
            charOut(tempScreenBuffer, 80, 36, '-',                                             palette[PAL_GENTXT]);
            charOut(tempScreenBuffer, 88, 36, '0' + (0x10 - (currentSample->fineTune & 0x0F)), palette[PAL_GENTXT]);
        }
        else if (currentSample->fineTune > 0)
        {
            charOut(tempScreenBuffer, 80, 36, '+',                                    palette[PAL_GENTXT]);
            charOut(tempScreenBuffer, 88, 36, '0' + (currentSample->fineTune & 0x0F), palette[PAL_GENTXT]);
        }
        else
        {
            charOut(tempScreenBuffer, 88, 36, '0', palette[PAL_GENTXT]);
        }

        printTwoHex(tempScreenBuffer, 80, 47, editor.SampleZero ? 0 : ((*editor.currentSampleDisp) + 1), palette[PAL_GENTXT]);

        printTwoHex(tempScreenBuffer,  80, 58, *currentSample->volumeDisp,     palette[PAL_GENTXT]);
        printFiveHex(tempScreenBuffer, 56, 69, *currentSample->lengthDisp,     palette[PAL_GENTXT]);
        printFiveHex(tempScreenBuffer, 56, 80, *currentSample->loopStartDisp,  palette[PAL_GENTXT]);
        printFiveHex(tempScreenBuffer, 56, 91, *currentSample->loopLengthDisp, palette[PAL_GENTXT]);

        // zeroes (can't integrate zeroes in the graphics, the palette entry is above the 2-bit range)
        charOut(tempScreenBuffer, 64,  3, '0',  palette[PAL_GENTXT]);
        charOut(tempScreenBuffer, 64, 25, '0',  palette[PAL_GENTXT]);
        textOut(tempScreenBuffer, 64, 14, "00", palette[PAL_GENTXT]);
        textOut(tempScreenBuffer, 64, 47, "00", palette[PAL_GENTXT]);
        textOut(tempScreenBuffer, 64, 58, "00", palette[PAL_GENTXT]);
    }

    // DRAW SONG NAME
    for (x = 0; x < 20; ++x)
    {
        charOut(tempScreenBuffer, 104 + (x * FONT_CHAR_W), 102,
                (mod->head.moduleTitle[x] == '\0') ? '_' : mod->head.moduleTitle[x], palette[PAL_GENTXT]);
    }

    // DRAW AUTO INSERT FLAG
    if (editor.AutoInsFlag)
    {
        charOut(tempScreenBuffer, 0, 113, 'I', palette[PAL_GENTXT]);
        charOut(tempScreenBuffer, 8, 113, '1', palette[PAL_GENTXT]);
    }

    // DRAW METRONOME FLAG
    if (editor.MetroFlag)
        charOut(tempScreenBuffer, 0, 102, 'M', palette[PAL_GENTXT]);

    // DRAW MULTIKEY FLAG
    if (editor.MultiFlag)
        charOut(tempScreenBuffer, 16, 102, 'M', palette[PAL_GENTXT]);

    // DRAW ROW STEP VALUE
    charOut(tempScreenBuffer, 24, 102, '0' + editor.EditMoveAdd, palette[PAL_GENTXT]);

    // DRAW SONG PLAYING TIME (clamped to 00:00 .. 99:59 in _50HzCallBack())
    printTwoDecimals(tempScreenBuffer, 272, 102, editor.playTime / 60, palette[PAL_GENTXT]);
    printTwoDecimals(tempScreenBuffer, 296, 102, editor.playTime % 60, palette[PAL_GENTXT]);

    // DRAW TEMPO/BPM AND CURRENT PATTERN NUMBER
    if (!editor.ui.samplerScreenShown)
    {
        printThreeDecimals(tempScreenBuffer, 32, 123, mod->currBPM,                   palette[PAL_GENTXT]);
        printTwoDecimals(tempScreenBuffer,    8, 127, *editor.currentEditPatternDisp, palette[PAL_GENTXT]);
    }

    // DRAW CURRENT SAMPLE'S TEXT
    for (x = 0; x < 22; ++x)
    {
        charOut(tempScreenBuffer, 104 + (x * FONT_CHAR_W), 113,
                (currentSample->text[x] == '\0') ? '_' : currentSample->text[x], palette[PAL_GENTXT]);
    }

    // DRAW MUTE BUTTONS (IF NEEDED)
    if (!editor.ui.diskOpScreenShown)
    {
        for (i = 0; i < PAULA_CHANNELS; ++i)
        {
            if (editor.muted[i])
            {
                ptr32_src = muteButtonsBitmap + (i * (6 * 7));
                ptr32_dst = tempScreenBuffer + ((3 + (i * 11)) * SCREEN_W) + 310;

                y = 6;
                while (y--)
                {
                    *(ptr32_dst + 0) = *(ptr32_src + 0);
                    *(ptr32_dst + 1) = *(ptr32_src + 1);
                    *(ptr32_dst + 2) = *(ptr32_src + 2);
                    *(ptr32_dst + 3) = *(ptr32_src + 3);
                    *(ptr32_dst + 4) = *(ptr32_src + 4);
                    *(ptr32_dst + 5) = *(ptr32_src + 5);
                    *(ptr32_dst + 6) = *(ptr32_src + 6);

                    ptr32_src += 7;
                    ptr32_dst += SCREEN_W;
                }
            }
        }
    }

    // DRAW EDIT OP
    if (editor.ui.editOpScreenShown)
    {
        switch (editor.ui.editOpScreen)
        {
            default:
            case 0: ptr32_src = editOpScreen1Bitmap; break;
            case 1: ptr32_src = editOpScreen2Bitmap; break;
            case 2: ptr32_src = editOpScreen3Bitmap; break;
            case 3: ptr32_src = editOpScreen4Bitmap; break;
        }

        ptr32_dst = tempScreenBuffer + (44 * SCREEN_W) + 120;

        y = 55;
        while (y--)
        {
            memcpy(ptr32_dst, ptr32_src, 200 * sizeof (int32_t));

            ptr32_src += 200;
            ptr32_dst += SCREEN_W;
        }

        switch (editor.ui.editOpScreen)
        {
            default:
            case 0:
            {
                textOut(tempScreenBuffer, 144, 47, "TRACK      PATTERN", palette[PAL_GENTXT]);
                ptr32_src = &editOpModeCharsBitmap[editor.SampleAllFlag ? EDOP_MODE_BMP_A_OFS : EDOP_MODE_BMP_S_OFS];
            }
            break;

            case 1:
            {
                textOut(tempScreenBuffer, 144, 47, "RECORD     SAMPLES", palette[PAL_GENTXT]);

                     if (editor.TrackPattFlag == 0) ptr32_src = &editOpModeCharsBitmap[EDOP_MODE_BMP_T_OFS];
                else if (editor.TrackPattFlag == 1) ptr32_src = &editOpModeCharsBitmap[EDOP_MODE_BMP_P_OFS];
                else                                ptr32_src = &editOpModeCharsBitmap[EDOP_MODE_BMP_S_OFS];

                // RECORD
                textOut(tempScreenBuffer, 176, 58, (editor.RecordMode == RECORD_PATT) ? "PATT" : "SONG", palette[PAL_GENTXT]);

                // QUANTIZE
                printTwoDecimals(tempScreenBuffer, 192, 69, *editor.QuantizeValueDisp, palette[PAL_GENTXT]);

                // METRO
                printTwoDecimals(tempScreenBuffer, 168, 80, *editor.MetroSpeedDisp,   palette[PAL_GENTXT]);
                printTwoDecimals(tempScreenBuffer, 192, 80, *editor.MetroChannelDisp, palette[PAL_GENTXT]);

                // SAMPLE FROM
                printTwoHex(tempScreenBuffer, 264, 80, *editor.SampleFromDisp, palette[PAL_GENTXT]);

                // KEYS
                textOut(tempScreenBuffer, 160, 91, editor.MultiFlag ? "MULTI" : "SINGLE", palette[PAL_GENTXT]);

                // SAMPLE TO
                printTwoHex(tempScreenBuffer, 264, 91, *editor.SampleToDisp, palette[PAL_GENTXT]);
            }
            break;

            case 2:
            {
                if (editor.MixFlag)
                    textOut(tempScreenBuffer, 128, 47, editor.MixText, palette[PAL_GENTXT]);
                else
                    textOut(tempScreenBuffer, 160, 47, "SAMPLE EDITOR", palette[PAL_GENTXT]);

                ptr32_src = &editOpModeCharsBitmap[editor.HalfClipFlag ? EDOP_MODE_BMP_C_OFS : EDOP_MODE_BMP_H_OFS];

                // SAMPLE
                printFiveHex(tempScreenBuffer, 240, 58, *editor.SamplePosDisp, palette[PAL_GENTXT]);

                // MOD
                printThreeDecimals(tempScreenBuffer, 256, 69,
                    (editor.ModulateSpeed < 0) ? (0 - editor.ModulateSpeed) : editor.ModulateSpeed,
                    palette[PAL_GENTXT]);

                if (editor.ModulateSpeed < 0)
                    charOut(tempScreenBuffer, 248, 69, '-', palette[PAL_GENTXT]);

                // VOL
                printThreeDecimals(tempScreenBuffer, 248, 91, *editor.SampleVolDisp, palette[PAL_GENTXT]);
                charOut(tempScreenBuffer,            272, 91, '%',                   palette[PAL_GENTXT]);
            }
            break;

            case 3:
            {
                textOut(tempScreenBuffer, 136, 47, "SAMPLE CHORD EDITOR", palette[PAL_GENTXT]);

                ptr32_src = (editor.NewOldFlag == 0) ? &editOpModeCharsBitmap[EDOP_MODE_BMP_N_OFS] : &editOpModeCharsBitmap[EDOP_MODE_BMP_O_OFS];

                // CHORD NOTE #1
                if (editor.Note1 > 35)
                    textOut(tempScreenBuffer, 256, 58, "---", palette[PAL_GENTXT]);
                else
                    textOut(tempScreenBuffer, 256, 58, editor.Accidental ? NoteNames2[editor.Note1] : NoteNames1[editor.Note1], palette[PAL_GENTXT]);

                // CHORD NOTE #2
                if (editor.Note2 > 35)
                    textOut(tempScreenBuffer, 256, 69, "---", palette[PAL_GENTXT]);
                else
                    textOut(tempScreenBuffer, 256, 69, editor.Accidental ? NoteNames2[editor.Note2] : NoteNames1[editor.Note2], palette[PAL_GENTXT]);

                // CHORD NOTE #3
                if (editor.Note3 > 35)
                    textOut(tempScreenBuffer, 256, 80, "---", palette[PAL_GENTXT]);
                else
                    textOut(tempScreenBuffer, 256, 80, editor.Accidental ? NoteNames2[editor.Note3] : NoteNames1[editor.Note3], palette[PAL_GENTXT]);

                // CHORD NOTE #4
                if (editor.Note4 > 35)
                    textOut(tempScreenBuffer, 256, 91, "---", palette[PAL_GENTXT]);
                else
                    textOut(tempScreenBuffer, 256, 91, editor.Accidental ? NoteNames2[editor.Note4] : NoteNames1[editor.Note4], palette[PAL_GENTXT]);

                // CHORD MAX LENGTH
                printFiveHex(tempScreenBuffer, 160, 91, *editor.ChordLengthDisp, palette[PAL_GENTXT]);

                // MIN/MAX FLAG
                charOut(tempScreenBuffer, 198, 91, (editor.ChordLengthMin) ? '.' : ':', palette[PAL_GENTXT]);
            }
            break;
        }

        // draw edit op mode character
        ptr32_dst = tempScreenBuffer + (47 * SCREEN_W) + 310;

        y = 6;
        while (y--)
        {
            *(ptr32_dst + 0) = *(ptr32_src + 0);
            *(ptr32_dst + 1) = *(ptr32_src + 1);
            *(ptr32_dst + 2) = *(ptr32_src + 2);
            *(ptr32_dst + 3) = *(ptr32_src + 3);
            *(ptr32_dst + 4) = *(ptr32_src + 4);
            *(ptr32_dst + 5) = *(ptr32_src + 5);
            *(ptr32_dst + 6) = *(ptr32_src + 6);

            ptr32_src += 7;
            ptr32_dst += SCREEN_W;
        }
    }

    // DRAW POSITION EDITOR SCREEN
    if (editor.ui.posEdScreenShown) // never enabled if disk op. is shown
    {
        ptr32_src = posEdBitmap;
        ptr32_dst = tempScreenBuffer + 120;

        y = 99;
        while (y--)
        {
            memcpy(ptr32_dst, ptr32_src, 200 * sizeof (int32_t));

            ptr32_src += 200;
            ptr32_dst += SCREEN_W;
        }

        posEdPosition = mod->currOrder;
        if (posEdPosition > (mod->head.orderCount - 1))
            posEdPosition =  mod->head.orderCount - 1;

        // top five
        for (y = 0; y < 5; ++y)
        {
            if ((posEdPosition - (5 - y)) >= 0)
            {
                printThreeDecimals(tempScreenBuffer, 128, 23 + (y * 6), posEdPosition - (5 - y),                  palette[PAL_QRDSCP]);
                printTwoDecimals(tempScreenBuffer,   160, 23 + (y * 6), mod->head.order[posEdPosition - (5 - y)], palette[PAL_QRDSCP]);

                textOut(tempScreenBuffer, 184, 23 + (y * 6),
                    (const char *)(&editor.ui.pattNames[(mod->head.order[posEdPosition - (5 - y)] * 16) + editor.textofs.posEdPattName]),
                    palette[PAL_QRDSCP]);
            }
        }

        // middle
        printThreeDecimals(tempScreenBuffer, 128, 53, posEdPosition, palette[PAL_GENTXT]);
        printTwoDecimals(tempScreenBuffer,   160, 53, *editor.currentPosEdPattDisp, palette[PAL_GENTXT]);

        textOut(tempScreenBuffer, 184, 53,
            (const char *)(&editor.ui.pattNames[(mod->head.order[posEdPosition] * 16) + editor.textofs.posEdPattName]),
            palette[PAL_GENTXT]);

        // bottom six
        for (y = 0; y < 6; ++y)
        {
            if ((posEdPosition + y) < (mod->head.orderCount - 1))
            {
                printThreeDecimals(tempScreenBuffer, 128, 59 + (y * 6), posEdPosition + (y + 1),                  palette[PAL_QRDSCP]);
                printTwoDecimals(tempScreenBuffer  , 160, 59 + (y * 6), mod->head.order[posEdPosition + (y + 1)], palette[PAL_QRDSCP]);

                textOut(tempScreenBuffer, 184, 59 + (y * 6),
                    (const char *)(&editor.ui.pattNames[(mod->head.order[posEdPosition + (y + 1)] * 16) + editor.textofs.posEdPattName]),
                    palette[PAL_QRDSCP]);
            }
        }
    }

    // DRAW DISK OP
    if (editor.ui.diskOpScreenShown)
    {
        memcpy(tempScreenBuffer, diskOpScreenBitmap, 99 * 320 * sizeof (int32_t));
        diskOpDraw(tempScreenBuffer);
    }

    // DRAW SAMPLER SCREEN
    if (editor.ui.samplerScreenShown)
    {
        memcpy(tempScreenBuffer + (121 * SCREEN_W), samplerScreenBitmap, 134 * 320 * sizeof (int32_t));

        drawSampler(tempScreenBuffer, mod);

        if (editor.ui.samplerVolBoxShown)
        {
            ptr32_src = samplerVolumeBitmap;
            ptr32_dst = tempScreenBuffer + (154 * SCREEN_W) + 72;

            y = 33;
            while (y--)
            {
                memcpy(ptr32_dst, ptr32_src, 136 * sizeof (int32_t));

                ptr32_src += 136;
                ptr32_dst += SCREEN_W;
            }

            // DISPLAY VOL1 DRAG BAR
            ptr32_dst = &tempScreenBuffer[(158 * SCREEN_W) + (105 + ((editor.Vol1 * 3) / 10))];
            pix       = palette[PAL_QRDSCP];

            x = 5;
            while (x--)
            {
                *(ptr32_dst + (SCREEN_W * 0)) = pix;
                *(ptr32_dst + (SCREEN_W * 1)) = pix;
                *(ptr32_dst + (SCREEN_W * 2)) = pix;

                ptr32_dst++;
            }

            // DISPLAY VOL2 DRAG BAR
            ptr32_dst = &tempScreenBuffer[(169 * SCREEN_W) + (105 + ((editor.Vol2 * 3) / 10))];
            pix       = palette[PAL_QRDSCP];

            x = 5;
            while (x--)
            {
                *(ptr32_dst + (SCREEN_W * 0)) = pix;
                *(ptr32_dst + (SCREEN_W * 1)) = pix;
                *(ptr32_dst + (SCREEN_W * 2)) = pix;

                ptr32_dst++;
            }

            // DISPLAY VOLUMES
            printThreeDecimals(tempScreenBuffer, 176, 157, *editor.Vol1Disp, palette[PAL_GENTXT]);
            printThreeDecimals(tempScreenBuffer, 176, 168, *editor.Vol2Disp, palette[PAL_GENTXT]);
        }
    }

    // DRAW CLEAR DIALOG IF NEEDED
    if (editor.ui.clearScreenShown)
    {
        ptr32_src = clearDialogBitmap;
        ptr32_dst = tempScreenBuffer + ((51 * SCREEN_W) + 160);

        y = 39;
        while (y--)
        {
            memcpy(ptr32_dst, ptr32_src, 104 * sizeof (int32_t));

            ptr32_src += 104;
            ptr32_dst += SCREEN_W;
        }
    }

    // DRAW MOD2WAV PROGRESS DIALOG
    if (editor.isWAVRendering)
    {
        ptr32_src = mod2wavBitmap;
        ptr32_dst = tempScreenBuffer + ((27 * SCREEN_W) + 64);

        y = 48;
        while (y--)
        {
            memcpy(ptr32_dst, ptr32_src, 192 * sizeof (int32_t));

            ptr32_src += 192;
            ptr32_dst += SCREEN_W;
        }

        // DRAW PROGRESS BAR
        percent = getSongProgressInPercentage();
        if (percent > 100)
            percent = 100;

        barLength = (uint8_t)(floorf(((float)(percent) * (180.0f / 100.0f)) + 0.5f));
        ptr32_dst = tempScreenBuffer + ((42 * SCREEN_W) + 70);

        pix = palette[PAL_GENBKG2];

        y = 11;
        while (y--)
        {
            x = barLength;
            while (x--)
                ptr32_dst[x] = pix;

            ptr32_dst += SCREEN_W;
        }

        if (percent > 99)
            printThreeDecimals(tempScreenBuffer, 144, 45, percent, palette[PAL_GENTXT]);
        else
            printTwoDecimals(tempScreenBuffer,   152, 45, percent, palette[PAL_GENTXT]);

        charOut(tempScreenBuffer, 168, 45, '%', palette[PAL_GENTXT]);
    }

    // DRAW ASK DIALOG IF NEEDED
    if (editor.ui.askScreenShown)
    {
        ptr32_src = yesNoDialogBitmap;
        ptr32_dst = tempScreenBuffer + ((51 * SCREEN_W) + 160);

        y = 39;
        while (y--)
        {
            memcpy(ptr32_dst, ptr32_src, 104 * sizeof (int32_t));

            ptr32_src += 104;
            ptr32_dst += SCREEN_W;
        }
    }

    // DRAW MODULE SIZE
    if (mod->head.moduleSize > 999999)
    {
        charOut(tempScreenBuffer, 304, 123, 'K', palette[PAL_GENTXT]);
        printFourDecimals(tempScreenBuffer, 272, 123, mod->head.moduleSize / 1000, palette[PAL_GENTXT]);
    }
    else
    {
        printSixDecimals(tempScreenBuffer, 264, 123, mod->head.moduleSize, palette[PAL_GENTXT]);
    }

    // DRAW BLOCK MARK TEXT OR STATUS MESSAGE TEXT
    if (!input.mouse.buttonErrorMsgActive && editor.BlockMarkFlag && !editor.ui.askScreenShown
        && !editor.ui.clearScreenShown && !editor.swapChannelFlag)
    {
        textOut(tempScreenBuffer,  88, 127, "MARK BLOCK", palette[PAL_GENTXT]);
        charOut(tempScreenBuffer, 192, 127, '-',          palette[PAL_GENTXT]);

        editor.BlockToPos = mod->currRow;
        if (editor.BlockFromPos >= editor.BlockToPos)
        {
            printTwoDecimals(tempScreenBuffer, 176, 127, editor.BlockToPos,   palette[PAL_GENTXT]);
            printTwoDecimals(tempScreenBuffer, 200, 127, editor.BlockFromPos, palette[PAL_GENTXT]);
        }
        else
        {
            printTwoDecimals(tempScreenBuffer, 176, 127, editor.BlockFromPos, palette[PAL_GENTXT]);
            printTwoDecimals(tempScreenBuffer, 200, 127, editor.BlockToPos,   palette[PAL_GENTXT]);
        }
    }
    else
    {
        textOut(tempScreenBuffer, 88, 127, editor.ui.statusMessage, palette[PAL_GENTXT]);
    }

    // DRAW TIMING MODE TEXT
    textOut(tempScreenBuffer, 288, 130, (editor.timingMode == TEMPO_MODE_CIA) ? "CIA" : "VBL", palette[PAL_GENTXT]);

    // DRAW EDIT TEXT MARKER IF NEEDED
    if (editor.ui.getLineFlag)
    {
        ptr32_dst   = tempScreenBuffer + ((editor.ui.lineCurY * SCREEN_W) + (editor.ui.lineCurX - 4));
        ptr32_dst_2 = ptr32_dst - SCREEN_W;

        *(ptr32_dst   + 0) = 0xFF770077;
        *(ptr32_dst   + 1) = 0xFF770077;
        *(ptr32_dst   + 2) = 0xFF770077;
        *(ptr32_dst   + 3) = 0xFF770077;
        *(ptr32_dst   + 4) = 0xFF770077;
        *(ptr32_dst   + 5) = 0xFF770077;
        *(ptr32_dst   + 6) = 0xFF770077;
        *(ptr32_dst_2 + 0) = 0xFF770077;
        *(ptr32_dst_2 + 1) = 0xFF770077;
        *(ptr32_dst_2 + 2) = 0xFF770077;
        *(ptr32_dst_2 + 3) = 0xFF770077;
        *(ptr32_dst_2 + 4) = 0xFF770077;
        *(ptr32_dst_2 + 5) = 0xFF770077;
        *(ptr32_dst_2 + 6) = 0xFF770077;
    }

    // DRAW DRUMPAD DOTS
    if (editor.pnoteflag == 1)
    {
        tempScreenBuffer[(129 * SCREEN_W) + 314] = palette[PAL_GENTXT];
        tempScreenBuffer[(129 * SCREEN_W) + 315] = palette[PAL_GENTXT];
    }
    else if (editor.pnoteflag == 2)
    {
        tempScreenBuffer[(128 * SCREEN_W) + 314] = palette[PAL_GENTXT];
        tempScreenBuffer[(128 * SCREEN_W) + 315] = palette[PAL_GENTXT];
        tempScreenBuffer[(130 * SCREEN_W) + 314] = palette[PAL_GENTXT];
        tempScreenBuffer[(130 * SCREEN_W) + 315] = palette[PAL_GENTXT];
    }

    if (editor.ui.doEasterEgg)
        drawEasterEgg();

    // DRAW MOUSE CURSOR
    ptr32_dst = tempScreenBuffer + ((input.mouse.y * SCREEN_W) + input.mouse.x);
    ptr8_src  = (uint8_t *)(mousePointerBitmap);

    for (y = 0; y < 16; ++y)
    {
        if ((input.mouse.y + y) >= SCREEN_H)
            break;

        for (x = 0; x < 16; ++x)
        {
            if ((ptr8_src[x] != PAL_COLORKEY) && ((input.mouse.x + x) < SCREEN_W))
                ptr32_dst[x] = palette[ptr8_src[x]];
        }

        ptr8_src  += 16;
        ptr32_dst += SCREEN_W;
    }
}

// easter egg stuff :)
static void setLemmingMode(uint8_t i, uint8_t mode)
{
    if (editor.ui.lem[i].spawned)
    {
        editor.ui.lemCounter = 0;

        switch (mode)
        {
            case MODE_FALL:
            {
                editor.ui.lem[i].mode           = MODE_FALL;
                editor.ui.lem[i].regularFalling = true;
                editor.ui.lem[i].umbrellaOpened = false;
                editor.ui.lem[i].counter        = 0;
                editor.ui.lem[i].currSprite     = 0;
            }
            break;

            case MODE_WALK:
            {
                editor.ui.lem[i].mode       = MODE_WALK;
                editor.ui.lem[i].currSprite = 0;
            }
            break;

            case MODE_CLIMB:
            {
                editor.ui.lem[i].mode       = MODE_CLIMB;
                editor.ui.lem[i].counter    = 0;
                editor.ui.lem[i].currSprite = 0;
            }
            break;

            default: break;
        }
    }
}

void spawnLemming(void)
{
    if (editor.ui.numLemmings < MAX_LEMMINGS)
    {
        editor.ui.lem[editor.ui.numLemmings].spawned   = true;
        editor.ui.lem[editor.ui.numLemmings].direction = DIRECTION_RIGHT;
        editor.ui.lem[editor.ui.numLemmings].x         = rand() % (SCREEN_W - 6);
        editor.ui.lem[editor.ui.numLemmings].y         = -10;

        setLemmingMode(editor.ui.numLemmings, MODE_FALL);

        editor.ui.numLemmings++;
    }

    editor.ui.doEasterEgg = true;
}

void initEasterEgg(void)
{
    memset(editor.ui.lem, 0, sizeof (editor.ui.lem));

    editor.ui.numLemmings = 0;
    editor.ui.lemCounter  = 0;
}

void drawEasterEgg(void)
{
    const uint8_t umbrellaOpenTable[12] =  { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3 };
    const uint8_t umbrellaTable[24]     =  { 4, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5 };

    uint8_t i;
    uint8_t x;
    uint8_t y;

    uint32_t pixel;

    // the lemmings engine! (lite edition)
    for (i = 0; i < MAX_LEMMINGS; ++i)
    {
        if (editor.ui.lem[i].spawned)
        {
            // remove lemming if you clicked on her/him
            if (input.mouse.leftButtonPressed)
            {
                if (
                    (input.mouse.x >= editor.ui.lem[i].x) && (input.mouse.x <= (editor.ui.lem[i].x +  8)) &&
                    (input.mouse.y >= editor.ui.lem[i].y) && (input.mouse.y <= (editor.ui.lem[i].y + 16))
                   )
                {
                    editor.ui.lem[i].spawned = false;

                    if (--editor.ui.numLemmings <= 0)
                    {
                        editor.ui.doEasterEgg = false;
                        memset(editor.ui.lem, 0, sizeof (editor.ui.lem));

                        return;
                    }
                }
            }

            switch (editor.ui.lem[i].mode)
            {
                case MODE_FALL:
                {
                    // render lemming
                    if (editor.ui.lem[i].regularFalling)
                    {
                        for (y = 0; y < 10; ++y)
                        {
                            if (((editor.ui.lem[i].y + y) < 0) || ((editor.ui.lem[i].y + y) >= SCREEN_H))
                                continue;

                            for (x = 0; x < 6; ++x)
                            {
                                if (((editor.ui.lem[i].x + x) < 0) || ((editor.ui.lem[i].x + x) >= SCREEN_W))
                                    continue;

                                pixel = lem_fall[(y * 48) + ((24 * editor.ui.lem[i].direction) + (editor.ui.lem[i].currSprite * 6) + x)];
                                if (pixel != palette[PAL_LEMCKEY])
                                    tempScreenBuffer[((editor.ui.lem[i].y + y) * SCREEN_W) + (editor.ui.lem[i].x + x)] = pixel;
                            }
                        }
                    }
                    else
                    {
                        for (y = 0; y < 16; ++y)
                        {
                            if (((editor.ui.lem[i].y + y) < 0) || ((editor.ui.lem[i].y + y) >= SCREEN_H))
                                continue;

                            for (x = 0; x < 9; ++x)
                            {
                                if (((editor.ui.lem[i].x + x) < 0) || ((editor.ui.lem[i].x + x) >= SCREEN_W))
                                    continue;

                                pixel = lem_fall_umbrella[(y * 144) + ((72 * editor.ui.lem[i].direction) + (editor.ui.lem[i].currSprite * 9) + x)];
                                if (pixel != palette[PAL_LEMCKEY])
                                    tempScreenBuffer[((editor.ui.lem[i].y + y) * SCREEN_W) + (editor.ui.lem[i].x + x)] = pixel;
                            }
                        }
                    }

                    // move/update lemming
                    if (editor.ui.lem[i].counter < 64)
                    {
                        editor.ui.lem[i].regularFalling = true;

                        // slower sprite animation speed
                        if ((editor.ui.lemCounter & 3) == 3)
                            editor.ui.lem[i].currSprite = (editor.ui.lem[i].currSprite + 1) & (4 - 1);

                        editor.ui.lem[i].y++; // normal y increase on all frames
                    }
                    else if (editor.ui.lem[i].counter < (64 + 12))
                    {
                        editor.ui.lem[i].regularFalling = false;
                        editor.ui.lem[i].currSprite     = umbrellaOpenTable[editor.ui.lem[i].counter - 64];

                        editor.ui.lem[i].y++; // normal y increase on all frames
                    }
                    else
                    {
                        editor.ui.lem[i].umbrellaOpened = true;
                        editor.ui.lem[i].currSprite     = (editor.ui.lem[i].counter >= (64 + 26)) ? umbrellaTable[editor.ui.lem[i].counter % sizeof (umbrellaTable)] : 5;

                        // no y increase for a short time after umbrella animation
                        if ((editor.ui.lem[i].counter >= (64 + 20)) && ((editor.ui.lemCounter & 1) == 1))
                            editor.ui.lem[i].y++;
                    }

                    if (editor.ui.lem[i].y >= (SCREEN_H - 14))
                    {
                        setLemmingMode(i, MODE_WALK);
                        editor.ui.lem[i].y = SCREEN_H - 10; // new sprite has a new height, skew y position
                    }

                    editor.ui.lem[i].counter++;
                }
                break;

                case MODE_WALK:
                {
                    // render lemming
                    for (y = 0; y < 10; ++y)
                    {
                        if (((editor.ui.lem[i].y + y) < 0) || ((editor.ui.lem[i].y + y) >= SCREEN_H))
                            continue;

                        for (x = 0; x < 6; ++x)
                        {
                            if (((editor.ui.lem[i].x + x) < 0) || ((editor.ui.lem[i].x + x) >= SCREEN_W))
                                continue;

                            pixel = lem_walk[(y * 96) + ((48 * editor.ui.lem[i].direction) + (editor.ui.lem[i].currSprite * 6) + x)];
                            if (pixel != palette[PAL_LEMCKEY])
                                tempScreenBuffer[((editor.ui.lem[i].y + y) * SCREEN_W) + (editor.ui.lem[i].x + x)] = pixel;
                        }
                    }

                    // move/update lemming
                    if ((editor.ui.lemCounter & 3) == 3)
                    {
                        if (editor.ui.lem[i].direction == DIRECTION_LEFT)
                        {
                            if (--editor.ui.lem[i].x <= -2)
                            {
                                if ((rand() % 6) == 0) // the lemming will climb at random
                                {
                                    setLemmingMode(i, MODE_CLIMB);
                                    editor.ui.lem[i].x = -1; // new sprite has a new width, skew x position
                                }
                                else
                                {
                                    editor.ui.lem[i].direction  = DIRECTION_RIGHT;
                                    editor.ui.lem[i].currSprite = 0;
                                }
                            }
                        }
                        else
                        {
                            if (++editor.ui.lem[i].x >= (SCREEN_W - 5))
                            {
                                if ((rand() % 6) == 0) // the lemming will climb at random
                                {
                                    setLemmingMode(i, MODE_CLIMB);
                                    editor.ui.lem[i].x = SCREEN_W - 8; // new sprite has a new width, skew x position
                                }
                                else
                                {
                                    editor.ui.lem[i].direction  = DIRECTION_LEFT;
                                    editor.ui.lem[i].currSprite = 0;
                                }
                            }
                        }

                        editor.ui.lem[i].currSprite = (editor.ui.lem[i].currSprite + 1) & (8 - 1);
                    }
                }
                break;

                case MODE_CLIMB:
                {
                    // render lemming
                    for (y = 0; y < 11; ++y)
                    {
                        if (((editor.ui.lem[i].y + y) < 0) || ((editor.ui.lem[i].y + y) >= SCREEN_H))
                            continue;

                        for (x = 0; x < 9; ++x)
                        {
                            if (((editor.ui.lem[i].x + x) < 0) || ((editor.ui.lem[i].x + x) >= SCREEN_W))
                                continue;

                            pixel = lem_climb[(y * 144) + ((72 * editor.ui.lem[i].direction) + (editor.ui.lem[i].currSprite * 9) + x)];
                            if (pixel != palette[PAL_LEMCKEY])
                                tempScreenBuffer[((editor.ui.lem[i].y + y) * SCREEN_W) + (editor.ui.lem[i].x + x)] = pixel;
                        }
                    }

                    // move/update lemming
                    if ((editor.ui.lemCounter & 3) == 3)
                    {
                        if (editor.ui.lem[i].y <= -2)
                        {
                            setLemmingMode(i, MODE_FALL);

                            // new sprite has a new width, skew x position
                            editor.ui.lem[i].x += ((editor.ui.lem[i].direction == DIRECTION_RIGHT) ? -1 : +1);

                            // new sprite has a new height, skew y position
                            editor.ui.lem[i].y = 1;
                        }

                        // the climbing lemming only increases y position on sprite frame 4 to 8
                        if ((editor.ui.lem[i].counter++ & (8 - 1)) > 3)
                            editor.ui.lem[i].y--;

                        editor.ui.lem[i].currSprite = (editor.ui.lem[i].currSprite + 1) & (8 - 1);
                    }
                }
                break;

                default: break;
            }
        }
    }

    editor.ui.lemCounter++;
}

void videoResize(uint32_t width, uint32_t height, int8_t fullscreen)
{
#ifndef USE_OPENGL // make compiler happy
    (void)(width);
    (void)(height);
    (void)(fullscreen);
#else
    uint8_t i;

    GLfloat tex_width;
    GLfloat tex_height;

    if (updating)
        blitFrame();

    if (pixel_buffer_object)
    {
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
        if (buffer)
            glDeleteBuffersARB(1, &buffer);
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
#if SDL_VERSION_ATLEAST(1, 2, 11)
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, true);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, true);
#endif

    if (fullscreen)
        screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN);
    else
        screen = SDL_SetVideoMode(width, height, 32, SDL_OPENGL | SDL_HWSURFACE);

    if (pixel_buffer_object)
    {
        glGenBuffersARB(1, &buffer);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, buffer);
        glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_EXT, SCREEN_W * SCREEN_H * sizeof (int32_t), NULL, GL_STREAM_DRAW_ARB);
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
    }
    else if (tempScreenBuffer == NULL)
    {
        tempScreenBuffer = (uint32_t *)(malloc(SCREEN_W * SCREEN_H * sizeof (int32_t)));
    }

    if (fullscreen)
    {
        // to maintain aspect ratio, find highest scaling that fits on sccreen
        for (i = 8; i > 0; i--)
        {
            if (((uint32_t)(SCREEN_W * i) <= width) && ((uint32_t)(SCREEN_H * i) <= height))
                break;
        }

        if (i == 0)
            i = 1;

        // center image
        glViewport((width - (SCREEN_W * i)) / 2, (height - (SCREEN_H * i)) / 2, SCREEN_W * i, SCREEN_H * i);
        editor.ui.screenScaling = i;

        input.mouse.scale = floorf(((float)(i) / 2.0f) + 0.5f);
        if (input.mouse.scale < 1.0f)
            input.mouse.scale = 1.0f;

        input.mouse.edgeX = ((SCREEN_W * i) / 2) - 128;
        input.mouse.edgeY = ((SCREEN_H * i) / 2) - 128;
    }
    else
    {
        glViewport(0, 0, width, height);
    }

    glMatrixMode(GL_PROJECTION);
    glDeleteTextures(1, &texture);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 512, 512, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, 0);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapBuffers();
    glClear(GL_COLOR_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    tex_width  = (GLfloat)(SCREEN_W) / 512.0f;
    tex_height = (GLfloat)(SCREEN_H) / 512.0f;

    if (glIsList(displaylist))
        glDeleteLists(displaylist, 1);

    displaylist = glGenLists(1);
    glNewList(displaylist, GL_COMPILE);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, tex_height);
        glVertex2f(-1.0f, -1.0f);

        glTexCoord2f(tex_width, tex_height);
        glVertex2f(1.0f, -1.0f);

        glTexCoord2f(tex_width, 0.0f);
        glVertex2f(1.0f, 1.0f);

        glTexCoord2f(0.0f, 0.0f);
        glVertex2f(-1.0f, 1.0f);
    glEnd();
    glEndList();
#endif
}

void freeDisplayBuffers(void)
{
#if USE_OPENGL
    if (updating)
        blitFrame();

    updating = false;

    if (pixel_buffer_object)
    {
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, 0);
        if (buffer)
            glDeleteBuffersARB(1, &buffer);
    }
    else if (tempScreenBuffer != NULL)
    {
        free(tempScreenBuffer);
        tempScreenBuffer = NULL;
    }

#else
    SDL_FreeSurface(tempScreenSurface);
#endif
}

void startGfxUpdate(void)
{
#if USE_OPENGL
    if (updating)
        return;

    updating = true;

    if (pixel_buffer_object)
    {
        glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, buffer);
        tempScreenBuffer = (uint32_t *)(glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_EXT, GL_WRITE_ONLY));
    }
#endif
}

void handleAskNo(MODULE *modEntry)
{
    switch (editor.ui.askScreenType)
    {
        case ASK_SAVEMOD_OVERWRITE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveModule(modEntry, DONT_CHECK_IF_FILE_EXIST, GIVE_NEW_FILENAME);
        }
        break;

        case ASK_SAVESMP_OVERWRITE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveSample(modEntry, DONT_CHECK_IF_FILE_EXIST, GIVE_NEW_FILENAME);
        }
        break;

        case ASK_DOWNSAMPLING:
        {
            extLoadWAVSampleCallback(modEntry, DONT_DOWNSAMPLE);

            pointerSetPreviousMode();
            setPrevStatusMessage();
        }
        break;

        default:
        {
            pointerSetPreviousMode();
            setPrevStatusMessage();

            input.mouse.buttonErrorMsgActive = true;
            input.mouse.buttonErrorMsgBlock  = true;
            input.mouse.errorMsgCounter      = 0;

            pointerErrorMode();
        }
        break;
    }
}

void handleAskYes(MODULE *modEntry)
{
    char fileName[20 + 4 + 1];
    int8_t *tmpSmpBuffer;
    int8_t oldSample;
    int8_t oldRow;

    int32_t j;
    int32_t renderSampleBlock;
    int32_t renderSamplesTodo;
    int32_t renderSampleCounter;
    int32_t oldSamplesPerTick;
    int32_t newLength;
    uint32_t totalSampleCounter;
    uint32_t i;

    MODULE_SAMPLE *s;

    switch (editor.ui.askScreenType)
    {
        case ASK_RESTORE_SAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            redoSampleData(modEntry, editor.currentSample);
        }
        break;

        case ASK_PAT2SMP:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            editor.pat2SmpBuf = (int16_t *)(malloc(0x0001FFFE * sizeof (int16_t)));
            if (editor.pat2SmpBuf == NULL)
            {
                displayErrorMsg("OUT OF MEMORY !!!");
                return;
            }

            oldSamplesPerTick     = samplesPerTick;
            oldRow                = editor.songPlaying ? 0 : modEntry->currRow;
            totalSampleCounter    = 0;
            renderSampleCounter   = 0;
            editor.isSMPRendering = true;
            editor.pat2SmpPos     = 0;

            storeTempVariables();
            restartSong();
            editor.BlockMarkFlag = false;

            pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
            setStatusMessage("RENDERING...", NO_CARRY);

            mixerSetSamplesPerTick((PT_STD_BPM * (editor.pat2SmpHQ ? 28836 : 22168)) / (modEntry->currBPM * PAL_AMIGA_VBL_RATE));

            modEntry->row     = oldRow;
            modEntry->currRow = modEntry->row;

            while (editor.isSMPRendering)
            {
                renderSampleBlock = (editor.pat2SmpHQ ? 28836 : 22168) / PAL_AMIGA_VBL_RATE;
                while (renderSampleBlock)
                {
                    renderSamplesTodo = (renderSampleBlock < renderSampleCounter) ? renderSampleBlock : renderSampleCounter;
                    if (renderSamplesTodo > 0)
                    {
                        outputAudioToSample(renderSamplesTodo);

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
                            editor.isSMPRendering = false;

                            break; // rendering is done
                        }

                        renderSampleCounter = samplesPerTick;
                    }
                }
            }

            modEntry->row     = oldRow;
            modEntry->currRow = modEntry->row;

            mixerSetSamplesPerTick(oldSamplesPerTick);

            normalize16bitSigned(editor.pat2SmpBuf, MIN(editor.pat2SmpPos, 0x0001FFFE), NORMALIZE_MONO);

            memset(&modEntry->sampleData[modEntry->samples[editor.currentSample].offset], 0, 0x0001FFFE);

            for (i = 0; i < editor.pat2SmpPos; ++i)
                modEntry->sampleData[modEntry->samples[editor.currentSample].offset + i] = (int8_t)(editor.pat2SmpBuf[i] / 256);

            free(editor.pat2SmpBuf);

            memset(modEntry->samples[editor.currentSample].text, 0, sizeof (modEntry->samples[editor.currentSample].text));

            if (editor.pat2SmpHQ)
            {
                strcpy(modEntry->samples[editor.currentSample].text, "pat2smp (a-3 tune:+5)");
                modEntry->samples[editor.currentSample].fineTune = 5;
            }
            else
            {
                strcpy(modEntry->samples[editor.currentSample].text, "pat2smp (f-3 tune:+1)");
                modEntry->samples[editor.currentSample].fineTune = 1;
            }

            modEntry->samples[editor.currentSample].length       = editor.pat2SmpPos;
            modEntry->samples[editor.currentSample].volume       = 64;
            modEntry->samples[editor.currentSample].loopStart    = 0;
            modEntry->samples[editor.currentSample].loopLength   = 2;

            pointerSetMode(POINTER_MODE_IDLE, DO_CARRY);

            displayMsg("ROWS RENDERED!");
            setMsgPointer();

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            updateWindowTitle(modEntry, MOD_IS_MODIFIED);

            editor.SamplePos = 0;
        }
        break;

        case ASK_SAVE_ALL_SAMPLES:
        {
            oldSample = editor.currentSample;

            for (i = 0; i < MOD_SAMPLES; ++i)
            {
                editor.currentSample = (int8_t)(i);
                if (modEntry->samples[i].length > 2)
                    saveSample(modEntry, DONT_CHECK_IF_FILE_EXIST, GIVE_NEW_FILENAME);
            }

            editor.currentSample = oldSample;

            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();
        }
        break;

        case ASK_MAKE_CHORD:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            MixChordSample(modEntry);
        }
        break;

        case ASK_BOOST_ALL_SAMPLES: // for insane minds
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            removeSampleDataMarking();

            for (i = 0; i < MOD_SAMPLES; ++i)
                BoostSample(modEntry, i);

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
        }
        break;

        case ASK_FILTER_ALL_SAMPLES: // for insane minds
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            removeSampleDataMarking();

            for (i = 0; i < MOD_SAMPLES; ++i)
                FilterSample(modEntry, i);

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
        }
        break;

        case ASK_UPSAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            s = &modEntry->samples[editor.currentSample];

            tmpSmpBuffer = (int8_t *)(malloc(s->length));
            if (tmpSmpBuffer == NULL)
            {
                displayErrorMsg("OUT OF MEMORY !!!");
                return;
            }

            newLength = s->length / 2;
            if (newLength < 2)
                return;

            mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

            memcpy(tmpSmpBuffer, &modEntry->sampleData[s->offset], s->length);
            memset(&modEntry->sampleData[s->offset], 0, 0x0001FFFE);

            // upsample
            for (j = 0; j < newLength; ++j)
                modEntry->sampleData[s->offset + j] = tmpSmpBuffer[j * 2];

            free(tmpSmpBuffer);

            s->length      = newLength;
            s->loopStart  /= 2;
            s->loopLength /= 2;

            if (s->loopLength < 2)
            {
                s->loopStart  = 0;
                s->loopLength = 2;
            }

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            calcRealModSize(modEntry);
            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
        }
        break;

        case ASK_DOWNSAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            s = &modEntry->samples[editor.currentSample];

            tmpSmpBuffer = (int8_t *)(malloc(s->length));
            if (tmpSmpBuffer == NULL)
            {
                displayErrorMsg("OUT OF MEMORY !!!");
                return;
            }

            newLength = s->length * 2;
            if (newLength > 0x0001FFFE)
                newLength = 0x0001FFFE;

            mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

            memcpy(tmpSmpBuffer, &modEntry->sampleData[s->offset], s->length);
            memset(&modEntry->sampleData[s->offset], 0, 0x0001FFFE);

            // downsample
            for (j = 0; j < newLength; ++j)
                modEntry->sampleData[s->offset + j] = tmpSmpBuffer[j / 2];

            free(tmpSmpBuffer);

            s->length = newLength;

            if (s->loopLength > 2)
            {
                s->loopStart  *= 2;
                s->loopLength *= 2;

                if ((s->loopStart + s->loopLength) > s->length)
                {
                    s->loopStart  = 0;
                    s->loopLength = 2;
                }
            }

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            calcRealModSize(modEntry);
            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
        }
        break;

        case ASK_KILL_SAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            mixerKillVoiceIfReadingSample(modEntry, editor.currentSample);

            modEntry->samples[editor.currentSample].fineTune     = 0;
            modEntry->samples[editor.currentSample].volume       = 0;
            modEntry->samples[editor.currentSample].length       = 0;
            modEntry->samples[editor.currentSample].loopStart    = 0;
            modEntry->samples[editor.currentSample].loopLength   = 2;

            memset(&modEntry->samples[editor.currentSample].text, 0, sizeof (modEntry->samples[editor.currentSample].text));
            memset(&modEntry->sampleData[(editor.currentSample * 0x0001FFFE)], 0, 0x0001FFFE);

            setupSampler(modEntry, REMOVE_SAMPLE_MARKING);
            calcRealModSize(modEntry);

            updateWindowTitle(modEntry, MOD_IS_MODIFIED);
        }
        break;

        case ASK_RESAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            resampleSample(modEntry);
        }
        break;

        case ASK_DOWNSAMPLING:
        {
            // for WAV sample loader

            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            extLoadWAVSampleCallback(modEntry, DO_DOWNSAMPLE);
        }
        break;

        case ASK_MOD2WAV_OVERWRITE:
        {
            memset(fileName, 0, sizeof (fileName));

            if (modEntry->head.moduleTitle[0])
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

                strcat(fileName, ".wav");
            }
            else
            {
                strcpy(fileName, "untitled.wav");
            }

            renderToWav(fileName, DONT_CHECK_IF_FILE_EXIST);
        }
        break;

        case ASK_MOD2WAV:
        {
            memset(fileName, 0, sizeof (fileName));

            if (modEntry->head.moduleTitle[0])
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

                strcat(fileName, ".wav");
            }
            else
            {
                strcpy(fileName, "untitled.wav");
            }

            renderToWav(fileName, CHECK_IF_FILE_EXIST);
        }
        break;

        case ASK_QUIT:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            editor.ui.throwExit = true;
        }
        break;

        case ASK_SAVE_SAMPLE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveSample(modEntry, CHECK_IF_FILE_EXIST, DONT_GIVE_NEW_FILENAME);
        }
        break;

        case ASK_SAVESMP_OVERWRITE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveSample(modEntry, DONT_CHECK_IF_FILE_EXIST, DONT_GIVE_NEW_FILENAME);
        }
        break;

        case ASK_SAVE_MODULE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveModule(modEntry, CHECK_IF_FILE_EXIST, DONT_GIVE_NEW_FILENAME);
        }
        break;

        case ASK_SAVEMOD_OVERWRITE:
        {
            input.mouse.buttonErrorMsgActive = false;
            input.mouse.errorMsgCounter      = 0;

            pointerSetPreviousMode();
            setPrevStatusMessage();

            saveModule(modEntry, DONT_CHECK_IF_FILE_EXIST, DONT_GIVE_NEW_FILENAME);
        }
        break;

        default:
            break;
    }
}

void createGraphics(void)
{
    uint8_t x;
    uint8_t y;
    uint8_t i;
    uint8_t j;

    int16_t r;
    int16_t g;
    int16_t b;
    int16_t r2;
    int16_t g2;
    int16_t b2;

    // create pattern cursor graphics
    for (y = 0; y < 14; ++y)
    {
        if (y < 2)
        {
            r = (palette[PAL_CURSOR] & 0x00FF0000) >> 16;
            g = (palette[PAL_CURSOR] & 0x0000FF00) >>  8;
            b = (palette[PAL_CURSOR] & 0x000000FF) >>  0;

            r += 0x33;
            g += 0x33;
            b += 0x33;

            if (r > 0xFF) r = 0xFF;
            if (g > 0xFF) g = 0xFF;
            if (b > 0xFF) b = 0xFF;

            for (x = 0; x < 11; ++x)
                patternCursorBitmap[(y * 11) + x] = 0xFF000000 | (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF));
        }

        if ((y >= 2) && y <= 12)
        {
            patternCursorBitmap[(y * 11) + 0] = palette[PAL_CURSOR];

            for (x = 1; x < 10; ++x)
                patternCursorBitmap[(y * 11) + x] = palette[PAL_COLORKEY];

            patternCursorBitmap[(y * 11) + 10] = palette[PAL_CURSOR];
        }

        if (y > 11)
        {
            r = (palette[PAL_CURSOR] & 0x00FF0000) >> 16;
            g = (palette[PAL_CURSOR] & 0x0000FF00) >>  8;
            b = (palette[PAL_CURSOR] & 0x000000FF) >>  0;

            r -= 0x33;
            g -= 0x33;
            b -= 0x33;

            if (r < 0x00) r = 0x00;
            if (g < 0x00) g = 0x00;
            if (b < 0x00) b = 0x00;

            for (x = 0; x < 11; ++x)
                patternCursorBitmap[(y * 11) + x] = 0xFF000000 | (((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF));
        }
    }

    // create spectrum analyzer bar graphics
    for (i = 0; i < 36; ++i)
    {
        r = ((AnalyzerColors[35 - i] & 0x0F00) >> 8) * 17;
        g = ((AnalyzerColors[35 - i] & 0x00F0) >> 4) * 17;
        b = ((AnalyzerColors[35 - i] & 0x000F) >> 0) * 17;

        spectrumAnaBitmap[i] = 0xFF000000 | ((r & 0xFF) << 16) | ((g & 0xFF) << 8) | (b & 0xFF);
    }

    // create VU-Meter bar graphics
    for (i = 0; i < 48; ++i)
    {
        r = ((VUmeterColors[47 - i] & 0x0F00) >> 8) * 17;
        g = ((VUmeterColors[47 - i] & 0x00F0) >> 4) * 17;
        b = ((VUmeterColors[47 - i] & 0x000F) >> 0) * 17;

        // left brighter pixels
        r2 = r + 0x33;
        g2 = g + 0x33;
        b2 = b + 0x33;

        if (r2 > 0xFF) r2 = 0xFF;
        if (g2 > 0xFF) g2 = 0xFF;
        if (b2 > 0xFF) b2 = 0xFF;

        vuMeterBitmap[(i * 10) + 0] = 0xFF000000 | ((r2 & 0xFF) << 16) | ((g2 & 0xFF) << 8) | (b2 & 0xFF);
        vuMeterBitmap[(i * 10) + 1] = 0xFF000000 | ((r2 & 0xFF) << 16) | ((g2 & 0xFF) << 8) | (b2 & 0xFF);

        // main pixels
        for (j = 2; j < 8; ++j)
            vuMeterBitmap[(i * 10) + j] = 0xFF000000 | (r << 16) | (g << 8) | b;

        // right darker pixels
        r2 = r - 0x33;
        g2 = g - 0x33;
        b2 = b - 0x33;

        if (r2 < 0x00) r2 = 0x00;
        if (g2 < 0x00) g2 = 0x00;
        if (b2 < 0x00) b2 = 0x00;

        vuMeterBitmap[(i * 10) + 8] = 0xFF000000 | ((r2 & 0xFF) << 16) | ((g2 & 0xFF) << 8) | (b2 & 0xFF);
        vuMeterBitmap[(i * 10) + 9] = 0xFF000000 | ((r2 & 0xFF) << 16) | ((g2 & 0xFF) << 8) | (b2 & 0xFF);
    }
}

void freeBitmaps(void)
{
    if (trackerFrameBitmap    != NULL) free(trackerFrameBitmap);
    if (samplerScreenBitmap   != NULL) free(samplerScreenBitmap);
    if (samplerVolumeBitmap   != NULL) free(samplerVolumeBitmap);
    if (clearDialogBitmap     != NULL) free(clearDialogBitmap);
    if (diskOpScreenBitmap    != NULL) free(diskOpScreenBitmap);
    if (mod2wavBitmap         != NULL) free(mod2wavBitmap);
    if (posEdBitmap           != NULL) free(posEdBitmap);
    if (spectrumVisualsBitmap != NULL) free(spectrumVisualsBitmap);
    if (yesNoDialogBitmap     != NULL) free(yesNoDialogBitmap);
    if (editOpScreen1Bitmap   != NULL) free(editOpScreen1Bitmap);
    if (editOpScreen2Bitmap   != NULL) free(editOpScreen2Bitmap);
    if (editOpScreen3Bitmap   != NULL) free(editOpScreen3Bitmap);
    if (editOpScreen4Bitmap   != NULL) free(editOpScreen4Bitmap);
    if (aboutScreenBitmap     != NULL) free(aboutScreenBitmap);
    if (introScreenBitmap     != NULL) free(introScreenBitmap);
    if (muteButtonsBitmap     != NULL) free(muteButtonsBitmap);
    if (editOpModeCharsBitmap != NULL) free(editOpModeCharsBitmap);
    if (arrowBitmap           != NULL) free(arrowBitmap);
}

uint32_t *unpackBitmap(const uint8_t *src, uint32_t packedLen)
{
    const uint8_t *packSrc;
    uint8_t *tmpBuffer;
    uint8_t *packDst;
    uint8_t byteIn;

    int16_t count;

    uint32_t *dst;
    uint32_t decodedLength;
    uint32_t i;

    // RLE decode
    decodedLength = (src[0] << 24) | (src[1] << 16) | (src[2] << 8) | src[3];
    tmpBuffer     = (uint8_t *)(malloc(decodedLength));
    if (tmpBuffer == NULL)
        return (NULL);

    packSrc = src + 4;
    packDst = tmpBuffer;

    i = packedLen - 4;
    while (i > 0)
    {
        byteIn = *packSrc++;
        if (byteIn == 0xCC) // compactor code
        {
            count  = *packSrc++;
            byteIn = *packSrc++;

            while (count >= 0)
            {
                *packDst++ = byteIn;
                count--;
            }

            i -= 2;
        }
        else
        {
            *packDst++ = byteIn;
        }

        i--;
    }

    // 2-bit to 8-bit conversion
    dst = (uint32_t *)(malloc((decodedLength * 4) * sizeof (int32_t)));
    if (dst == NULL)
    {
        free(tmpBuffer);
        return (NULL);
    }

    for (i = 0; i < decodedLength; ++i)
    {
        dst[(i * 4) + 0] = palette[(tmpBuffer[i] & 0xC0) >> 6];
        dst[(i * 4) + 1] = palette[(tmpBuffer[i] & 0x30) >> 4];
        dst[(i * 4) + 2] = palette[(tmpBuffer[i] & 0x0C) >> 2];
        dst[(i * 4) + 3] = palette[(tmpBuffer[i] & 0x03) >> 0];
    }

    free(tmpBuffer);

    return (dst);
}

int8_t unpackBitmaps(void)
{
    uint8_t i;

    trackerFrameBitmap    = unpackBitmap(trackerFramePackedBitmap,    sizeof (trackerFramePackedBitmap));
    samplerScreenBitmap   = unpackBitmap(samplerScreenPackedBitmap,   sizeof (samplerScreenPackedBitmap));
    samplerVolumeBitmap   = unpackBitmap(samplerVolumePackedBitmap,   sizeof (samplerVolumePackedBitmap));
    clearDialogBitmap     = unpackBitmap(clearDialogPackedBitmap,     sizeof (clearDialogPackedBitmap));
    diskOpScreenBitmap    = unpackBitmap(diskOpScreenPackedBitmap,    sizeof (diskOpScreenPackedBitmap));
    mod2wavBitmap         = unpackBitmap(mod2wavPackedBitmap,         sizeof (mod2wavPackedBitmap));
    posEdBitmap           = unpackBitmap(posEdPackedBitmap,           sizeof (posEdPackedBitmap));
    spectrumVisualsBitmap = unpackBitmap(spectrumVisualsPackedBitmap, sizeof (spectrumVisualsPackedBitmap));
    yesNoDialogBitmap     = unpackBitmap(yesNoDialogPackedBitmap,     sizeof (yesNoDialogPackedBitmap));
    editOpScreen1Bitmap   = unpackBitmap(editOpScreen1PackedBitmap,   sizeof (editOpScreen1PackedBitmap));
    editOpScreen2Bitmap   = unpackBitmap(editOpScreen2PackedBitmap,   sizeof (editOpScreen2PackedBitmap));
    editOpScreen3Bitmap   = unpackBitmap(editOpScreen3PackedBitmap,   sizeof (editOpScreen3PackedBitmap));
    editOpScreen4Bitmap   = unpackBitmap(editOpScreen4PackedBitmap,   sizeof (editOpScreen4PackedBitmap));
    aboutScreenBitmap     = unpackBitmap(aboutScreenPackedBitmap,     sizeof (aboutScreenPackedBitmap));
    introScreenBitmap     = unpackBitmap(introScreenPackedBitmap,     sizeof (introScreenPackedBitmap));
    muteButtonsBitmap     = unpackBitmap(muteButtonsPackedBitmap,     sizeof (muteButtonsPackedBitmap));
    editOpModeCharsBitmap = unpackBitmap(editOpModeCharsPackedBitmap, sizeof (editOpModeCharsPackedBitmap));
    arrowBitmap           = (uint32_t *)(malloc(30 * sizeof (int32_t))); // different format

    if (
        (trackerFrameBitmap  == NULL) || (samplerScreenBitmap   == NULL) || (samplerVolumeBitmap == NULL) ||
        (clearDialogBitmap   == NULL) || (diskOpScreenBitmap    == NULL) || (mod2wavBitmap       == NULL) ||
        (posEdBitmap         == NULL) || (spectrumVisualsBitmap == NULL) || (yesNoDialogBitmap   == NULL) ||
        (editOpScreen1Bitmap == NULL) || (editOpScreen2Bitmap   == NULL) || (editOpScreen3Bitmap == NULL) ||
        (editOpScreen4Bitmap == NULL) || (aboutScreenBitmap     == NULL) || (introScreenBitmap   == NULL) ||
        (muteButtonsBitmap   == NULL) || (editOpModeCharsBitmap == NULL) || (arrowBitmap         == NULL)
       )
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    for (i = 0; i < 30; ++i)
        arrowBitmap[i] = palette[arrowPaletteBitmap[i]];

    createGraphics();

    return (true);
}
