#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_find_dist_version)" == function ]]; then
  return
fi

sst_import_function \
  sst_barf \
  sst_expect_argument_count \
;

sst_find_dist_version() {

  sst_expect_argument_count $# 0

  if [[ -f build-aux/VERSION.sh ]]; then
    sh build-aux/VERSION.sh
    return
  fi

  sst_barf 'unable to find dist version'

}

readonly -f sst_find_dist_version
