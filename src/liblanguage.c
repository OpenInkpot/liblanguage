#include "liblanguage.h"

#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <locale.h>
#include <stdlib.h>
#include "liblops.h"

#define LOCALE_DIR "/usr/share/i18n/languages"

#define MAXPATH 256
#define BUFSIZE 256

static int fill_lang_info(language_t* lang, const char* buf)
{
    char* locsep = strchr(buf, '|');
    char* eolsep = strchr(locsep ? locsep : buf, '\n');

    if(eolsep)
        *eolsep = '\0';

    if(locsep)
    {
        *locsep = '\0';
        if(!(lang->name = strdup(locsep+1)))
            return -1;
        if(!(lang->native_name = strdup(buf)))
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

static int try_add_language(languages_t* langs, const char* base_dir,
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
    if(-1 == readn(fd, buf, BUFSIZE))
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

static int cmp_langs(const void* lhs_, const void* rhs_)
{
    const language_t* lhs = (const language_t*)lhs_;
    const language_t* rhs = (const language_t*)rhs_;

    return strcmp(lhs->internal_name, rhs->internal_name);
}


languages_t* languages_get_supported()
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
    if(d)
    {
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
    }

    char* current = setlocale(LC_MESSAGES, "");
    langs->current = strdup(current ? current : "");
    if(strchr(langs->current, '.'))
        *strchr(langs->current, '.') = '\0';

    if(langs->n == 0)
    {
        language_t* l = realloc(langs->langs, (langs->n+1) * sizeof(language_t));
        if(!l)
            goto err;

        int nlang = langs->n;
        langs->langs = l;
        langs->n++;

        if(!(langs->langs[nlang].locale = strdup("C")))
            goto err;
        if(!(langs->langs[nlang].internal_name = strdup("C")))
            goto err;
        if(!(langs->langs[nlang].name = strdup("English")))
            goto err;
        langs->langs[nlang].native_name = NULL;
    }

    qsort(langs->langs, langs->n, sizeof(language_t), &cmp_langs);

    return langs;

err:
    if(d)
        closedir(d);

    languages_free(langs);
    return NULL;
}

void languages_free(languages_t* langs)
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

    free(langs->current);
    free(langs);
}

static int write_lang_file(const char* locale)
{
    char* home = getenv("HOME");
    if(!home)
    {
        errno = ENOENT;
        return -1;
    }

    char lang_file[512];
    snprintf(lang_file, 512, "%s/.locale", home);

    int fd = open(lang_file, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR);
    if(fd == -1)
        return -1;

    const char* envs[] = {
        "LANG",
        "LC_CTYPE",
        "LC_NUMERIC",
        "LC_TIME",
        "LC_COLLATE",
        "LC_MESSAGES",
        "LC_PAPER",
        "LC_NAME",
        "LC_ADDRESS",
        "LC_TELEPHONE",
        "LC_MEASUREMENT"
    };

    int i;
    for(i = 0; i < sizeof(envs)/sizeof(envs[0]); ++i)
    {
        char content[256];
        snprintf(content, 256, "%s=%s; export %s\n", envs[i], locale, envs[i]);
        writen(fd, content, strlen(content));
    }

    close(fd);
    return 0;
}

int languages_set(languages_t* langs, const char* internal_name)
{
    int i;
    for(i = 0; i < langs->n; ++i)
        if(!strcmp(langs->langs[i].internal_name, internal_name))
            return write_lang_file(langs->langs[i].locale);
    return write_lang_file(internal_name);
}
