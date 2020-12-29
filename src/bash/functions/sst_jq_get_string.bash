#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_jq_get_string)" == function ]]; then
  return
fi

sst_import_function \
;

sst_jq_get_string() {

  sst_expect_argument_count $# 2-3
  jq_expect_string "$1" "$2"
  if (($# == 2)); then
    jq -r " $1" <"$2"
  else
    sst_expect_basic_identifier "$3"
    eval $3='$(jq -r " $1" <"$2" | sst_csf)'
    sst_csf $3
  fi

}

readonly -f sst_jq_get_string
