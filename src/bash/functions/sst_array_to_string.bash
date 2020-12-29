#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_array_to_string)" == function ]]; then
  return
fi

sst_import_function \
  sst_barf \
  sst_expect_argument_count \
  sst_expect_basic_identifier \
  sst_quote \
;

sst_array_to_string() {

  local -r sst_regex='^(0|[1-9][0-9]*)$'

  local sst_xs
  local sst_xis
  local sst_xs_size
  local sst_space
  local sst_i
  local sst_xi
  local sst_x

  sst_expect_argument_count $# 1

  sst_xs=$1

  sst_expect_basic_identifier "$sst_xs"

  eval sst_xis="(\${$sst_xs[@]+\"\${!$sst_xs[@]}\"})"

  eval sst_xs_size="\${$sst_xs[@]+\${#$sst_xs[@]}}"

  printf '('
  sst_space=
  for ((sst_i = 0; sst_i != sst_xs_size; ++sst_i)); do
    sst_xi=${sst_xis[sst_i]}
    eval sst_x="\${$sst_xs[\$sst_xi]}"
    if [[ ! "$sst_xi" =~ $sst_regex ]]; then
      sst_xi=$(sst_quote <<<"$sst_xi")
    fi
    sst_x=$(sst_quote <<<"$sst_x")
    printf '%s[%s]=%s' "$sst_space" "$sst_xi" "$sst_x"
    sst_space=' '
  done
  printf ')\n'

}

readonly -f sst_array_to_string
