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

#define max(a, b) ((a) > (b) ? (a) : (b))

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

int font_draw_glyph_L(const struct font *font,
		      int x, int y, int width, int height,
		      uint8_t *buf, glyph_t glyph, glyph_t prev)
{
	if (font == NULL) return -1;
	const struct glyph *g = font_get_glyph(font, glyph);
	if (g == NULL) return -2;

	int kerning_offset = font_get_kerning(font, prev, glyph);

	for (int row = 0; row < g->rows; row++) {
		int yofs = row + y + (font->ascender - g->top);

		if (yofs < 0) continue;
		if (yofs >= height) break;

		for (int col = 0; col < g->cols; col++) {
			int xofs = col + x + g->left + kerning_offset;

			if (xofs < 0) continue;
			if (xofs >= width) break;

			uint8_t val = g->bitmap[(row * g->cols) + col];
			uint8_t *pixel = buf + (yofs * width) + xofs;

			if (val < 64) *pixel = ' ';
			else if (val < 128) *pixel = '.';
			else if (val < 192) *pixel = 'x';
			else *pixel = 'X';
		}
	}

	return g->advance + kerning_offset;
}
