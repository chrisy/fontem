/**
 * \file src/fontrender.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "fontem.h"

const struct glyph *font_get_glyph(const struct font *font, glyph_t glyph)
{
	if (glyph > font->max) return NULL;
	return font->glyphs[glyph];
}

int16_t font_get_kerning(const struct font *font, glyph_t left, glyph_t right)
{
	if (font == NULL || left == 0 || right == 0) return 0;
	const struct glyph *g = font_get_glyph(font, right);
	if (g == NULL || g->kerning == NULL) return 0;

	const struct kerning *k;
	for (k = g->kerning; k->left != 0; k++)
		if (k->left == left) return k->offset;

	return 0;
}

int font_calculate_box(const struct font *font,
		       int *maxx, int *maxy,
		       const char *str)
{
	if (font == NULL) return 0;
	if (str == NULL) return 0;

	int x = 0;
	int y = font->height;
	int count = 0;
	glyph_t prev = 0;
	for (const char *p = str; *p; p++, count++) {
		const struct glyph *g = font_get_glyph(font, *p);
		if (g == NULL) continue;
		x += g->advance + font_get_kerning(font, prev, *p);
		prev = *p;
	}

	*maxx = x;
	*maxy = y;

	return count;
}
