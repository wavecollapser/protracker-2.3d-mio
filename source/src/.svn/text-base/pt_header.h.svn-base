/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** All of the files are considered 'public domain',
** do whatever you want with it.
**
*/

#ifndef __PT_HEADER_H
#define __PT_HEADER_H

#include <SDL/SDL.h>
#include <stdint.h>

#ifdef WIN32
#define PATH_MAX_LEN 260
#else
#define PATH_MAX_LEN 4096
#endif

#ifdef _MSC_VER
#pragma warning(disable:4820) // disable struct padding warnings
#pragma warning(disable:4996) // disable deprecated POSIX warnings
#pragma warning(disable:4127) // disable while (true) warnings
#endif

#include "pt_config.h" // this must be included after PATH_MAX_LEN definition

#define MAX_LEMMINGS 100

#define SCREEN_W 320
#define SCREEN_H 255

#define FONT_CHAR_W 8 // actual data length is 7, includes right spacing (1px column)
#define FONT_CHAR_H 5

#define MOD_ROWS 64
#define MOD_SAMPLES 31
#define MOD_ORDERS 128
#define MAX_PATTERNS 100

#define HIGHEST_NOTE_PERIOD 856
#define PAULA_CHANNELS 4
#define SCOPE_WIDTH 40
#define SPECTRUM_BAR_NUM 23
#define SPECTRUM_BAR_HEIGHT 36
#define SPECTRUM_BAR_WIDTH 6
#define PT_STD_BPM 125

#define POSED_LIST_SIZE 12

#define PAL_AMIGA_VBL_RATE 50
#define PAL_AMIGA_PAULA_CC_f 3546895.0f

#define KEYB_REPEAT_DELAY 20

#ifdef __APPLE__
#define SDLK_LWIN SDLK_LMETA
#define SDLK_RWIN SDLK_RMETA
#else
#define SDLK_LWIN SDLK_LSUPER
#define SDLK_RWIN SDLK_RSUPER
#endif

enum
{
    FORMAT_MK,     // ProTracker 1.x
    FORMAT_MK2,    // ProTracker 2.x (if tune has >64 patterns)
    FORMAT_FLT4,   // StarTrekker
    FORMAT_4CHN,   // FastTracker II (only 4 channel MODs)
    FORMAT_STK,    // The Ultimate SoundTracker (15 samples)
    FORMAT_NT,     // NoiseTracker 1.0
    FORMAT_UNKNOWN
};

enum
{
    NO_CARRY = 0,
    DO_CARRY = 1,

    INCREMENT_SLOW = 0,
    INCREMENT_FAST = 1,

    NO_SAMPLE_CUT = 0,
    SAMPLE_CUT    = 1,

    EDIT_SPECIAL = 0,
    EDIT_NORMAL  = 1,

    EDIT_TEXT_NO_UPDATE = 0,
    EDIT_TEXT_UPDATE    = 1,

    TRANSPOSE_NORMAL = 0,
    TRANSPOSE_SAMPLE = 1,

    MOUSE_BUTTON_NOT_HELD = 0,
    MOUSE_BUTTON_HELD     = 1,

    OPENGL_WINDOWED   = 0,
    OPENGL_FULLSCREEN = 1,

    DONT_SET_ORDER   = -1,
    DONT_SET_PATTERN = -1,
    DONT_SET_ROW     = -1,

    NORMALIZE_MONO   = 0,
    NORMALIZE_STEREO = 1,

    REMOVE_SAMPLE_MARKING = 0,
    KEEP_SAMPLE_MARKING   = 1,

    MOD_NOT_MODIFIED = 0,
    MOD_IS_MODIFIED  = 1,

    DONT_CHECK_IF_FILE_EXIST = 0,
    CHECK_IF_FILE_EXIST      = 1,

    DONT_GIVE_NEW_FILENAME = 0,
    GIVE_NEW_FILENAME      = 1,

    DONT_DOWNSAMPLE = 0,
    DO_DOWNSAMPLE   = 1,

    SCREEN_ALL         = 0,
    SCREEN_MAINSCREEN  = 1,
    SCREEN_DISKOP      = 2,
    SCREEN_SAMPLER     = 4,
    SCREEN_QUIT        = 8,
    SCREEN_CLEAR       = 16,
    VISUAL_QUADRASCOPE = 0,
    VISUAL_SPECTRUM    = 1,

    MODE_IDLE   = 0,
    MODE_EDIT   = 1,
    MODE_PLAY   = 2,
    MODE_RECORD = 3,

    RECORD_PATT = 0,
    RECORD_SONG = 1,

    CURSOR_NOTE    = 0,
    CURSOR_SAMPLE1 = 1,
    CURSOR_SAMPLE2 = 2,
    CURSOR_CMD     = 3,
    CURSOR_PARAM1  = 4,
    CURSOR_PARAM2  = 5,

    PLAY_MODE_NORMAL  = 0,
    PLAY_MODE_PATTERN = 1,

    OCTAVE_HIGH = 0,
    OCTAVE_LOW  = 1,

    AMIGA_PAL  = 0,
    AMIGA_NTSC = 1,

    DISKOP_MODE_MOD = 0,
    DISKOP_MODE_SMP = 1,

    DISKOP_SMP_WAV = 0,
    DISKOP_SMP_IFF = 1,
    DISKOP_SMP_RAW = 2,

    ASK_QUIT                  =  0,
    ASK_SAVE_MODULE           =  1,
    ASK_SAVE_SONG             =  2,
    ASK_SAVE_SAMPLE           =  3,
    ASK_MOD2WAV               =  4,
    ASK_MOD2WAV_OVERWRITE     =  5,
    ASK_SAVEMOD_OVERWRITE     =  6,
    ASK_SAVESMP_OVERWRITE     =  7,
    ASK_DOWNSAMPLING          =  8,
    ASK_RESAMPLE              = 9,
    ASK_KILL_SAMPLE           = 10,
    ASK_UPSAMPLE              = 11,
    ASK_DOWNSAMPLE            = 12,
    ASK_FILTER_ALL_SAMPLES    = 13,
    ASK_BOOST_ALL_SAMPLES     = 14,
    ASK_MAKE_CHORD            = 15,
    ASK_SAVE_ALL_SAMPLES      = 16,
    ASK_PAT2SMP               = 17,
    ASK_RESTORE_SAMPLE        = 18,

    TEMPO_MODE_CIA    = 0,
    TEMPO_MODE_VBLANK = 1,

    TEXT_EDIT_STRING  = 0,
    TEXT_EDIT_DECIMAL = 1,
    TEXT_EDIT_HEX     = 2,

    // for lemmings easter-egg
    MODE_FALL  = 0,
    MODE_WALK  = 1,
    MODE_CLIMB = 2,

    DIRECTION_RIGHT = 0,
    DIRECTION_LEFT  = 1,
    // -------------------------
};

typedef struct
{
    uint8_t mode;
    uint8_t spawned;
    uint8_t direction;
    uint8_t currSprite;
    uint8_t regularFalling;
    uint8_t umbrellaOpened;
    int16_t x;
    int16_t y;
    uint16_t counter;
} LEMMING;

typedef struct
{
    uint32_t ChunkID;
    uint32_t ChunkSize;
    uint32_t Format;
    uint32_t Subchunk1ID;
    uint32_t Subchunk1Size;
    uint16_t AudioFormat;
    uint16_t NumChannels;
    uint32_t SampleRate;
    uint32_t ByteRate;
    uint16_t BlockAlign;
    uint16_t BitsPerSample;
    uint32_t Subchunk2ID;
    uint32_t Subchunk2Size;
} WAV_HEADER;

typedef struct modnote
{
    uint8_t param;
    uint8_t sample;
    uint8_t command;
    uint16_t period;
} modnote_t;

typedef struct
{
    char moduleTitle[20 + 1];

    uint8_t ticks;
    uint8_t format;
    uint8_t restartPos;

    int16_t order[MOD_ORDERS];
    int16_t orderCount;
    int16_t patternCount;
    uint16_t tempo;
    uint16_t initBPM;

    uint32_t moduleSize;
    uint32_t totalSampleSize;
} MODULE_HEADER;

typedef struct
{
    volatile int8_t *volumeDisp;
    volatile int32_t *lengthDisp;
    volatile int32_t *loopStartDisp;
    volatile int32_t *loopLengthDisp;

    char text[22];
    int8_t volume;
    uint8_t fineTune;

    int32_t length;
    int32_t offset;
    int32_t loopStart;
    int32_t loopLength;
    int32_t tmpLoopStart;
} MODULE_SAMPLE;

typedef struct
{
    volatile int8_t scopeVolume;
    volatile int16_t scopePeriod;
    volatile int32_t scopePos;

    int8_t *invertLoopPtr;
    int8_t *invertLoopStart;
    int8_t volume;
    int8_t noNote;
    int8_t rawVolume;
    int8_t tremoloPos;
    int8_t vibratoPos;
    int8_t scopeEnabled;
    int8_t scopeTrigger;
    int8_t scopeLoopFlag;
    int8_t patternLoopRow;
    int8_t scopeChangePos;
    int8_t scopeKeepPeriod;
    int8_t scopeKeepVolume;
    int8_t offsetBugNotAdded;
    int8_t patternLoopCounter;
    int8_t toneportdirec;
    int8_t DidQuantize;
    uint8_t param;
    uint8_t flags;
    uint8_t sample;
    uint8_t command;
    uint8_t fineTune;
    uint8_t chanIndex;
    uint8_t tempFlags;
    uint8_t tremoloCmd;
    uint8_t vibratoCmd;
    uint8_t waveControl;
    uint8_t toneportspeed;
    uint8_t invertLoopDelay;
    uint8_t invertLoopSpeed;
    uint8_t tempFlagsBackup;
    uint8_t glissandoControl;

    int16_t wantedperiod;
    int16_t period;
    int16_t tempPeriod;

    int32_t offset;
    int32_t offsetTemp;
    int32_t invertLoopLength;
    int32_t scopeLoopQuirk;
    int32_t scopeEnd;
    int32_t scopeLoopBegin;
    int32_t scopeLoopEnd;
} mod_channel;

typedef struct
{
    int8_t *sampleData;
    int8_t currRow;
    int8_t modified;
    int8_t row;
    uint8_t currSpeed;
    uint8_t moduleLoaded;

    int16_t currOrder;
    int16_t currPattern;
    uint16_t currBPM;

    uint32_t rowsCounter;
    uint32_t rowsInTotal;

    MODULE_HEADER head;
    MODULE_SAMPLE samples[MOD_SAMPLES];
    modnote_t *patterns[MAX_PATTERNS];
    mod_channel channels[PAULA_CHANNELS];
} MODULE;

struct audio_t
{
    uint32_t tempoTimerVal;
} audio;

struct input_t
{
    struct keyb_t
    {
        int8_t repeatKey;
        int8_t delayKey;
        uint8_t shiftKeyDown;
        uint8_t controlKeyDown;
        uint8_t altKeyDown;
        uint8_t leftAmigaKeyDown;
        uint8_t keypadEnterKeyDown;
        uint8_t repeatCounter;
        uint8_t delayCounter;

        SDLKey lastKey;
    } keyb;

    struct mouse_t
    {
        int8_t buttonWaiting;
        int8_t leftButtonPressed;
        int8_t rightButtonPressed;
        int8_t buttonErrorMsgActive;
        int8_t buttonErrorMsgBlock;
        uint8_t repeatCounter;
        uint8_t errorMsgCounter;
        uint8_t buttonWaitCounter;

        int32_t lastClickedGUIButton;
        int32_t x;
        int32_t y;
        int32_t edgeX;
        int32_t edgeY;
        int32_t lastMouseX;

        float scale;
    } mouse;
} input;

// this is massive... not exactly eyecandy!
struct editor_t
{
    volatile int8_t vuMeterVolumes[PAULA_CHANNELS];
    volatile int8_t spectrumVolumes[SPECTRUM_BAR_NUM];
    volatile int8_t *SampleFromDisp;
    volatile int8_t *SampleToDisp;
    volatile int8_t *currentSampleDisp;
    volatile uint8_t isWAVRendering;
    volatile uint8_t isSMPRendering;
    volatile uint8_t modTick;
    volatile uint8_t modSpeed;
    volatile uint8_t programRunning;

    volatile int16_t *QuantizeValueDisp;
    volatile int16_t *MetroSpeedDisp;
    volatile int16_t *MetroChannelDisp;
    volatile int16_t *SampleVolDisp;
    volatile int16_t *Vol1Disp;
    volatile int16_t *Vol2Disp;
    volatile int16_t *currentEditPatternDisp;
    volatile int16_t *currentPosDisp;
    volatile int16_t *currentPatternDisp;
    volatile int16_t *currentPosEdPattDisp;
    volatile int16_t *currentLengthDisp;

    volatile int32_t *SamplePosDisp;
    volatile int32_t *ChordLengthDisp;

    char MixText[16];
    char *filenameTmp;
    char *entrynameTmp;
    char *currentPath;
    char *displayPath;
    char *tempPath;
    int8_t sampleRedoFinetunes[MOD_SAMPLES];
    int8_t sampleRedoVolumes[MOD_SAMPLES];
    int8_t MultiModeNext[4];
    int8_t *sampleRedoBuffer[MOD_SAMPLES];
    int8_t currentSample;
    int8_t MetroFlag;
    int8_t RecordMode;
    int8_t SampleFrom;
    int8_t MultiFlag;
    int8_t SampleTo;
    int8_t keypadSampleOffset;
    int8_t SampleAllFlag;
    int8_t TrackPattFlag;
    int8_t HalfClipFlag;
    int8_t NewOldFlag;
    int8_t pat2SmpHQ;
    int8_t MixFlag;
    int8_t Note1;
    int8_t Note2;
    int8_t Note3;
    int8_t Note4;
    int8_t OldNote1;
    int8_t OldNote2;
    int8_t OldNote3;
    int8_t OldNote4;
    int8_t modLoaded;
    int8_t fullScreenFlag;
    int8_t AutoInsFlag;
    int8_t NoteRepFlag;
    int8_t SampleZero;
    int8_t Accidental;
    int8_t TransDelFlag;
    int8_t ChordLengthMin;
    uint8_t muted[PAULA_CHANNELS];
    uint8_t *rowVisitTable;
    uint8_t playMode;
    uint8_t songPlaying;
    uint8_t currentMode;
    uint8_t useLEDFilter;
    uint8_t tuningFlag;
    uint8_t pnoteflag;
    uint8_t tuningVol;
    uint8_t stepPlayEnabled;
    uint8_t stepPlayBackwards;
    uint8_t BlockBufferFlag;
    uint8_t BuffFromPos;
    uint8_t BuffToPos;
    uint8_t BlockFromPos;
    uint8_t BlockToPos;
    uint8_t BlockMarkFlag;
    uint8_t timingMode;
    uint8_t swapChannelFlag;
    uint8_t F6pos;
    uint8_t F7pos;
    uint8_t F8pos;
    uint8_t F9pos;
    uint8_t F10pos;
    uint8_t keyOctave;
    uint8_t tuningNote;
    uint8_t ResampleNote;
    uint8_t initialTempo;
    uint8_t initialSpeed;
    uint8_t EditMoveAdd;

    int16_t *mod2WavBuffer;
    int16_t *pat2SmpBuf;
    int16_t Vol1;
    int16_t Vol2;
    int16_t QuantizeValue;
    int16_t MetroSpeed;
    int16_t MetroChannel;
    int16_t SampleVol;
    int16_t ModulateSpeed;
    uint16_t EffectMacros[10];
    uint16_t oldBPM;
    uint16_t currentPlayNote;
    uint16_t ticks50Hz;

    int32_t sampleRedoLoopStarts[MOD_SAMPLES];
    int32_t sampleRedoLoopLengths[MOD_SAMPLES];
    int32_t sampleRedoLengths[MOD_SAMPLES];
    int32_t MarkStartOfs;
    int32_t MarkEndOfs;
    int32_t MarkStart;
    int32_t MarkEnd;
    int32_t SamplePos;
    int32_t ModulatePos;
    int32_t ModulateOffset;
    int32_t ChordLength;
    int32_t playTime;
    uint32_t pat2SmpPos;
    uint32_t outputFreq;

    modnote_t TrackBuffer[MOD_ROWS];
    modnote_t CmdsBuffer[MOD_ROWS];
    modnote_t BlockBuffer[MOD_ROWS];
    modnote_t PatternBuffer[MOD_ROWS * PAULA_CHANNELS];
    modnote_t UndoBuffer[MOD_ROWS * PAULA_CHANNELS];

    SDL_Thread *mod2WavThread;
    SDL_Thread *pat2SmpThread;

    struct diskop_t
    {
        volatile uint8_t cached;
        volatile uint8_t isFilling;
        volatile uint8_t forceStopReading;

        int8_t moddot;
        int8_t mode;
        int8_t modPackFlg;
        int8_t smpSaveType;

        int32_t numFiles;
        int32_t scrollOffset;

        SDL_Thread *fillThread;
    } diskop;

    struct cursor_t
    {
        uint8_t pos;
        uint8_t mode;
        uint8_t channel;
    } cursor;

    struct text_offsets_t
    {
        uint16_t diskOpPath;
        uint16_t posEdPattName;
    } textofs;

    struct ui_t
    {
        char *pattNames;
        char statusMessage[18];
        char previousStatusMessage[18];
        char *dstPtr;
        char *editPos;
        char *textEndPtr;
        char *showTextPtr;

        int8_t *numPtr8;
        int8_t tmpDisp8;
        int8_t answerNo;
        int8_t answerYes;
        int8_t throwExit;
        int8_t pointerMode;
        int8_t getLineFlag;
        int8_t getLineType;
        int8_t doEasterEgg;
        int8_t askScreenType;
        int8_t askScreenShown;
        int8_t visualizerMode;
        int8_t leftLoopPinMoving;
        int8_t rightLoopPinMoving;
        int8_t samplerScreenShown;
        int8_t previousPointerMode;
        int8_t scale3x;
        int8_t blankzero;
        int8_t changingSmpResample;
        int8_t changingDrumPadNote;
        int8_t changingChordNote;
        int8_t forceSampleDrag;
        int8_t forceVolDrag;
        int8_t forceSampleEdit;
        int8_t introScreenShown;
        int8_t aboutScreenShown;
        int8_t clearScreenShown;
        int8_t posEdScreenShown;
        int8_t diskOpScreenShown;
        int8_t samplerVolBoxShown;
        int8_t editOpScreenShown;
        int8_t editOpScreen;
        uint8_t numLen;
        uint8_t numBits;
        uint8_t digitStart;
        uint8_t numLemmings;
        uint8_t screenScaling;

        int16_t *numPtr16;
        int16_t editID;
        int16_t tmpDisp16;
        int16_t lineCurX;
        int16_t lineCurY;
        int16_t sampleMarkingPos;
        uint16_t *dstOffset;
        uint16_t dstPos;
        uint16_t lemCounter;
        uint16_t textLength;
        uint16_t editTextPos;
        uint16_t dstOffsetEnd;
        uint16_t lastSampleOffset;

        int32_t *numPtr32;
        int32_t tmpDisp32;
        int32_t askTempData;

        SDL_PixelFormat *pixelFormat;

        LEMMING lem[MAX_LEMMINGS];
    } ui;
} editor;

void restartSong(void);
void resetSong(void);
void incPatt(void);
void decPatt(void);
void modSetPos(int16_t order, int16_t row);
void modStop(void);
void doStopIt(void);
void playPattern(int8_t startRow);
void modPlay(int16_t patt, int16_t order, int8_t row);
void modSetSpeed(uint8_t speed);
void modSetTempo(uint16_t bpm);
void modFree(MODULE *modEntry);
int8_t setupAudio(const PTCONFIG *config);
void audioClose(void);
void setPlayerModSource(MODULE *modEntry);
void setMainModSource(MODULE *modEntry);
void clearSong(void);
void clearSamples(void);
void clearAll(void);
void drawEasterEgg(void);

extern uint8_t bigEndian; // pt_main.c

#endif
