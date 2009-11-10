#ifndef LANGUAGE_CONTROL_H
#define LANGUAGE_CONTROL_H

#include <stdbool.h>

typedef struct {
    const char *internal_name; /* E.g. ru_RU */
    const char *locale; /* E.g. ru_RU.UTF-8 */
    const char *name; /* E.g. Russian */
    const char *native_name; /* E.g. Русский, may be NULL */
} language_t;

typedef struct {
    language_t *langs;
    int n;

    /* may be not amongst langs */
    char *current;
} languages_t;

/* Returns languages_t or NULL if not succesful */
languages_t *
languages_get_supported();

void
languages_free(languages_t *langs);

int
languages_set(languages_t *langs, const char *internal_name);

#endif
