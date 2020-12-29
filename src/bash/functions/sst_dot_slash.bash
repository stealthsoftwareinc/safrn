#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_dot_slash)" == function ]]; then
  return
fi

sst_import_function \
;

sst_dot_slash() {

  local path

  sst_expect_argument_count $# 0-1

  if (($# == 0)); then
    path=$(cat | sst_csf)
    sst_csf path
  else
    path=$1
  fi

  if [[ "$path" != /* ]]; then
    path=./$path
  fi

  printf '%s\n' "$path"

}

readonly -f sst_dot_slash
