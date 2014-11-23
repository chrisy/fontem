/**
 * \file src/fontrender_rgv32a.c
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

int font_draw_glyph_RGB16(const struct font *font,
			  int x, int y, int width, int height,
			  uint8_t *buf, const struct glyph *g,
			  uint16_t rgb)
{
	for (int row = 0; row < g->rows; row++) {
		int yofs = row + y + (font->ascender - g->top);

		if (yofs < 0) continue;
		if (yofs >= height) break;

		for (int col = 0; col < g->cols; col++) {
			int xofs = col + x + g->left;

			if (xofs < 0) continue;
			if (xofs >= width) break;

			uint16_t val = g->bitmap[(row * g->cols) + col];
			uint16_t *pixel = (uint16_t *)(buf + (yofs * width * 2) + (xofs * 2));

			uint16_t r = alpha_blend(rgb16_get_r(*pixel), 0, rgb16_get_r(rgb), val);
			uint16_t g = alpha_blend(rgb16_get_g(*pixel), 0, rgb16_get_g(rgb), val);
			uint16_t b = alpha_blend(rgb16_get_b(*pixel), 0, rgb16_get_b(rgb), val);

			*pixel = rgb16_combine(r, g, b);
		}
	}

	return g->advance;
}

int font_draw_char_RGB16(const struct font *font,
			 int x, int y, int width, int height,
			 uint8_t *buf, glyph_t glyph, glyph_t prev,
			 uint16_t rgb)
{
	if (font == NULL) return -1;
	const struct glyph *g = font_get_glyph(font, glyph);
	if (g == NULL) return -2;

	int kerning_offset = font_get_kerning(font, prev, glyph);

	return font_draw_glyph_RGB16(font, x + kerning_offset, y, width, height,
				     buf, g, rgb) + kerning_offset;
}

char *font_draw_string_RGB16(const struct font *font,
			     int x, int y, int width, int height,
			     uint8_t *buf, char *str, char prev,
			     uint16_t rgb)
{
	if (font == NULL) return NULL;

	while (*str) {
		const struct glyph *g = font_get_glyph(font, *str);
		if (g == NULL) continue;

		int kerning_offset = font_get_kerning(font, prev, *str);

		if (g->advance + kerning_offset > width)
			break;  // todo - linewrap?

		x += font_draw_glyph_RGB16(font, x + kerning_offset, y, width, height,
					   buf, g, rgb) + kerning_offset;

		str++;
	}

	return str;
}
