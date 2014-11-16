# root Makefile.am
# vim:set softtabstop=8 shiftwidth=8 tabstop=8 noexpandtab:
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT
#

ACLOCAL_AMFLAGS = -I aclocal

sources = include src
SUBDIRS = $(sources)

EXTRA_DIST = README LICENSE \
	bootstrap do-configure-debug \
	uncrustify.cfg

# Uncrustify the source
uncrustify: amlocal/uncrustify.cfg
	@for dir in $(sources) src/resource; do \
		find "$(top_srcdir)/$$dir" -type f -name '*.[ch]' ! -path '*/fonts/*' -print0 | \
			xargs -0 uncrustify -c $(top_srcdir)/amlocal/uncrustify.cfg --replace --no-backup; \
	done

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

