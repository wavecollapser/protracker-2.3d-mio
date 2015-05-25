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
#include <ctype.h> // for toupper()/tolower()
#include <math.h>
#include <SDL/SDL.h>
#ifdef WIN32
#include <direct.h>
#include <io.h>
#else
#include <unistd.h>
#endif
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include "pt_header.h"
#include "pt_dirent.h"
#include "pt_textout.h"
#include "pt_diskop.h"
#include "pt_tables.h"
#include "pt_palette.h"
#include "pt_modloader.h"
#include "pt_audio.h"
#include "pt_sampler.h"
#include "pt_config.h"
#include "pt_helpers.h"

static int8_t firstCached = false;

static uint32_t *diskOpEntrySize;
static char *diskOpEntryType;
static char **diskOpEntryDate;
static char **diskOpEntryPath;
static char **diskOpEntry;

void handleEntryJumping(SDLKey keyEntry)
{
    char jumpToChar;
    int8_t j;

    int32_t i;

    jumpToChar = (char)(keyEntry);

    // slow and unintelligent method, but whatever
    if (editor.diskop.numFiles >= 10)
    {
        for (i = 0; i <= (editor.diskop.numFiles - 10); i++)
        {
            for (j = 0; j < 10; ++j)
            {
                if (jumpToChar == (char)(tolower(diskOpEntry[i + j][0])))
                {
                    // fix visual overrun
                    while ((i + j) > (editor.diskop.numFiles - 10)) j--;

                    editor.diskop.scrollOffset = i + j;

                    return;
                }
            }
        }
    }
    else
    {
        // the reason we still search, is to throw an error
        // message if there is no such filename starting
        // with given character.
        for (i = 0; i < editor.diskop.numFiles; ++i)
        {
            if (jumpToChar == (char)(tolower(diskOpEntry[i][0])))
                return;
        }
    }

    // character not found in filelist, show red pointer!
    input.mouse.buttonErrorMsgActive = true;
    input.mouse.buttonErrorMsgBlock  = true;
    input.mouse.errorMsgCounter      = 0;

    pointerErrorMode();

    return;
}

int8_t diskOpEntryIsDir(int32_t fileIndex)
{
    if ((editor.diskop.scrollOffset + fileIndex) < editor.diskop.numFiles)
        return (diskOpEntryType[editor.diskop.scrollOffset + fileIndex] == DISKOP_DIR);

    return (-1);
}

char *diskOpGetEntry(int32_t fileIndex)
{
    if ((editor.diskop.scrollOffset + fileIndex) < editor.diskop.numFiles)
    {
        if (diskOpEntry[editor.diskop.scrollOffset + fileIndex][0] != '\0')
            return (diskOpEntry[editor.diskop.scrollOffset + fileIndex]);
    }

    return (NULL);
}

char *diskOpGetEntryPath(int32_t fileIndex)
{
    if ((editor.diskop.scrollOffset + fileIndex) < editor.diskop.numFiles)
    {
        if (diskOpEntry[editor.diskop.scrollOffset + fileIndex][0] == '\0')
            return (NULL);

        if (diskOpEntryPath[editor.diskop.scrollOffset + fileIndex][0] != '\0')
            return (diskOpEntryPath[editor.diskop.scrollOffset + fileIndex]);
    }

    return (NULL);
}

uint8_t dirCanBeOpened(const char *path)
{
    DIR *dir;

    dir = opendir(path);
    if (dir == NULL)
    {
        return (false);
    }
    else
    {
        closedir(dir);
        return (true);
    }
}

int8_t diskOpSetPath(const char *path, uint8_t cache)
{
    uint32_t pathLen;

    if (*path == '\0')
        return (false);

    if (dirCanBeOpened(path))
    {
        pathLen = strlen(path);
        if (pathLen < PATH_MAX_LEN)
        {
            strcpy(editor.currentPath, path);

#ifdef WIN32
            /* for Windows, turn drive: into drive:\ */
            if ((editor.currentPath[1] == ':') && (editor.currentPath[2] == '\0'))
            {
                editor.currentPath[2] = '\\';
                editor.currentPath[3] = '\0';
            }
#endif

            chdir(editor.currentPath);

            editor.diskop.scrollOffset = 0;

            if (cache)
                editor.diskop.cached = false;

            return (true);
        }
        else
        {
            displayErrorMsg("CAN'T OPEN DIR !");
        }
    }
    else
    {
        displayErrorMsg("CAN'T OPEN DIR !");
    }

    if (cache)
        editor.diskop.cached = false;

    return (false);
}

void diskOpSetInitPath(PTCONFIG *config)
{
    int8_t didSetDefPath;

    didSetDefPath = false;
    if (config->defaultDiskOpDir[0] != '\0') // if DEFAULTDIR is set or not in config
    {
        if (diskOpSetPath(config->defaultDiskOpDir, DISKOP_CACHE))
            didSetDefPath = true;
    }

    if (!didSetDefPath)
    {
        memset(editor.tempPath, 0, PATH_MAX_LEN + 1); // must be wiped in case getwd() doesn't insert null termination
        getcwd(editor.tempPath, PATH_MAX_LEN);

        diskOpSetPath(editor.tempPath, DISKOP_CACHE);
    }
}

void diskOpGoParent(void)
{
    uint32_t pathLen;

    pathLen = strlen(editor.currentPath);

#ifdef WIN32
    if ((editor.currentPath[pathLen - 1] != '\\') && (editor.currentPath[pathLen - 2] != ':'))
    {
        strcpy(editor.tempPath, editor.currentPath);
        strcat(editor.tempPath, "\\..");

        diskOpSetPath(editor.tempPath, DISKOP_CACHE);

        editor.diskop.scrollOffset = 0;
    }
#else
    if (editor.currentPath[pathLen - 1] != '/')
    {
        strcpy(editor.tempPath, editor.currentPath);
        strcat(editor.tempPath, "/..");

        diskOpSetPath(editor.tempPath, DISKOP_CACHE);

        editor.diskop.scrollOffset = 0;
    }
#endif
}

int8_t allocDiskOpVars(void)
{
    editor.filenameTmp  = (char *)(calloc(PATH_MAX_LEN + 1, 1));
    editor.entrynameTmp = (char *)(calloc(PATH_MAX_LEN + 1, 1));
    editor.currentPath  = (char *)(calloc(PATH_MAX_LEN + 1, 1));
    editor.displayPath  = (char *)(calloc(PATH_MAX_LEN + 1, 1));
    editor.tempPath     = (char *)(calloc(PATH_MAX_LEN + 1, 1));

    if (
        (editor.filenameTmp == NULL) || (editor.entrynameTmp == NULL) ||
        (editor.currentPath == NULL) || (editor.displayPath  == NULL) ||
        (editor.tempPath    == NULL)
       )
    {
        fprintf(stderr, "ERROR: Out of memory!\n");
        return (false);
    }

    return (true);
}

void deAllocDiskOpVars(void)
{
    if (editor.filenameTmp != NULL)
    {
        free(editor.filenameTmp);
        editor.filenameTmp = NULL;
    }

    if (editor.entrynameTmp != NULL)
    {
        free(editor.entrynameTmp);
        editor.entrynameTmp = NULL;
    }

    if (editor.tempPath != NULL)
    {
        free(editor.tempPath);
        editor.tempPath = NULL;
    }

    if (editor.currentPath != NULL)
    {
        free(editor.currentPath);
        editor.currentPath = NULL;
    }

    if (editor.displayPath != NULL)
    {
        free(editor.displayPath);
        editor.displayPath = NULL;
    }
}

void freeDiskOpFileMem(void)
{
    uint16_t tickCounter;

    int32_t i;

    if (firstCached)
    {
        if (!input.mouse.buttonErrorMsgActive)
            pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);

        tickCounter = 0;
        for (i = 0; i < editor.diskop.numFiles; ++i)
        {
            if (!input.mouse.buttonErrorMsgActive)
            {
                     if (tickCounter >= 1536) setStatusMessage("FREE'ING MEM..",  NO_CARRY);
                else if (tickCounter >= 1024) setStatusMessage("FREE'ING MEM.",   NO_CARRY);
                else if (tickCounter >=  512) setStatusMessage("FREE'ING MEM",    NO_CARRY);
                else if (tickCounter ==    0) setStatusMessage("FREE'ING MEM...", NO_CARRY);
            }

            tickCounter = (tickCounter + 1) & 2047;

            if (diskOpEntryPath[i] != NULL)
            {
                free(diskOpEntryPath[i]);
                diskOpEntryPath[i] = NULL;
            }

            if (diskOpEntry[i] != NULL)
            {
                free(diskOpEntry[i]);
                diskOpEntry[i] = NULL;
            }

            if (diskOpEntryType[i] == DISKOP_FILE)
            {
                if (diskOpEntryDate[i] != NULL)
                {
                    free(diskOpEntryDate[i]);
                    diskOpEntryDate[i] = NULL;
                }
            }
        }

        if (diskOpEntryPath != NULL)
        {
            free(diskOpEntryPath);
            diskOpEntryPath = NULL;
        }

        if (diskOpEntry != NULL)
        {
            free(diskOpEntry);
            diskOpEntry = NULL;
        }

        if (diskOpEntrySize != NULL)
        {
            free(diskOpEntrySize);
            diskOpEntrySize = NULL;
        }

        if (diskOpEntryDate != NULL)
        {
            free(diskOpEntryDate);
            diskOpEntryDate = NULL;
        }
    }
}

void allocDiskOpMem(void)
{
    if (!input.mouse.buttonErrorMsgActive)
    {
        pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);
        setStatusMessage("ALLOC'ING MEM...", NO_CARRY);
    }

    diskOpEntryPath =    (char **)(calloc(editor.diskop.numFiles, sizeof (char *)));
    diskOpEntry     =    (char **)(calloc(editor.diskop.numFiles, sizeof (char *)));
    diskOpEntryDate =    (char **)(calloc(editor.diskop.numFiles, sizeof (char *)));
    diskOpEntryType =    (char  *)(malloc(editor.diskop.numFiles));
    diskOpEntrySize = (uint32_t *)(malloc(editor.diskop.numFiles * sizeof (int32_t)));
}

static int8_t diskOpFillBufferMod(const char *path)
{
    char tempStr[18];
    char fileExtFirst[4];
    char fileExtLast[4];

    uint16_t tickCounter;

    int32_t fileHandle;
    uint32_t fileIndex;
    uint32_t fileNameLength;
    uint32_t currentPathLength;

    DIR *dir;

    struct dirent *ent;

    struct stat fileStat;

    editor.diskop.scrollOffset = 0;

    dir = opendir(path);
    if (dir == NULL)
        return (false);

    memset(editor.tempPath,    0, PATH_MAX_LEN + 1); // must be wiped in case getwd() doesn't insert null termination
    memset(editor.displayPath, 0, PATH_MAX_LEN + 1); // must be wiped because of display method

    getcwd(editor.tempPath, PATH_MAX_LEN);
    strcpy(editor.displayPath, editor.tempPath);
    diskOpSetPath(editor.tempPath, DISKOP_NO_CACHE);

    // evaluate number of files (used as a variable for memory allocation)

    if (!firstCached)
        firstCached = true;
    else
        freeDiskOpFileMem();

    if (!input.mouse.buttonErrorMsgActive)
        pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);

    currentPathLength      = strlen(editor.currentPath);
    tickCounter            = 0;
    editor.diskop.numFiles = 0;

    while ((ent = readdir(dir)) != NULL)
    {
        if (editor.diskop.forceStopReading)
            return (false);

        if (!input.mouse.buttonErrorMsgActive)
        {
                 if (tickCounter >= 1536) setStatusMessage("COUNTING FILES..",  NO_CARRY);
            else if (tickCounter >= 1024) setStatusMessage("COUNTING FILES.",   NO_CARRY);
            else if (tickCounter >=  512) setStatusMessage("COUNTING FILES",    NO_CARRY);
            else if (tickCounter ==    0) setStatusMessage("COUNTING FILES...", NO_CARRY);
        }

        tickCounter = (tickCounter + 1) & 2047;

        memset(fileExtFirst, 0, sizeof (fileExtFirst));
        memset(fileExtLast,  0, sizeof (fileExtLast));

        if (ent->d_type == DT_REG)
        {
            fileNameLength = strlen(ent->d_name);
            if (fileNameLength > 4)
            {
                // MOD.<filename>
                fileExtFirst[0] = (char)(toupper(ent->d_name[0]));
                fileExtFirst[1] = (char)(toupper(ent->d_name[1]));
                fileExtFirst[2] = (char)(toupper(ent->d_name[2]));
                fileExtFirst[3] = (char)(toupper(ent->d_name[3]));

                // <filename>.MOD
                fileExtLast[0] = (char)(toupper(ent->d_name[fileNameLength - 4]));
                fileExtLast[1] = (char)(toupper(ent->d_name[fileNameLength - 3]));
                fileExtLast[2] = (char)(toupper(ent->d_name[fileNameLength - 2]));
                fileExtLast[3] = (char)(toupper(ent->d_name[fileNameLength - 1]));
            }
        }

        if ((
                !strncmp(fileExtFirst, "MOD.", 4) || !strncmp(fileExtLast,     ".MOD", 4))
            || (!strncmp(fileExtFirst, "STK.", 4) || !strncmp(fileExtLast,     ".STK", 4))
            || (!strncmp(fileExtFirst, "M15.", 4) || !strncmp(fileExtLast,     ".M15", 4))
            || (!strncmp(fileExtFirst, "PP.",  3) || !strncmp(fileExtLast + 1, ".PP",  3))
            || ((ent->d_type ==        DT_DIR   ) && (strcmp(ent->d_name,      "."      ))
           ))
        {
            editor.diskop.numFiles++;
        }
    }

    closedir(dir);

    allocDiskOpMem();

    if (!input.mouse.buttonErrorMsgActive)
        pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);

    dir = opendir(path);
    if (dir == NULL)
        return (false);

    // fill disk op. buffer (path, filename, size, date, type (dir/file))

    fileIndex = 0;
    struct dirent **namelist;
    int n;

    n = scandir(".", &namelist, 0, alphasort);

    if (n < 0)
        perror("scandir error reading dir..");

    int q; // reverse the alphasort, since it doesn't have a reverse, we want A before W in filelist
    for (q=0;q<n;++q)
    {
	ent=(struct dirent *) namelist[q];

        if (editor.diskop.forceStopReading)
            return (false);

        if (!input.mouse.buttonErrorMsgActive && (editor.diskop.numFiles > 0))
        {
            sprintf(tempStr, "READING DIR %3d%%", (uint8_t)(floorf((((float)(fileIndex) / (float)(editor.diskop.numFiles)) * 100.0f) + 0.5f)));
            setStatusMessage(tempStr, NO_CARRY);
        }

        memset(fileExtFirst, 0, sizeof (fileExtFirst));
        memset(fileExtLast,  0, sizeof (fileExtLast));

        if (ent->d_type == DT_REG)
        {
            fileNameLength = strlen(ent->d_name);
            if (fileNameLength > 4)
            {
                // MOD.<filename>
                fileExtFirst[0] = (char)(toupper(ent->d_name[0]));
                fileExtFirst[1] = (char)(toupper(ent->d_name[1]));
                fileExtFirst[2] = (char)(toupper(ent->d_name[2]));
                fileExtFirst[3] = (char)(toupper(ent->d_name[3]));

                // <filename>.MOD
                fileExtLast[0] = (char)(toupper(ent->d_name[fileNameLength - 4]));
                fileExtLast[1] = (char)(toupper(ent->d_name[fileNameLength - 3]));
                fileExtLast[2] = (char)(toupper(ent->d_name[fileNameLength - 2]));
                fileExtLast[3] = (char)(toupper(ent->d_name[fileNameLength - 1]));
            }
        }

        if ((
                !strncmp(fileExtFirst, "MOD.", 4) || !strncmp(fileExtLast,     ".MOD", 4))
            || (!strncmp(fileExtFirst, "STK.", 4) || !strncmp(fileExtLast,     ".STK", 4))
            || (!strncmp(fileExtFirst, "M15.", 4) || !strncmp(fileExtLast,     ".M15", 4))
            || (!strncmp(fileExtFirst, "PP.",  3) || !strncmp(fileExtLast + 1, ".PP",  3))
            || ((ent->d_type ==        DT_DIR   ) && (strcmp(ent->d_name,      "."      ))
           ))
        {
            diskOpEntryType[fileIndex] = (ent->d_type == DT_REG) ? DISKOP_FILE : DISKOP_DIR;

            if (ent->d_name[0] == '\0')
            {
                diskOpEntry[fileIndex]        = (char *)(malloc(1)); // these 1 mallocs might seem stupid, but they're really needed
                diskOpEntryDate[fileIndex]    = (char *)(malloc(1));
                diskOpEntryPath[fileIndex]    = (char *)(malloc(1));
                diskOpEntry[fileIndex][0]     = '\0';
                diskOpEntryDate[fileIndex][0] = '\0';
                diskOpEntryPath[fileIndex][0] = '\0';
                diskOpEntrySize[fileIndex]    = 0;

                fileIndex++;

                continue;
            }

            fileNameLength             = strlen(ent->d_name);
            diskOpEntry[fileIndex]     = (char *)(malloc(fileNameLength + 1));
            diskOpEntryPath[fileIndex] = (char *)(malloc((currentPathLength + 1) + 1 + (fileNameLength + 1)));

            strcpy(diskOpEntryPath[fileIndex], editor.currentPath);
#ifdef WIN32
            strcat(diskOpEntryPath[fileIndex], "\\");
#else
            strcat(diskOpEntryPath[fileIndex], "/");
#endif
            strcat(diskOpEntryPath[fileIndex], ent->d_name);

            strcpy(diskOpEntry[fileIndex], ent->d_name);

            // get file size and modification date
            if (diskOpEntryType[fileIndex] == DISKOP_FILE)
            {
                diskOpEntryDate[fileIndex] = (char *)(malloc(7));

                fileHandle = open(diskOpEntryPath[fileIndex], O_RDONLY);
                if (fileHandle != -1)
                {
                    fstat(fileHandle, &fileStat);
                    close(fileHandle);

                    diskOpEntrySize[fileIndex] = (uint32_t)(fileStat.st_size);
                    strftime(diskOpEntryDate[fileIndex], 7, "%d%m%y", localtime(&fileStat.st_mtime));
                }
                else
                {
                    // no access to file/directory
                    diskOpEntryDate[fileIndex][0] = '\0';
                    diskOpEntryPath[fileIndex][0] = '\0';
                    diskOpEntrySize[fileIndex]    = 0;
                }
            }

            fileIndex++;
        }
    }

    closedir(dir);
    free(namelist);

    if (!input.mouse.buttonErrorMsgActive)
    {
        pointerSetPreviousMode();
        setPrevStatusMessage();
    }

    return (true);
}

static int8_t diskOpFillBufferSmp(const char *path)
{
    char tempStr[18];

    uint16_t tickCounter;

    int32_t fileHandle;
    uint32_t fileIndex;
    uint32_t fileNameLength;
    uint32_t currentPathLength;

    DIR *dir;

    struct dirent *ent;

    struct stat fileStat;

    editor.diskop.scrollOffset = 0;

    dir = opendir(path);
    if (dir == NULL)
        return (false);

    memset(editor.tempPath,    0, PATH_MAX_LEN + 1); // must be wiped in case getwd() doesn't insert null termination
    memset(editor.displayPath, 0, PATH_MAX_LEN + 1); // must be wiped because of display method

    getcwd(editor.tempPath, PATH_MAX_LEN);
    strcpy(editor.displayPath, editor.tempPath);
    diskOpSetPath(editor.tempPath, DISKOP_NO_CACHE);

    // evaluate number of files (used as a variable for memory allocation)

    if (!firstCached)
        firstCached = true;
    else
        freeDiskOpFileMem();

    pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);

    currentPathLength      = strlen(editor.currentPath);
    tickCounter            = 0;
    editor.diskop.numFiles = 0;

    while ((ent = readdir(dir)) != NULL)
    {
        if (editor.diskop.forceStopReading)
            return (false);

        if (!input.mouse.buttonErrorMsgActive)
        {
                 if (tickCounter >= 1536) setStatusMessage("COUNTING FILES..",  NO_CARRY);
            else if (tickCounter >= 1024) setStatusMessage("COUNTING FILES.",   NO_CARRY);
            else if (tickCounter >=  512) setStatusMessage("COUNTING FILES",    NO_CARRY);
            else if (tickCounter ==    0) setStatusMessage("COUNTING FILES...", NO_CARRY);
        }

        // don't count "." directory (not read lateron)
        if (!((ent->d_name[0] == '.') && (ent->d_name[1] == '\0')))
            editor.diskop.numFiles++;

        tickCounter = (tickCounter + 1) & 2047;
    }

    closedir(dir);

    allocDiskOpMem();

    pointerSetMode(POINTER_MODE_READ_DIR, NO_CARRY);

    dir = opendir(path);
    if (dir == NULL)
        return (false);

    // fill disk op. buffer (path, filename, size, date, type (dir/file))

    fileIndex = 0;
    struct dirent **namelist;
    int n;

    n = scandir(".", &namelist, 0, alphasort);

    if (n < 0)
        perror("scandir error reading dir..");

    int q; // reverse the alphasort, since it doesn't have a reverse, we want A before W in filelist
    for (q=0;q<n;++q)
    {
	ent=(struct dirent *) namelist[q];

        if (editor.diskop.forceStopReading)
            return (false);

        if (!input.mouse.buttonErrorMsgActive && (editor.diskop.numFiles > 0))
        {
            sprintf(tempStr, "READING DIR %3d%%", (uint8_t)(floorf((((float)(fileIndex) / (float)(editor.diskop.numFiles)) * 100.0f) + 0.5f)));
            setStatusMessage(tempStr, NO_CARRY);
        }

        // don't handle "." directory
        if ((ent->d_name[0] == '.') && (ent->d_name[1] == '\0'))
            continue;

        diskOpEntryType[fileIndex] = (ent->d_type == DT_REG) ? DISKOP_FILE : DISKOP_DIR;

        if (ent->d_name[0] == '\0')
        {
            diskOpEntry[fileIndex]        = (char *)(malloc(1)); // these 1 mallocs might seem stupid, but they're really needed
            diskOpEntryDate[fileIndex]    = (char *)(malloc(1));
            diskOpEntryPath[fileIndex]    = (char *)(malloc(1));
            diskOpEntry[fileIndex][0]     = '\0';
            diskOpEntryDate[fileIndex][0] = '\0';
            diskOpEntryPath[fileIndex][0] = '\0';
            diskOpEntrySize[fileIndex]    = 0;

            fileIndex++;

            continue;
        }

        fileNameLength             = strlen(ent->d_name);
        diskOpEntry[fileIndex]     = (char *)(malloc(fileNameLength + 1));
        diskOpEntryPath[fileIndex] = (char *)(malloc((currentPathLength + 1) + 1 + (fileNameLength + 1)));

        strcpy(diskOpEntryPath[fileIndex], editor.currentPath);
#ifdef WIN32
        strcat(diskOpEntryPath[fileIndex], "\\");
#else
        strcat(diskOpEntryPath[fileIndex], "/");
#endif
        strcat(diskOpEntryPath[fileIndex], ent->d_name);

        strcpy(diskOpEntry[fileIndex], ent->d_name);

        // get file size and modification date
        if (diskOpEntryType[fileIndex] == DISKOP_FILE)
        {
            diskOpEntryDate[fileIndex] = (char *)(malloc(7));

            fileHandle = open(diskOpEntryPath[fileIndex], O_RDONLY);
            if (fileHandle != -1)
            {
                fstat(fileHandle, &fileStat);
                close(fileHandle);

                diskOpEntrySize[fileIndex] = (uint32_t)(fileStat.st_size);
                strftime(diskOpEntryDate[fileIndex], 7, "%d%m%y", localtime(&fileStat.st_mtime));
            }
            else
            {
                // no access to file/directory
                diskOpEntryDate[fileIndex][0] = '\0';
                diskOpEntryPath[fileIndex][0] = '\0';
                diskOpEntrySize[fileIndex]    = 0;
            }
        }

        fileIndex++;
    }

    closedir(dir);
    free(namelist);

    pointerSetPreviousMode();
    setPrevStatusMessage();

    return (true);
}

int32_t diskOpFillThreadFunc(void *ptr)
{
    editor.diskop.isFilling = true;

         if (editor.diskop.mode == DISKOP_MODE_MOD) diskOpFillBufferMod(editor.currentPath);
    else if (editor.diskop.mode == DISKOP_MODE_SMP) diskOpFillBufferSmp(editor.currentPath);

    editor.diskop.isFilling = false;

    (void)(ptr); // make compiler happy

    return (1);
}

void diskOpDraw(uint32_t *frameBuffer)
{
    char tmpChar;
    char tmpStr[7];
    uint8_t listYPos;
    uint8_t y;

    int32_t i;
    int32_t j;
    const uint32_t *ptr32_src;
    uint32_t *ptr32_dst;
    uint32_t k;
    uint32_t entryLength;
    uint32_t fileSize;

    if (!input.mouse.buttonErrorMsgActive
        && (editor.ui.pointerMode != POINTER_MODE_READ_DIR)
        && (editor.ui.pointerMode != POINTER_MODE_MSG1)
       )
    {
        if (editor.diskop.mode == DISKOP_MODE_MOD)
            setStatusMessage("SELECT MODULE", NO_CARRY);
        else
            setStatusMessage("SELECT SAMPLE", NO_CARRY);
    }

    // print MOD pack flag
    textOut(frameBuffer, 120, 3, editor.diskop.modPackFlg ? "ON" : "OFF", palette[PAL_GENTXT]);

         if (editor.diskop.smpSaveType == DISKOP_SMP_WAV) textOut(frameBuffer, 120, 14, "WAV", palette[PAL_GENTXT]);
    else if (editor.diskop.smpSaveType == DISKOP_SMP_IFF) textOut(frameBuffer, 120, 14, "IFF", palette[PAL_GENTXT]);
    else if (editor.diskop.smpSaveType == DISKOP_SMP_RAW) textOut(frameBuffer, 120, 14, "RAW", palette[PAL_GENTXT]);

    // draw mode arrow (load mod or load sample)
    ptr32_src = arrowBitmap;
    ptr32_dst = frameBuffer + ((((11 * editor.diskop.mode) + 3) * SCREEN_W) + 148);

    y = 5;
    while (y--)
    {
        *(ptr32_dst + 0) = *(ptr32_src + 0);
        *(ptr32_dst + 1) = *(ptr32_src + 1);
        *(ptr32_dst + 2) = *(ptr32_src + 2);
        *(ptr32_dst + 3) = *(ptr32_src + 3);
        *(ptr32_dst + 4) = *(ptr32_src + 4);
        *(ptr32_dst + 5) = *(ptr32_src + 5);

        ptr32_src += 6;
        ptr32_dst += SCREEN_W;
    }

    // print disk op. path
    for (i = 0; i < 26; ++i)
    {
        tmpChar = editor.displayPath[editor.textofs.diskOpPath + i];
        if (((tmpChar < ' ') || (tmpChar > '~')) && (tmpChar != '\0'))
            tmpChar = ' '; // was illegal character

        charOut(frameBuffer, 24 + (i * FONT_CHAR_W), 25, (tmpChar == '\0') ? '_' : tmpChar, palette[PAL_GENTXT]);
    }

    if (editor.diskop.forceStopReading)
        return;

    // if needed, update the filelist and add entries
    if (!editor.diskop.cached)
    {
        editor.diskop.fillThread = SDL_CreateThread(diskOpFillThreadFunc, NULL);
        editor.diskop.cached     = true;
    }

    // print filelist
    if (!editor.diskop.isFilling && firstCached)
    {
        for (i = 0; (i < DISKOP_LIST_SIZE) && (i < editor.diskop.numFiles); ++i)
        {
            j = editor.diskop.scrollOffset + i;
            if (j < editor.diskop.numFiles)
            {
                listYPos = (uint8_t)(35 + (i * (FONT_CHAR_H + 1)));

                if (diskOpEntry[j][0] == '\0')
                {
                    if (diskOpEntryType[j] == DISKOP_FILE)
                        textOut(frameBuffer, 64, listYPos, "<COULDN'T LIST FILE>", palette[PAL_QRDSCP]);
                    else
                        textOut(frameBuffer, 64, listYPos, "<COULDN'T LIST DIR>", palette[PAL_QRDSCP]);

                    continue;
                }

                entryLength = strlen(diskOpEntry[j]);
                if (entryLength > 23)
                {
                    // shorten filename and add ".." to end
                    memcpy(editor.tempPath, diskOpEntry[j], 23);

                    editor.tempPath[21] = '.';
                    editor.tempPath[22] = '.';
                    editor.tempPath[23] = '\0';

                    for (k = 0; k < 23; ++k)
                    {
                        tmpChar = editor.tempPath[k];
                        if (((tmpChar < ' ') || (tmpChar > '~')) && (tmpChar != '\0'))
                            editor.tempPath[k] = ' '; // was illegal character
                    }

                    textOut(frameBuffer, 64, listYPos, editor.tempPath, palette[PAL_QRDSCP]);
                }
                else
                {
                    // print whole filename
                    for (k = 0; k < entryLength; ++k)
                    {
                        tmpChar = diskOpEntry[j][k];
                        if (((tmpChar < ' ') || (tmpChar > '~')) && (tmpChar != '\0'))
                            tmpChar = ' '; // was illegal character

                        charOut(frameBuffer, 64 + (k * FONT_CHAR_W), listYPos, tmpChar, palette[PAL_QRDSCP]);
                    }
                }

                if (diskOpEntryType[j] == DISKOP_FILE)
                {
                    // print modification date
                    if (diskOpEntryDate[j][0] != '\0')
                        textOut(frameBuffer, 8, listYPos, diskOpEntryDate[j], palette[PAL_QRDSCP]);
                    else
                        textOut(frameBuffer, 8, listYPos, "000000", palette[PAL_QRDSCP]);

                    // print filesize (can be optimized/cleansed further...)
                    fileSize = diskOpEntrySize[j];
                    if (fileSize == 0)
                    {
                        textOut(frameBuffer, 256, listYPos, "     0", palette[PAL_QRDSCP]);
                    }
                    else if (fileSize > 999999)
                    {
                        if (fileSize > 9999999)
                        {
                            if (fileSize >= 0x80000000UL)
                            {
                                textOut(frameBuffer, 256, listYPos, "  >2GB", palette[PAL_QRDSCP]);
                            }
                            else
                            {
                                fileSize /= 1000000;

                                tmpStr[3] = '0' + (fileSize % 10); fileSize /= 10;
                                tmpStr[2] = '0' + (fileSize % 10); fileSize /= 10;
                                tmpStr[1] = '0' + (fileSize % 10);
                                tmpStr[0] = '0' + (char)(fileSize / 10);
                                tmpStr[4] = 'M';
                                tmpStr[5] = 'B';
                                tmpStr[6] = '\0';

                                k = 0;
                                while (tmpStr[k]  == '0')
                                       tmpStr[k++] = ' ';

                                textOut(frameBuffer, 256, listYPos, tmpStr, palette[PAL_QRDSCP]);
                            }
                        }
                        else
                        {
                            fileSize /= 1000;

                            tmpStr[3] = '0' + (fileSize % 10); fileSize /= 10;
                            tmpStr[2] = '0' + (fileSize % 10); fileSize /= 10;
                            tmpStr[1] = '0' + (fileSize % 10);
                            tmpStr[0] = '0' + (char)(fileSize / 10);
                            tmpStr[4] = 'K';
                            tmpStr[5] = 'B';
                            tmpStr[6] = '\0';
                        }

                        textOut(frameBuffer, 256, listYPos, tmpStr, palette[PAL_QRDSCP]);
                    }
                    else
                    {
                        tmpStr[5] = '0' + (fileSize % 10); fileSize /= 10;
                        tmpStr[4] = '0' + (fileSize % 10); fileSize /= 10;
                        tmpStr[3] = '0' + (fileSize % 10); fileSize /= 10;
                        tmpStr[2] = '0' + (fileSize % 10); fileSize /= 10;
                        tmpStr[1] = '0' + (fileSize % 10);
                        tmpStr[0] = '0' + (char)(fileSize / 10);
                        tmpStr[6] = '\0';

                        k = 0;
                        while (tmpStr[k]  == '0')
                               tmpStr[k++] = ' ';

                        textOut(frameBuffer, 256, listYPos, tmpStr, palette[PAL_QRDSCP]);
                    }
                }
            }
        }
    }
}
