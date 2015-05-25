/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** This file is NOT considered 'public domain', please read licenses.txt.
** The "dirent for MSVC" project is Copyright (C) 2006-2012 Toni Ronkko
** (code taken from dirent-1.10.zip->dirent.h and modified)
**
*/

#ifndef __PT_DIRENT_H
#define __PT_DIRENT_H

#include <errno.h>
#include <sys/stat.h>
#include <limits.h>

#ifdef WIN32
#include <windows.h>
#include <stdint.h>

#ifndef DT_REG
#define DT_REG S_IFREG
#endif

#ifndef DT_DIR
#define DT_DIR S_IFDIR
#endif

#ifndef DT_CHR
#define DT_CHR S_IFCHR
#endif

typedef struct dirent
{
    char d_name[MAX_PATH + 1];
    int32_t d_type;
    uint32_t d_namlen;
} dirent;

typedef struct DIR
{
    char patt[MAX_PATH + 3];
    int32_t cached;
    dirent curentry;
    WIN32_FIND_DATAA find_data;
    HANDLE search_handle;
} DIR;

DIR *opendir(const char *dirname);
struct dirent *readdir(DIR *dirp);
int8_t closedir(DIR *dirp);
void rewinddir(DIR *dirp);
#else
#include <dirent.h>
#endif

#endif
