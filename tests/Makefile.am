# test Makefile.am
# vim:set softtabstop=8 shiftwidth=8 tabstop=8 noexpandtab:
#
# This file is distributed under the terms of the MIT License.
# See the LICENSE file at the top of this tree, or if it is missing a copy can
# be found at http://opensource.org/licenses/MIT
#

test_data_dir = test-data


# (Re)-generate test data
generate-test-data:
	mkdir -p "$(test_data_dir)"
	find "$(test_data_dir)" -type f -print0 | xargs -0r rm
	./generate-all "$(test_data_dir)"

test:
	@if [ ! -d "$(test_data_dir)" -o ! -f "$(test_data_dir)"/fontlist ]; then \
		echo -n "ERROR: No test data to compare with. " >&2; \
		echo "Perhaps try 'make generate-test-data'." >&2; \
		echo >&2; \
		exit 1; \
	fi
	./test-all "$(test_data_dir)"
