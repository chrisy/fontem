/** Font structure definitions.
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _FONTEM_H
#define _FONTEM_H

#include <stdint.h>
#include <stdio.h>

typedef uint16_t glyph_t;

/** Description of a glyph; a single character in a font. */
struct glyph {
	glyph_t			glyph;          /** The glyph this entry refers to */

	int16_t			left;           /** Offset of the left edge of the glyph */
	int16_t			top;            /** Offset of the top edge of the glyph */
	int16_t			advance;        /** Horizonal offset when advancing to the next glyph */

	uint16_t		cols;           /** Width of the bitmap */
	uint16_t		rows;           /** Height of the bitmap */
	const uint8_t		*bitmap;        /** Bitmap data */

	const struct kerning	*kerning;       /** Font kerning data */
};

/** Kerning table; for a pair of glyphs, provides the horizontal adjustment. */
struct kerning {
	glyph_t left;   /** The left-glyph */
	int16_t offset; /** The kerning offset for this glyph pair */
};

/** Description of a font. */
struct font {
	char			*name;          /** Name of the font */
	char			*style;         /** Style of the font */

	uint16_t		size;           /** Point size of the font */
	uint16_t		dpi;            /** Resolution of the font */

	int16_t			ascender;       /** Ascender height */
	int16_t			descender;      /** Descender height */
	int16_t			height;         /** Baseline-to-baseline height */

	uint16_t		count;          /** Number of glyphs */
	uint16_t		max;            /** Maximum glyph index */
	const struct glyph	**glyphs;       /** Font glyphs */
};


const struct glyph *font_get_glyph(const struct font *font, glyph_t glyph);
int16_t font_get_kerning(const struct font *font, glyph_t left, glyph_t right);
int font_calculate_box(const struct font *font, int *maxx, int *maxy, const char *str);
int font_draw_glyph_L(const struct font *font, int x, int y, int width, int height, uint8_t *buf, glyph_t glyph, glyph_t prev);
int font_draw_glyph_RGB(const struct font *font, int x, int y, int width, int height, uint8_t *buf, glyph_t glyph, glyph_t prev, uint32_t rgb);
const struct font *font_find(char *name, char *style, int size);
void font_print_all(FILE *out);

#endif /* _FONTEM_H */
