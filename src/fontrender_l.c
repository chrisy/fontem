/**
 * \file fontrender_l.c
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

int font_draw_glyph_L(const struct font *font,
		      int x, int y, int width, int height,
		      uint8_t *buf, const struct glyph *glyph)
{
	unsigned int rows = glyph->rows, cols = glyph->cols;
	const unsigned char *data = glyph->bitmap;
	unsigned char count = 0, class = 0;

	for (unsigned int row = 0; row < rows; row++) {
		int yofs = row + y + (font->ascender - glyph->top);

		for (unsigned int col = 0; col < cols; col++) {
			int xofs = col + x + glyph->left;

			uint8_t val;
			if (font->compressed) {
				if (count == 0) {
					count = (*data & 0x3f) + 1;
					class = *data >> 6;
					data++;
				}

				if (class == 0)
					val = *(data++);
				else if (class == 3)
					val = 0xff;
				else
					val = 0;
				count--;
			} else {
				val = data[(row * cols) + col];
			}

			if ((yofs >= 0) && (yofs < height) && (xofs >= 0) && (xofs < width)) {
				uint8_t *pixel = buf + (yofs * width) + xofs;

				if (val < 64) *pixel = ' ';
				else if (val < 128) *pixel = '.';
				else if (val < 192) *pixel = '+';
				else *pixel = 'X';
			}
		}
	}

	return glyph->advance;
}

int font_draw_char_L(const struct font *font,
		     int x, int y, int width, int height,
		     uint8_t *buf, glyph_t glyph, glyph_t prev)
{
	if (font == NULL) return -1;
	const struct glyph *g = font_get_glyph(font, glyph);
	if (g == NULL) return -2;

	int kerning_offset = font_get_kerning(font, prev, glyph);

	return font_draw_glyph_L(font, x + kerning_offset, y, width, height,
				 buf, g) + kerning_offset;
}
