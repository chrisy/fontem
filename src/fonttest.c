/**
 * \file src/fonttest.c
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

#include "resource/fontem.h"
#include "fonts/font_all.h"

#ifndef EOL
#define EOL "\n"
#endif

static void fia_print_table(struct font *font, void *opaque)
{
	FILE *out = (FILE *)opaque;

	fprintf(out, "%-20s %-8s %-6d %-6d %-6d %-3c" EOL,
		font->name,
		font->style,
		font->size,
		font->height,
		font->ascender + font->descender,
		font->compressed ? 'Y' : ' ');
}

/** Dumps a table of all the fonts we know about. */
static void font_print_all(FILE *out)
{
	fprintf(out, "%-20s %-8s %-6s %-6s %-6s %-3s" EOL,
		"Font name", "Style", "Size",
		"Vdist", "Height", "RLE");
	font_iterate_all(fia_print_table, (void *)out);
}

static void fia_print_args(struct font *font, void *opaque)
{
	FILE *out = (FILE *)opaque;

	fprintf(out, "--fontname=\"%s\" --fontstyle=\"%s\" " \
		"--fontsize=%d --fontrle=%d\n",
		font->name,
		font->style,
		font->size,
		font->compressed);
}

/** Dumps the fonttable as a set of args that can be used
 * with fonttest to invoke that font. */
static void font_print_args(FILE *out)
{
	font_iterate_all(fia_print_args, (void *)out);
}

int main(int argc, const char *argv[])
{
	char *string = "Test";
	char *font_name = "DejaVu Serif";
	char *font_style = NULL;
	int font_size = 10;
	int font_rle = -1;
	int width = -1;
	int height = -1;

	struct poptOption opts[] = {
		{ "text",      't', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &string,     1, "String to render",		       "text"  },
		{ "fontname",  'f', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_name,  1, "Name of the font to use",	       "font"  },
		{ "fontstyle", 'S', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_style, 1, "Style of the font to use",	       "style" },
		{ "fontsize",  's', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &font_size,  1, "Size of the fonr to use",	       "pts"   },
		{ "fontrle",   'r', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &font_rle,   1, "0 = no RLE, 1 = RLE only, -1 = any", "mode"  },
		{ "width",     'w', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &width,      1, "Canvas width",		       "chars" },
		{ "height",    'h', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &height,     1, "Canvas height",		       "chars" },
		{ "list",      'l', 0,						 NULL,	      2, "List available fonts",	       NULL    },
		{ "list-args", 'a', 0,						 NULL,	      3, "List available fonts as shell args", NULL    },
		POPT_AUTOHELP
		POPT_TABLEEND
	};

	poptContext ctx = poptGetContext(NULL, argc, argv, opts, 0);
	int rc;

	while ((rc = poptGetNextOpt(ctx)) > 0) {
		switch (rc) {
		case 2:
			font_print_all(stdout);
			return 0;
		case 3:
			font_print_args(stdout);
			return 0;
		}
	}

	poptFreeContext(ctx);

	if (string == NULL) {
		fprintf(stderr, "ERROR: You must specify a string to render.\n");
		return 1;
	}

	if (font_name == NULL) {
		fprintf(stderr, "ERROR: You must specify a font name to render.\n");
		return 1;
	}

	const struct font *font = font_find_all(font_name, font_style, font_size, (char)font_rle);

	if (font == NULL) {
		fprintf(stderr, "ERROR: Unable to find a font matching \"%s\" size \"%d\".\n",
			font_name, font_size);
		return 1;
	}

	if (width == -1 || height == -1) {
		int w = 0, h = 0;
		font_calculate_box(font, &w, &h, string);
		if (width == -1) width = w;
		if (height == -1) height = h;
	}

	uint8_t *canvas = malloc(((size_t)width * (size_t)height) + 1);
	memset(canvas, ' ', (size_t)width * (size_t)height);

	char *p = string;

	int x = 0;
	char prev = 0;
	while (*p) {
		x += font_draw_char_L(font, x, 0, width, height, canvas, *p, prev);
		prev = *p;
		p++;
	}

	for (int y = 0; y < height; y++) {
		uint8_t *p = canvas + (y * width);
		uint8_t *q = p + width;
		uint8_t ch = *q;
		*q = '\0';
		puts((const char *)p);
		*q = ch;
	}

	return 0;
}
