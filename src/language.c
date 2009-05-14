#include "language.h"

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <locale.h>
#include <stdlib.h>
#include "lops.h"

/* FIXME: ? */
#define LOCALE_DIR "/usr/share/i18n/languages"

#define MAXPATH 256
#define BUFSIZE 256

int fill_lang_info(language_t* lang, const char* buf)
{
    char* locsep = strchr(buf, '|');
    char* eolsep = strchr(locsep ? locsep : buf, '\n');

    if(eolsep)
        *eolsep = '\0';

    if(locsep)
    {
        *locsep = '\0';
        if(!(lang->name = strdup(buf)))
            return -1;
        if(!(lang->native_name = strdup(locsep+1)))
            return -1;
    }
    else
    {
        if(!(lang->name = strdup(buf)))
            return -1;
        lang->native_name = NULL;
    }

    return 0;
}

int try_add_language(languages_t* langs, const char* base_dir,
                     const char* filename)
{
    if(!strcmp(filename, "."))
        return 0;
    if(!strcmp(filename, ".."))
        return 0;

    char path[MAXPATH];
    if(MAXPATH == snprintf(path, MAXPATH, "%s/%s", base_dir, filename))
    {
        errno = ENAMETOOLONG;
        return -1;
    }

    int fd = open(path, O_RDONLY);
    if(fd == -1)
        return -1;

    char buf[BUFSIZE+1] = ""; /* +1 for zero terminator */
    if(-1 == lread(fd, buf, BUFSIZE))
    {
        close(fd);
        return -1;
    }

    if(-1 == close(fd))
        return -1;

    int nlang = langs->n;

    language_t* l = realloc(langs->langs, (langs->n+1) * sizeof(language_t));
    if(!l)
        return -1;
    langs->langs = l;
    langs->n++;

    if(!(langs->langs[nlang].locale = strdup(filename)))
       return -1;
    if(!(langs->langs[nlang].internal_name = strdup(filename)))
        return -1;

    char* dotpos = strchr(langs->langs[nlang].internal_name, '.');
    if(dotpos)
        *dotpos = '\0';

    fill_lang_info(langs->langs+nlang, buf);

    return 0;
}

languages_t* get_supported_languages()
{
    DIR* d = NULL;
    languages_t* langs = malloc(sizeof(languages_t));

    if(!langs)
        goto err;

    langs->n = 0;
    langs->langs = malloc(0);
    if(!langs->langs)
        goto err;

    d = opendir(LOCALE_DIR);
    if(!d)
    {
        /* FIXME: warn */
        return langs;
    }

    for(;;)
    {
        errno = 0;
        struct dirent* entry = readdir(d);

        if(!entry)
        {
            if(!errno) break;

            /* FIXME: warn */
            continue;
        }

        if(-1 == try_add_language(langs, LOCALE_DIR, entry->d_name))
        {
            /* FIXME: warn and continue */
        }
    }

    if(-1 == closedir(d))
    {
        d = NULL;
        goto err;
    }

    return langs;

err:
    if(d)
        closedir(d);

    free_langs(langs);
    return NULL;
}

void free_langs(languages_t* langs)
{
    if(!langs)
        return;

    int i;
    for(i = 0; i < langs->n; ++i)
    {
        free((char*)langs->langs[i].internal_name);
        free((char*)langs->langs[i].locale);
        free((char*)langs->langs[i].name);
        free((char*)langs->langs[i].native_name);
    }

    free(langs);
}

char* get_current_language()
{
    char* loc = setlocale(LC_MESSAGES, NULL);
    loc = strdup(loc ? loc : "");
    if(strchr(loc, '.'))
        *strchr(loc, '.') = '\0';
    return loc;
}

int set_language(const char* internal_name, bool restart_session)
{
    /* Update user's locale */
    /* Kill X session */

    errno = ENOSYS;
    return -1;
}
