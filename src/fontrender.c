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

const struct glyph *font_get_glyph(const struct font *font, int glyph)
{
	if (glyph > font->max) return NULL;
	return font->glyphs[glyph];
}

int font_draw_glyph_L(const struct font *font,
		      int x, int y, int width, int height,
		      uint8_t *buf, int glyph)
{
	if (font == NULL) return -1;
	const struct glyph *g = font_get_glyph(font, glyph);
	if (g == NULL) return -2;

	for (int row = 0; row < g->rows; row++) {
		int yofs = row + y + (font->ascender - g->top);

		if (yofs < 0) continue;
		if (yofs >= height) break;

		for (int col = 0; col < g->cols; col++) {
			int xofs = col + x + g->left;

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

	return g->advance;
}
