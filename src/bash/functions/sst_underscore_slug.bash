#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_underscore_slug)" == function ]]; then
  return
fi

sst_import_function \
;

sst_underscore_slug() {

  if (($# == 0)); then
    local input
    input=$(cat | sst_csf)
    sst_csf input
    printf '%s\n' "${input//[!0-9A-Za-z]/_}"
  else
    sst_join "$@" | sst_underscore_slug
  fi

}

readonly -f sst_underscore_slug
