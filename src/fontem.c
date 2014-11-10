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
#include <errno.h>

#include <popt.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define DEFAULT_CHAR_LIST "!@#$%^&*()_+-={}|[]\\:\";'<>?,./`~" \
	" ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"	\
	"0123456789"

#define FONT_DPI 72

/** Font library handle */
FT_Library library;

void store_glyph(FT_GlyphSlotRec *glyph, int ch, int size, char *name, FILE *c, char **post, int *post_len, int *post_count);

int main(int argc, const char *argv[])
{
	int len, error;

	char *font_filename = NULL;
	char *char_list = DEFAULT_CHAR_LIST;
	char *output_name = NULL;
	char *output_dir = ".";
	int font_size = 10;

	struct poptOption opts[] = {
		{ "font",  'f', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_filename, 1, "Font filename",		   "file"    },
		{ "size",  's', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,    &font_size,     1, "Font size",			   "integer" },
		{ "chars", 'c', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &char_list,     1, "List of characters to produce",   "string"  },
		{ "name",  'n', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &output_name,   1, "Output name (without extension)", "file"    },
		{ "dir",   'd', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &output_dir,    1, "Output directory",		   "dir"     },
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

	len = strlen(output_dir) + strlen(output_name) + 32;
	char *c_name = malloc(len);
	snprintf(c_name, len, "%s/font-%s-%d.c", output_dir, output_name, font_size);
	FILE *c = fopen(c_name, "w");
	if (c == NULL) {
		fprintf(stderr, "ERROR: Can't open '%s' for writing: %s\n",
			c_name, strerror(errno));
		return 1;
	}

	char *h_name = malloc(len);
	snprintf(h_name, len, "%s/font-%s-%d.h", output_dir, output_name, font_size);
	FILE *h = fopen(h_name, "w");
	if (h == NULL) {
		fprintf(stderr, "ERROR: Can't open '%s' for writing: %s\n",
			h_name, strerror(errno));
		return 1;
	}
	char *h_basename = strrchr(h_name, '/');
	if (h_basename == NULL) h_basename = h_name;
	else h_basename++;

	// Initial output in the .c file
	fprintf(c, "%s",
		"/* AUTOMATICALLY GENERATED FILE! EDITING NOT RECOMMENDED!\n"
		" *\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(c, "#include <stdio.h>\n");
	fprintf(c, "#include \"fontem.h\"\n");
	fprintf(c, "#include \"%s\"\n\n", h_basename);

	// Initial output in the .h file
	fprintf(h, "%s",
		"/* AUTOMATICALLY GENERATED FILE! EDITING NOT RECOMMENDED!\n"
		" *\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(h, "#ifndef _FONTEM_%s_%d_H\n#define _FONTEM_%s_%d_H\n\n",
		output_name, font_size, output_name, font_size);
	fprintf(h, "#include \"fontem.h\"\n\n");

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
	error = FT_Set_Char_Size(face, font_size * 64, 0, FONT_DPI, 0);
	if (error) {
		fprintf(stderr, "ERROR: Can't set the font size to %d", font_size);
		return 1;
	}

	char *font_name = face->family_name;

	// Postamble for the c file
	char *post = malloc(512);
	snprintf(post, 512, "/** Glyphs table for font \"%s\". */\n" \
		 "static struct glyph *glyphs_%s_%d[] = {\n",
		 font_name,
		 output_name, font_size);
	int post_len = strlen(post);
	post = realloc(post, post_len + 1);
	int post_count = 0;

	len = strlen(char_list);
	for (int i = 0; i < len; i++) {
		int ch = char_list[i];

		// Load the glyph
		error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
		if (error) {
			fprintf(stderr, "ERROR : Can 't load glyph for %c.\n", ch);
			return 1;
		}

		store_glyph(face->glyph, ch, font_size, output_name, c, &post, &post_len, &post_count);
	}

	// Finish the post
	fprintf(c, "%s"	\
		"};\n\n", post);
	free(post);

	fprintf(c, "/** Definition for font \"%s\". */\n", font_name);
	fprintf(c, "struct font font_%s_%d = {\n" \
		"\t.name = \"%s\",\n" \
		"\t.style = \"%s\",\n" \
		"\t.size = %d,\n" \
		"\t.dpi = %d,\n" \
		"\t.count = %d,\n" \
		"\t.glyphs = glyphs_%s_%d,\n" \
		"};\n\n",
		output_name, font_size,
		font_name, face->style_name, font_size, FONT_DPI,
		post_count, output_name, font_size);

	// Add the reference to the .h
	fprintf(h, "extern struct font font_%s_%d;\n\n", output_name, font_size);

	// All done!
	fprintf(h, "#endif /* _FONTEM_%s_%d_H */\n", output_name, font_size);
	fclose(h);
	fclose(c);

	return 0;
}

void store_glyph(FT_GlyphSlotRec *glyph, int ch, int size, char *name,
		 FILE *c, char **post, int *post_len, int *post_count)
{
	FT_Bitmap *bitmap = &glyph->bitmap;

	// Work out a name for this glyph
	int len = strlen(name) + 24;
	char *bname = malloc(len);
	char *gname = malloc(len);

	snprintf(bname, len, "bitmap_%s_%d_%04x", name, size, ch);
	snprintf(gname, len, "glyph_%s_%d_%04x", name, size, ch);

	if (bitmap->rows && bitmap->width) {
		fprintf(c, "/** Bitmap definition for character '%c'. */\n", ch);
		fprintf(c, "static unsigned char %s[] = {\n", bname);
		for (int y = 0; y < bitmap->rows; y++) {
			fprintf(c, "\t");
			for (int x = 0; x < bitmap->width; x++)
				fprintf(c, "0x%02x, ", (unsigned char)bitmap->buffer[y * bitmap->width + x]);
			fprintf(c, "\n");
		}
		fprintf(c, "};\n\n");
	} else {
		free(bname);
		bname = strdup("NULL");
	}

	fprintf(c, "/** Glyph definition for character '%c'. */\n", ch);
	fprintf(c, "static struct glyph %s = {\n", gname);
	fprintf(c, "\t.left = %d,\n", glyph->bitmap_left);
	fprintf(c, "\t.top = %d,\n", glyph->bitmap_top);
	fprintf(c, "\t.advance = %d,\n", (int)glyph->advance.x);
	fprintf(c, "\t.cols = %d,\n", bitmap->width);
	fprintf(c, "\t.rows = %d,\n", bitmap->rows);
	fprintf(c, "\t.bitmap = %s,\n", bname);
	fprintf(c, "};\n\n");

	// Append to the post
	char *str = malloc(len + 100);
	snprintf(str, len + 100, "\t[%d] = &%s,  /* '%c' */\n", ch, gname, ch);
	*post = realloc(*post, (*post_len) + strlen(str) + 1);
	strcpy((*post) + (*post_len), str);
	(*post_len) += strlen(str);
	(*post_count)++;

	free(str);
	free(bname);
	free(gname);
}

// vim: set softtabstop=8 shiftwidth=8 tabstop=8:

// vim: set softtabstop=4 shiftwidth=4 tabstop=4 expandtab:
