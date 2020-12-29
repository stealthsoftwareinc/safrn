#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_echo_eval)" == function ]]; then
  return
fi

sst_import_function \
;

sst_echo_eval() {

  local x
  if (($# == 0)); then
    x=$(cat | sst_csf)
  else
    x=$(sst_join "$@" | sst_csf)
  fi
  sst_csf x
  printf '%s\n' "$x"
  eval " $x"

}

readonly -f sst_echo_eval
