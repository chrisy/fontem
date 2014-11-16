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
	int width = 80;
	int height = 20;

	struct poptOption opts[] = {
		{ "string", 's', POPT_ARG_STRING | POPT_ARGFLAG_SHOW_DEFAULT, &string, 1, "String to render", "text"  },
		{ "width",  'w', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,    &width,  1, "Canvas width",     "chars" },
		{ "height", 'h', POPT_ARG_INT | POPT_ARGFLAG_SHOW_DEFAULT,    &height, 1, "Canvas height",    "chars" },
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

	if (string == NULL) {
		fprintf(stderr, "ERROR: You must specify a string to render.\n");
		return 1;
	}

	uint8_t *canvas = malloc(width * height);
	memset(canvas, ' ', width * height);

	char *p = string;

	int x = 0;
	while (*p) {
		x += font_draw_glyph_L(&font_DejaVuSerif_10, x, 0, width, height, canvas, *p);
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
