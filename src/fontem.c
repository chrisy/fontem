/**
 * \file src/fontem.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <config.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <popt.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define DEFAULT_CHAR_LIST "!@#$%^&*()_+-={}|[]\\:\";'<>?,./`~" \
	" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"	\
	"0123456789"

/** Font library handle */
FT_Library library;

void store_glyph(FT_GlyphSlotRec *glyph, int ch, int size, char *name, FILE *c);

int main(int argc, const char *argv[])
{
	int error;

	char *font_filename = NULL;
	char *char_list = DEFAULT_CHAR_LIST;
	char *output_name = NULL;
	int font_size = 10;

	struct poptOption opts[] = {
		{ "font",  'f', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_filename, 1, "Font filename",		   "file"    },
		{ "size",  's', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,    &font_size,     1, "Font size",			   "integer" },
		{ "chars", 'c', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &char_list,     1, "List of characters to produce",   "string"  },
		{ "name",  'n', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &output_name,   1, "Output name (without extension)", "file"    },
		POPT_AUTOHELP
		POPT_TABLEEND
	};

	poptContext ctx = poptGetContext(NULL, argc, argv, opts, 0);
	int rc;

	while ((rc = poptGetNextOpt(ctx)) > 0) {
		switch (rc) {
		}
	}

	poptFreeContext(ctx);

	if (font_filename == NULL) {
		fprintf(stderr, "ERROR: You must specify a font filename.\n");
		return 1;
	}

	if (output_name == NULL) {
		fprintf(stderr, "ERROR: You must specify an output name.\n");
		return 1;
	}

	char *c_name = malloc(strlen(output_name) + 3);
	strcpy(c_name, output_name);
	strcat(c_name, ".c");
	FILE *c = fopen(c_name, "w");

	char *h_name = malloc(strlen(output_name) + 3);
	strcpy(h_name, output_name);
	strcat(h_name, ".h");
	FILE *h = fopen(h_name, "w");

	// Initial output in the .c file
	fprintf(c, "%s",
		"/**\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(c, "#include \"fontem.h\"\n");
	fprintf(c, "#include \"%s\"\n\n", h_name);

	// Initial output in the .h file
	fprintf(h, "%s",
		"/**\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(h, "#ifndef _FONTEM_%s_H\n#define _FONTEM_%s_H\n\n", output_name, output_name);
	fprintf(h, "#include \"fontem.h\"\n");

	// Init the font library
	error = FT_Init_FreeType(&library);
	if (error) {
		fprintf(stderr, "ERROR: Can't initialize FreeType.\n");
		return 1;
	}

	// Load the font
	FT_Face face;
	error = FT_New_Face(library, font_filename, 0, &face);
	if (error) {
		fprintf(stderr, "ERROR: Can't load '%s'.\n", font_filename);
		return 1;
	}

	// Set the size
	error = FT_Set_Char_Size(face, font_size * 64, 0, 72, 0);
	if (error) {
		fprintf(stderr, "ERROR: Can't set the font size to %d", font_size);
		return 1;
	}

	int len = strlen(char_list);
	for (int i = 0; i < len; i++) {
		int ch = char_list[i];

		// Load the glyph
		error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
		if (error) {
			fprintf(stderr, "ERROR : Can 't load glyph for %c.\n", ch);
			return 1;
		}

		store_glyph(face->glyph, ch, font_size, output_name, c);
	}

	fprintf(h, "#endif /* _FONTEM_%s_H */\n", output_name);
	fclose(h);
	fclose(c);

	return 0;
}

void store_glyph(FT_GlyphSlotRec *glyph, int ch, int size, char *name, FILE *c)
{
	FT_Bitmap *bitmap = &glyph->bitmap;

	// Work out a name for this glyph
	int len = strlen(name) + 16;
	char *gname = malloc(len);

	snprintf(gname, len, "font_%s_%d_%04x", name, size, ch);

	fprintf(c, "static struct glyph %s = {\n", gname);
	fprintf(c, "\t.left = %d,\n", glyph->bitmap_left);
	fprintf(c, "\t.top = %d,\n", glyph->bitmap_top);
	fprintf(c, "\t.cols = %d,\n", bitmap->width);
	fprintf(c, "\t.rows = %d,\n", bitmap->rows);
	fprintf(c, "\t.bitmap = {\n");
	for (int y = 0; y < bitmap->rows; y++) {
		fprintf(c, "\t\t");
		for (int x = 0; x < bitmap->width; x++)
			fprintf(c, "0x%02x, ", (unsigned char)bitmap->buffer[y * bitmap->width + x]);
		fprintf(c, "\n");
	}
	fprintf(c, "\t},\n");
	fprintf(c, "};\n\n");
}

// vim: set softtabstop=8 shiftwidth=8 tabstop=8:
