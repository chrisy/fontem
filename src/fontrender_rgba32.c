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

int font_draw_glyph_RGBA32(const struct font *font,
			   int x, int y, int width, int height,
			   uint8_t *buf, glyph_t glyph, glyph_t prev,
			   uint32_t rgb)
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
			uint8_t *pixel = buf + (yofs * width * 3) + (xofs * 3);

			*pixel = alpha_blend(*pixel, 0, rgba32_get_r(rgb), val);
			pixel++;
			*pixel = alpha_blend(*pixel, 0, rgba32_get_g(rgb), val);
			pixel++;
			*pixel = alpha_blend(*pixel, 0, rgba32_get_b(rgb), val);
		}
	}

	return g->advance + kerning_offset;
}
