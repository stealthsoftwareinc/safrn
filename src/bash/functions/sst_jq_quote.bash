#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_jq_quote)" == function ]]; then
  return
fi

sst_import_function \
;

sst_jq_quote() {

  if (($# == 0)); then
    jq -R -s 'rtrimstr("\n")'
  else
    sst_join "$@" | sst_jq_quote
  fi

}

readonly -f sst_jq_quote
