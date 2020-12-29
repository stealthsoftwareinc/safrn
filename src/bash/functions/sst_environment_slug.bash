#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_environment_slug)" == function ]]; then
  return
fi

sst_import_function \
;

sst_environment_slug() {

  local x

  if (($# != 0)); then
    sst_join "$@" | $FUNCNAME
    return
  fi

  x=$(cat | sst_underscore_slug)
  if [[ "$x" == "" || "$x" == [0-9]* ]]; then
    x=_$x
  fi
  tr a-z A-Z <<<"$x"

}

readonly -f sst_environment_slug
