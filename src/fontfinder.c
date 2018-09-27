/**
 * \file fontfinder.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "fontem.h"
#include "font_all.h"

/** Find a font based on name, style size and whether it is compressed. */
const struct font *font_find_all(const char *name, const char *style, const int size,
				 const char rle)
{
	for (int idx = 0; fonts[idx] != NULL; idx++) {
		if (size == fonts[idx]->size && !strcasecmp(name, fonts[idx]->name))
			if (style == NULL || !strcasecmp(style, fonts[idx]->style))
				if (rle < 0 || rle == fonts[idx]->compressed)
					return fonts[idx];
	}
	return NULL;
}

/** Find a font based on name, style and size. */
const struct font *font_find(const char *name, const char *style, const int size)
{
	return font_find_all(name, style, size, -1);
}

/** Iterate all the fonts we know about. */
void font_iterate_all(fia_fn_t fn, void *opaque)
{
	for (int idx = 0; fonts[idx] != NULL; idx++)
		fn((struct font *)fonts[idx], opaque);
}
