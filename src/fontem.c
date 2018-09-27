/**
 * \file src/fontem.c
 *
 * This file is distributed under the terms of the MIT License.
 * See the LICENSE file at the top of this tree, or if it is missing a copy can
 * be found at http://opensource.org/licenses/MIT
 */

#include <ctype.h>
#include <config.h>
#include <errno.h>
#include <locale.h>
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#define DEFAULT_CHAR_LIST "!@#$%^&*()_+-={}|[]\\:\";'<>?,./`~" \
	" " \
	"ABCDEFGHIJKLMNOPQRSTUVWXYZ" \
	"abcdefghijklmnopqrstuvwxyz" \
	"0123456789" \
	"ÄÖÜßäöü"

#define FONT_DPI 100

/** Font library handle */
FT_Library library;
char *section = NULL;

void store_glyph(FT_Face *face, FT_GlyphSlotRec *glyph, int ch, int size, char *name, FILE *c, char **post, int *post_len, int *post_count, int with_kerning, wchar_t *char_list, int compress);
static char *get_section(char *name);
static int cmp_wchar(const void *p1, const void *p2);

static char *mb(wchar_t wchar)
{
	static char ch_mb[16];
	int mb_len = wctomb(ch_mb, wchar);

	ch_mb[mb_len] = '\0';
	return ch_mb;
}

static char *validate_identifier(const char *identifier)
{
	char *result = strdup(identifier);

	for (char *p = result; *p; p++) {
		if (isdigit(*p) && (p == result))
			*p = '_';
#ifndef __GNUC__
		else if (*p == '$')
			*p = '_';
#endif
		else if (!isalnum(*p))
			*p = '_';
	}
	return result;
}

int main(int argc, const char *argv[])
{
	setlocale(LC_ALL, "");

	int len, error;
	int rle = 0;

	char *font_filename = NULL;
	char *char_list = strdup(DEFAULT_CHAR_LIST);
	char *output_name = NULL;
	char *output_dir = ".";
	char *append = "";
	char *append_sane = NULL;
	int font_size = 10;

	struct poptOption opts[] = {
		{ "font",    'f', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_filename, 1, "Font filename",		     "file"    },
		{ "size",    's', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,    &font_size,     1, "Font size",			     "integer" },
		{ "chars",   'c', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &char_list,     1, "List of characters to produce",   "string"  },
		{ "name",    'n', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &output_name,   1, "Output name (without extension)", "file"    },
		{ "dir",     'd', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &output_dir,    1, "Output directory",		     "dir"     },
		{ "section", 0,	  POPT_ARG_STRING,			       &section,       1, "Section for font data",	     "name"    },
		{ "rle",     0,	  POPT_ARG_VAL | POPT_ARGFLAG_SHOW_DEFAULT,    &rle,	       1, "Use RLE compression",	     "rle"     },
		{ "append",  0,	  POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &append,	       1, "Append str to filename, structs", ""	       },
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

	// Make a copy of 'append' with C-sane characters
	append_sane = validate_identifier(append);

	// Convert the char list into wide characters and sort it
	size_t char_count = mbstowcs(NULL, char_list, 0);
	if (char_count == (size_t)-1) {
		perror("converting char list");
		return 1;
	}
	wchar_t *wide_char_list = (wchar_t *)calloc(char_count + 1, sizeof(wchar_t));
	char_count = mbstowcs(wide_char_list, char_list, char_count + 1);
	qsort(wide_char_list, char_count, sizeof(wchar_t), cmp_wchar);

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

	// Open the output files
	len = strlen(output_dir) + strlen(output_name) + strlen(append) + 32;
	char *c_name = malloc(len);
	snprintf(c_name, len, "%s/font-%s-%d%s.c",
		 output_dir,
		 output_name,
		 font_size,
		 append);
	FILE *c = fopen(c_name, "w");
	if (c == NULL) {
		fprintf(stderr, "ERROR: Can't open '%s' for writing: %s\n",
			c_name, strerror(errno));
		return 1;
	}

	char *h_name = malloc(len);
	snprintf(h_name, len, "%s/font-%s-%d%s.h",
		 output_dir,
		 output_name,
		 font_size,
		 append);
	FILE *h = fopen(h_name, "w");
	if (h == NULL) {
		fprintf(stderr, "ERROR: Can't open '%s' for writing: %s\n",
			h_name, strerror(errno));
		return 1;
	}
	char *h_basename = strrchr(h_name, '/');
	if (h_basename == NULL) h_basename = h_name;
	else h_basename++;

	// Fix output name for the C identifier
	char *output_name_c = validate_identifier(output_name);

	// Initial output in the .c file
	fprintf(c, "%s",
		"/* AUTOMATICALLY GENERATED FILE! EDITING NOT RECOMMENDED!\n"
		" *\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(c, "#include <stdio.h>\n");
	fprintf(c, "#include <stdlib.h>\n");
	fprintf(c, "#include \"fontem.h\"\n");
	fprintf(c, "#include \"%s\"\n\n", h_basename);
	fprintf(c, "/* Character list: %s */\n\n", char_list);

	// Initial output in the .h file
	fprintf(h, "%s",
		"/* AUTOMATICALLY GENERATED FILE! EDITING NOT RECOMMENDED!\n"
		" *\n"
		" * This file is distributed under the terms of the MIT License.\n"
		" * See the LICENSE file at the top of this tree, or if it is missing a copy can\n"
		" * be found at http://opensource.org/licenses/MIT\n"
		" */\n\n");
	fprintf(h, "#ifndef _FONTEM_%s_%d%s_H\n#define _FONTEM_%s_%d%s_H\n\n",
		output_name_c, font_size, append_sane,
		output_name_c, font_size, append_sane);
	fprintf(h, "#include \"fontem.h\"\n\n");


	// Postamble for the c file
	char *post = malloc(512);
	snprintf(post, 512, "/** Glyphs table for font \"%s\". */\n" \
		 "static const struct glyph *glyphs_%s_%d%s[] %s= {\n",
		 font_name, output_name_c, font_size, append_sane,
		 get_section(output_name_c));
	int post_len = strlen(post);
	post = realloc(post, post_len + 1);
	int post_count = 0;
	wchar_t post_max = 0;

	// Are we kerning?
	int with_kerning = FT_HAS_KERNING(face);

	// Iterate the character list and generate the bitmap for each
	for (size_t i = 0; i < char_count; i++) {
		wchar_t ch = wide_char_list[i];
		if (ch > post_max)
			post_max = ch;

		// Load the glyph
		error = FT_Load_Char(face, ch, FT_LOAD_RENDER);
		if (error) {
			fprintf(stderr, "ERROR : Can't load glyph for %s.\n", mb(ch));
			return 1;
		}

		store_glyph(&face, face->glyph, ch, font_size, output_name_c, c, &post,
			    &post_len, &post_count, with_kerning, wide_char_list, rle);
	}

	// Finish the post
	fprintf(c, "%s"	\
		"};\n\n", post);
	free(post);

	fprintf(c, "/** Definition for font \"%s\". */\n", font_name);
	fprintf(c, "const struct font font_%s_%d%s %s= {\n"	\
		"\t.name = \"%s\",\n" \
		"\t.style = \"%s\",\n" \
		"\t.size = %d,\n" \
		"\t.dpi = %d,\n" \
		"\t.count = %d,\n" \
		"\t.max = %d,\n" \
		"\t.ascender = %d,\n" \
		"\t.descender = %d,\n" \
		"\t.height = %d,\n" \
		"\t.glyphs = glyphs_%s_%d%s,\n"	\
		"\t.compressed = %u,\n"	\
		"};\n\n",
		output_name_c, font_size, append_sane,
		get_section(output_name_c),
		font_name, face->style_name, font_size, FONT_DPI,
		post_count, post_max,
		(int)face->size->metrics.ascender / 64,
		(int)face->size->metrics.descender / 64,
		(int)face->size->metrics.height / 64,
		output_name_c, font_size, append_sane,
		rle);

	// Add the reference to the .h
	fprintf(h, "extern const struct font font_%s_%d%s;\n\n",
		output_name_c, font_size, append_sane);

	// All done!
	fprintf(h, "#endif /* _FONTEM_%s_%d%s_H */\n",
		output_name_c, font_size, append_sane);
	free(output_name_c);
	fclose(h);
	fclose(c);

	return 0;
}

static inline char get_class(unsigned char c)
{
	if (c == 0)
		return 1;
	else if (c == 0xff)
		return 2;
	else
		return 3;
}

static unsigned char *rle_compress(const unsigned char *data, size_t *length)
{
	size_t in_length = *length, out_length = 0;
	unsigned char *result = (unsigned char *)malloc(2 * in_length);
	unsigned char class = 0, count = 0;

	for (size_t i = 0; i <= in_length; i++) {
		unsigned char c = data[i];
		unsigned char c_class = (i == in_length ? 0 : get_class(c));

		if (((count > 0) && (class != c_class)) || (count >= 0x40)) {
			if (class == 3) {
				result[out_length] = count - 1;
				out_length++;

				memcpy(result + out_length, data + i - count, count);
				out_length += count;
			} else {
				result[out_length] = (class == 1 ? 0x80 : 0xc0) + count - 1;
				out_length++;
			}
			count = 0;
		}
		class = c_class;
		count++;
	}

	*length = out_length;
	return result;
}

static void store_bitmap(FILE *c, FT_Bitmap *bitmap, char *bname, wchar_t ch, int compress)
{
	if (bitmap->rows && bitmap->width) {
		fprintf(c, "/** Bitmap definition for character '%s'. */\n", mb(ch));
		fprintf(c, "static const uint8_t %s[] %s= {\n", bname, get_section(bname));
		if (compress) {
			size_t length = (size_t)bitmap->rows * (size_t)bitmap->width;
			unsigned char *compressed_data = rle_compress(bitmap->buffer, &length);
			fprintf(c, "\t");
			for (size_t i = 0; i < length; i++) {
				fprintf(c, "0x%02x, ", compressed_data[i]);
				if (i == length - 1)
					fprintf(c, "\n");
				else if ((i % 16) == 15)
					fprintf(c, "\n\t");
			}
			free(compressed_data);
		} else {
			for (unsigned int y = 0; y < (unsigned int)bitmap->rows; y++) {
				fprintf(c, "\t");
				for (unsigned int x = 0; x < (unsigned int)bitmap->width; x++)
					fprintf(c, "0x%02x, ", (unsigned char)bitmap->buffer[y * bitmap->width + x]);
				fprintf(c, "\n");
			}
		}
		fprintf(c, "};\n\n");
	} else {
		strcpy(bname, "NULL");
	}
}

void store_glyph(FT_Face *face, FT_GlyphSlotRec *glyph,
		 wchar_t ch, int size, char *name,
		 FILE *c, char **post, int *post_len, int *post_count,
		 int with_kerning, wchar_t *char_list, int compress)
{
	FT_Bitmap *bitmap = &glyph->bitmap;

	// Work out a name for this glyph
	int len = strlen(name) + 24;
	char *bname = malloc(len);
	char *gname = malloc(len);
	char *kname = malloc(len);

	snprintf(bname, len, "bitmap_%s_%d_%04x", name, size, ch);
	snprintf(gname, len, "glyph_%s_%d_%04x", name, size, ch);
	snprintf(kname, len, "kerning_%s_%d_%04x", name, size, ch);

	// Generate the bitmap
	store_bitmap(c, bitmap, bname, ch, compress);

	// Generate the kerning table
	if (with_kerning) {
		fprintf(c, "/** Kerning table for character '%s'. */\n", mb(ch));
		fprintf(c, "static const struct kerning %s[] %s= {\n", kname, get_section(kname));
		wchar_t *a = char_list;
		while (*a) {
			FT_Vector kern;
			FT_Get_Kerning(*face, *a, ch, FT_KERNING_DEFAULT, &kern);
			if (kern.x)
				fprintf(c, "\t{ /* .left = '%s' */ %u, /* .offset = */ %d },\n",
					mb(*a), *a, (int)kern.x / 64);
			a++;
		}
		fprintf(c, "\t{ /* .left = */ 0, /* .offset = */ 0 },\n");
		fprintf(c, "};\n\n");
	} else {
		free(kname);
		kname = strdup("NULL");
	}

	fprintf(c, "/** Glyph definition for character '%s'. */\n", mb(ch));
	fprintf(c, "static const struct glyph %s %s= {\n", gname, get_section(gname));
	fprintf(c, "\t.glyph = %u,\n", ch);
	fprintf(c, "\t.left = %d,\n", glyph->bitmap_left);
	fprintf(c, "\t.top = %d,\n", glyph->bitmap_top);
	fprintf(c, "\t.advance = %d,\n", (int)glyph->advance.x / 64);
	fprintf(c, "\t.cols = %d,\n", bitmap->width);
	fprintf(c, "\t.rows = %d,\n", bitmap->rows);
	fprintf(c, "\t.bitmap = %s,\n", bname);
	fprintf(c, "\t.kerning = %s,\n", kname);
	fprintf(c, "};\n\n");

	// Append to the post
	char *str = malloc(len + 100);
	snprintf(str, len + 100, "\t&%s,  /* U+%04X '%s' */\n", gname, ch, mb(ch));
	*post = realloc(*post, (*post_len) + strlen(str) + 1);
	strcpy((*post) + (*post_len), str);
	(*post_len) += strlen(str);
	(*post_count)++;

	free(str);
	free(bname);
	free(gname);
	free(kname);
}

char *get_section(char *name)
{
	static char str[512];
	static int count = 0;

	if (section == NULL) return "";
	snprintf(str, 512, "__attribute__ ((section (\"%s.%s_%04x\"))) ", section, name, count++);
	return str;
}

int cmp_wchar(const void *p1, const void *p2)
{
	return *(wchar_t *)p1 - *(wchar_t *)p2;
}

// vim: set softtabstop=8 shiftwidth=8 tabstop=8:
