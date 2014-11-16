/** Font structure definitions.
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _FONTEM_H
#define _FONTEM_H

#include <stdint.h>

/** Description of a glyph; a single character in a font. */
struct glyph {
	int	left;           /** Offset of the left edge of the glyph */
	int	top;            /** Offset of the top edge of the glyph */
	int	advance;        /** Horizonal offset when advancing to the next glyph */

	int	cols;           /** Width of the bitmap */
	int	rows;           /** Height of the bitmap */
	uint8_t *bitmap;        /** Bitmap data */
};

/** Description of a font. */
struct font {
	char		*name;          /** Name of the font */
	char		*style;         /** Style of the font */
	int		size;           /** Point size of the font */
	int		dpi;            /** Resolution of the font */
	int		count;          /** Number of glyphs */
	int		max;            /** Maximum glyph index */
	int		ascender;       /** Ascender height */
	int		descender;      /** Descender height */
	struct glyph	**glyphs;       /** Font glyphs */
};

struct glyph *font_get_glyph(struct font *font, int glyph);
int font_draw_glyph_L(struct font *font, int x, int y, int width, int height, uint8_t *buf, int glyph);

#endif /* _FONTEM_H */
