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
	fontrender.c \
	fontfinder.c

fonttest_sources = \
	fonttest.c

fontem_SOURCES = $(fontem_sources)

libfontem_la_SOURCES = $(libfontem_sources)
libfontem_la_CPPFLAGS = -Iresource -Ifonts $(AM_CPPFLAGS)

fonttest_LDADD = libfontem.la libfonts.la
fonttest_SOURCES = $(fonttest_sources)
fonttest_CPPFLAGS = -Iresource $(AM_CPPFLAGS)
fonttest.c: $(fontout)/font_all.h

fontdir := $(top_srcdir)/fonts
fontout := fonts

fonts_sources = \
	fonts/font-UbuntuMonoR-10.c \
	fonts/font-UbuntuMonoR-16.c \
	fonts/font-UbuntuMonoB-10.c \
	fonts/font-UbuntuMonoB-16.c \
	fonts/font-DejaVuSerif-10.c \
	fonts/font-DejaVuSerif-16.c

BUILT_SOURCES = $(fonts_sources) fonts/font_all.c
libfonts_la_SOURCES = $(fonts_sources) fonts/font_all.c
libfonts_la_CPPFLAGS = -Iresource $(AM_CPPFLAGS)

.SECONDEXPANSION:
$(fonts_sources): %.c: $$(fontdir)/$$(word 2,$$(subst -, ,%)).ttf resource/fontem.h fontem
	@mkdir -p $(fontout)
	./fontem --font=$(fontdir)/$(word 2,$(subst -, ,$(notdir $@))).ttf \
		--dir=$(fontout) --name=$(word 2,$(subst -, ,$(notdir $@))) --size=$(word 3,$(subst -, ,$(subst .c,,$(notdir $@))))

$(fontout)/font_all.h: $(fonts_sources) Makefile
	@echo "/* A list of all font headers. */" > $@
	@echo >> $@
	@echo "#include \"fontem.h\"" >> $@
	@echo >> $@
	for font in $(sort $(notdir $(fonts_sources:%.c=%.h))); do echo "#include \"$$font\"" >> $@; done
	@echo >> $@
	@echo "extern const struct font *fonts[];" >> $@
	@echo "extern const int font_count;" >> $@

$(fontout)/font_all.c: $(fontout)/font_all.h Makefile
	@echo "/* A list of all fonts. */" > $@
	@echo >> $@
	@echo "#include <stdio.h>" > $@
	@echo "#include \"fontem.h\"" >> $@
	@echo "#include \"font_all.h\"" >> $@
	@echo >> $@
	@echo "const int font_count = $(words $(fonts_sources));" >> $@
	@echo >> $@
	@echo "const struct font *fonts[] = {" >> $@
	for font in $(sort $(subst -,_,$(notdir $(fonts_sources:%.c=%)))); do printf "\t&$$font,\n" >> $@; done
	@printf "\tNULL\n" >> $@
	@echo "};" >> $@

	
clean-local:
	rm -f $(fonts_sources) $(fonts_sources:.c=.h) $(fontout)/font_all.h $(fontout)/font_all.c

