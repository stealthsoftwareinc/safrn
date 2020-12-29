#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_find_dist_date)" == function ]]; then
  return
fi

sst_import_function \
  sst_expect_argument_count \
;

sst_find_dist_date() {

  sst_expect_argument_count $# 0

  if [[ -f build-aux/DATE.sh ]]; then
    sh build-aux/DATE.sh
    return
  fi

  date -u '+%Y-%m-%d'

}

readonly -f sst_find_dist_date
