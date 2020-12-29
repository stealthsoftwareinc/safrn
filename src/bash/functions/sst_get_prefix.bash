#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_get_prefix)" == function ]]; then
  return
fi

sst_import_function \
;

sst_get_prefix() {

  sst_expect_argument_count $# 0-1
  local x
  if (($# == 0)); then
    x=$(cat | sst_csf)
    sst_csf x
  else
    x=$1
  fi
  if [[ "$x" == */* ]]; then
    x=${x%/*}/
  else
    x=
  fi
  printf '%s\n' "$x"

}

readonly -f sst_get_prefix
