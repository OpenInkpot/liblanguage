#ifndef LANGUAGE_CONTROL_H
#define LANGUAGE_CONTROL_H

#include <stdbool.h>

typedef struct
{
    const char* internal_name; /* E.g. ru_RU */
    const char* locale; /* E.g. ru_RU.UTF-8 */
    const char* name; /* E.g. Russian */
    const char* native_name; /* E.g. Русский, may be NULL */
} language_t;

typedef struct
{
    language_t* langs;
    int n;
} languages_t;

/* Returns languages_t or NULL if not succesful */
languages_t* get_supported_languages();

void free_langs(languages_t* langs);

/*
 * Returns current language, malloc'ed. Returned language may be not amongst
 * values returned by get_supported_language.
 */
char* get_current_language();

int set_language(const char* internal_name, bool restart_session);

#endif
