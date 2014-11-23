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

int main(int argc, const char *argv[])
{
	char *string = "Test";
	char *font_name = "DejaVu Serif";
	char *font_style = NULL;
	int font_size = 10;
	int width = -1;
	int height = -1;

	struct poptOption opts[] = {
		{ "text",      't', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &string,     1, "String to render",	     "text"  },
		{ "fontname",  'f', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_name,  1, "Name of the font to use",  "font"  },
		{ "fontstyle", 'S', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &font_style, 1, "Style of the font to use", "style" },
		{ "fontsize",  's', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &font_size,  1, "Size of the fonr to use",  "pts"   },
		{ "width",     'w', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &width,      1, "Canvas width",	     "chars" },
		{ "height",    'h', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,	 &height,     1, "Canvas height",	     "chars" },
		{ "list",      'l', 0,						 NULL,	      2, "List available fonts",     NULL    },
		POPT_AUTOHELP
		POPT_TABLEEND
	};

	poptContext ctx = poptGetContext(NULL, argc, argv, opts, 0);
	int rc;

	while ((rc = poptGetNextOpt(ctx)) > 0) {
		switch (rc) {
		case 2:
			font_print_all(stdout);
			exit(0);
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

	const struct font *font = font_find(font_name, font_style, font_size);

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

	uint8_t *canvas = malloc(width * height);
	memset(canvas, ' ', width * height);

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
