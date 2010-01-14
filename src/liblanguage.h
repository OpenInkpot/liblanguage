/*
 * Copyright © 2009 Mikhail Gusarov <dottedmag@dottedmag.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

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
