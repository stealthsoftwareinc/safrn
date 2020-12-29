#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_expect_extension)" == function ]]; then
  return
fi

sst_import_function \
;

sst_expect_extension() {

  sst_expect_argument_count $# 2
  if [[ "$1" != *[!/]"$2" ]]; then
    sst_barf 'path must end in %s: %s' "$2" "$1"
  fi

}

readonly -f sst_expect_extension
