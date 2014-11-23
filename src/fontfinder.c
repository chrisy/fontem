/**
 * \file src/fontfinder.c
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

#ifndef EOL
#define EOL "\n"
#endif

const struct font *font_find(const char *name, const char *style, const int size)
{
	for (int idx = 0; fonts[idx] != NULL; idx++) {
		if (size == fonts[idx]->size && !strcasecmp(name, fonts[idx]->name))
			if (style == NULL || !strcasecmp(style, fonts[idx]->style))
				return fonts[idx];
	}
	return NULL;
}

void font_print_all(FILE *out)
{
	fprintf(out, "%-20s %-8s %-4s" EOL, "Font name", "Style", "Size");
	for (int idx = 0; fonts[idx] != NULL; idx++) {
		fprintf(out, "%-20s %-8s %4d\n",
			fonts[idx]->name,
			fonts[idx]->style,
			fonts[idx]->size);
	}
}
