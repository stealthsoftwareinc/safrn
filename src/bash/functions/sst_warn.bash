#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_warn)" == function ]]; then
  return
fi

sst_import_function \
;

sst_warn() {

  printf '%s: ' "$0" >&2
  if (($# == 0)); then
    printf 'unknown warning' >&2
  else
    printf "$@" >&2
  fi
  printf '\n' >&2


}

readonly -f sst_warn
