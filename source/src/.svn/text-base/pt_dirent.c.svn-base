/*
** This file is part of the ProTracker v2.3D clone
** project by Olav "8bitbubsy" Sorensen.
**
** This file is NOT considered 'public domain', please read licenses.txt.
** The "dirent for MSVC" project is Copyright (C) 2006-2012 Toni Ronkko
** (code taken from dirent-1.10.zip->dirent.h and modified)
**
*/

#ifdef WIN32 // this file is for WIN32 port only

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "pt_dirent.h"
#include "pt_helpers.h" // true,false

#if defined (_MSC_VER) && (_MSC_VER >= 1400)
#define DIRENT_STRNCPY(dest,src,size) strncpy_s((dest), (size), (src), _TRUNCATE)
#else
#define DIRENT_STRNCPY(dest,src,size) strncpy((dest), (src), (size))
#endif

#if defined (_MSC_VER)
#define DIRENT_SET_ERRNO(x) _set_errno(x)
#else
#define DIRENT_SET_ERRNO(x) (errno = (x))
#endif

DIR *opendir(const char *dirname)
{
    char *p;
    int8_t error;

    DIR *dirp;

    if ((dirname == NULL) || (dirname[0] == '\0'))
    {
        DIRENT_SET_ERRNO(ENOENT);
        return (NULL);
    }

    if ((strlen(dirname) + 3) >= MAX_PATH)
    {
        DIRENT_SET_ERRNO(ENAMETOOLONG);
        return (NULL);
    }

    dirp = (DIR *)(malloc(sizeof (struct DIR)));
    if (dirp != NULL)
    {
        if (GetFullPathNameA(dirname, MAX_PATH, dirp->patt, NULL))
        {
            p = strchr(dirp->patt, '\0');
            if ((dirp->patt < p)  &&  (*(p - 1) != '\\')  &&  (*(p - 1) != ':'))
                *p++ = '\\';

            *p++ = '*';
            *p   = '\0';

            dirp->search_handle = FindFirstFileA(dirp->patt, &dirp->find_data);
            if (dirp->search_handle != INVALID_HANDLE_VALUE)
            {
                dirp->cached = true;
                error        = false;
            }
            else
            {
                DIRENT_SET_ERRNO(ENOENT);
                error = true;
            }
        }
        else
        {
            DIRENT_SET_ERRNO(ENOMEM);
            error = true;
        }

        if (error)
        {
            free(dirp);
            dirp = NULL;
        }
    }

    return (dirp);
}

struct dirent *readdir(DIR *dirp)
{
    uint32_t attr;

    if (dirp == NULL)
    {
        DIRENT_SET_ERRNO(EBADF);
        return (NULL);
    }

    if (dirp->cached)
    {
        dirp->cached = false;
    }
    else
    {
        if (dirp->search_handle == INVALID_HANDLE_VALUE)
            return (NULL);

        if (FindNextFileA(dirp->search_handle, &dirp->find_data) == FALSE)
        {
            FindClose(dirp->search_handle);
            dirp->search_handle = INVALID_HANDLE_VALUE;

            return (NULL);
        }
    }

    DIRENT_STRNCPY(dirp->curentry.d_name, dirp->find_data.cFileName, sizeof (dirp->curentry.d_name));

    dirp->curentry.d_name[MAX_PATH] = '\0';
    dirp->curentry.d_namlen         = strlen(dirp->curentry.d_name);

    attr = dirp->find_data.dwFileAttributes;

         if ((attr & FILE_ATTRIBUTE_DEVICE)    != 0) dirp->curentry.d_type = DT_CHR;
    else if ((attr & FILE_ATTRIBUTE_DIRECTORY) != 0) dirp->curentry.d_type = DT_DIR;
    else                                             dirp->curentry.d_type = DT_REG;

    return (&dirp->curentry);
}

int8_t closedir(DIR *dirp)
{
    if (dirp == NULL)
    {
        DIRENT_SET_ERRNO(EBADF);
        return (-1);
    }

    if (dirp->search_handle != INVALID_HANDLE_VALUE)
    {
        FindClose(dirp->search_handle);
        dirp->search_handle = INVALID_HANDLE_VALUE;
    }

    free(dirp);
    dirp = NULL;

    return (0);
}

void rewinddir(DIR* dirp)
{
    if (dirp != NULL)
    {
        if (dirp->search_handle != INVALID_HANDLE_VALUE)
            FindClose(dirp->search_handle);

        dirp->search_handle = FindFirstFileA(dirp->patt, &dirp->find_data);
        if (dirp->search_handle != INVALID_HANDLE_VALUE)
            dirp->cached = true;
        else
            dirp->cached = false;
    }
}

#endif
