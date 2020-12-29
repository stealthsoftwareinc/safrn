#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_join)" == function ]]; then
  return
fi

sst_import_function \
;

sst_join() {

  # Avoid $* so we don't depend on IFS.
  if (($# >= 1)); then
    printf '%s' "$1"
    if (($# >= 2)); then
      printf ' %s' "${@:2}"
    fi
  fi
  echo

}

readonly -f sst_join
