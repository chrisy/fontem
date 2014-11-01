/**
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#ifndef _FONTEM_H
#define _FONTEM_H

struct glyph {
	int		left;           //* Offset of the left edge of the glyph */
	int		top;            //* Offset of the top edge of the glyph */

	int		cols;           //* Width of the bitmap */
	int		rows;           //* Height of the bitmap */
	unsigned char	*bitmap;        //* Bitmap data */
};

struct font {
	char		*name;          //* Name of the font */
	int		size;           //* Point size of the font */
	int		dpi;            //* Resolution of the font */
	int		count;          //* Number of glyphs */
	struct glyph	**glyphs;       /** Font glyphs */
};

#endif /* _FONTEM_H */
