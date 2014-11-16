# src Makefile.am
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT

lib_LTLIBRARIES = libfontem.la libfonts.la
bin_PROGRAMS = fontem fonttest

fontem_sources = \
	fontem.c

libfontem_sources = \
	fontload.c

fonttest_sources = \
	fonttest.c

fontem_SOURCES = $(fontem_sources)

libfontem_la_SOURCES = $(libfontem_sources)
libfontem_la_CPPFLAGS = -Iresource $(AM_CPPFLAGS)

fonttest_LDADD = -lfontem -lfonts
fonttest_SOURCES = $(fonttest_sources)
fonttest_CPPFLAGS = -Iresource $(AM_CPPFLAGS)
fonttest.c: $(fontout)/font_all.h

fontdir := $(top_srcdir)/fonts
fontout := fonts

fonts_sources = \
	fonts/font-UbuntuMonoB-10.c \
	fonts/font-UbuntuMonoB-16.c \
	fonts/font-DejaVuSerif-10.c \
	fonts/font-DejaVuSerif-16.c

BUILT_SOURCES = $(fonts_sources)
libfonts_la_SOURCES = $(fonts_sources)
libfonts_la_CPPFLAGS = -Iresource $(AM_CPPFLAGS)

.SECONDEXPANSION:
$(fonts_sources): %.c: $$(fontdir)/$$(word 2,$$(subst -, ,%)).ttf resource/fontem.h fontem
	@mkdir -p $(fontout)
	./fontem --font=$(fontdir)/$(word 2,$(subst -, ,$(notdir $@))).ttf \
		--dir=$(fontout) --name=$(word 2,$(subst -, ,$(notdir $@))) --size=$(word 3,$(subst -, ,$(subst .c,,$(notdir $@))))

$(fontout)/font_all.h: $(fonts_sources)
	@echo "/* A list of all font headers. */" > $@
	for font in $(notdir $(fonts_sources:%.c=%.h)); do echo "#include \"$$font\"" >> $@; done

clean-local:
	rm -f $(fonts_sources) $(fonts_sources:.c=.h) $(fontout)/font_all.h

all-local: fonts/font_all.h

