#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_array_cmp)" == function ]]; then
  return
fi

sst_import_function \
  sst_expect_argument_count \
  sst_expect_basic_identifier \
;

sst_array_cmp() {

  local -r sst_ir='^(0|[1-9][0-9]*)$'

  local sst_xs
  local sst_ys
  local sst_xis
  local sst_yis
  local sst_xs_size
  local sst_ys_size
  local sst_min
  local sst_i
  local sst_xi
  local sst_yi
  local sst_x
  local sst_y

  sst_expect_argument_count $# 2

  sst_xs=$1
  sst_ys=$2

  sst_expect_basic_identifier "$sst_xs"
  sst_expect_basic_identifier "$sst_ys"

  eval sst_xis="(\${$sst_xs[@]+\"\${!$sst_xs[@]}\"})"
  eval sst_yis="(\${$sst_ys[@]+\"\${!$sst_ys[@]}\"})"

  eval sst_xs_size="\${$sst_xs[@]+\${#$sst_xs[@]}}"
  eval sst_ys_size="\${$sst_ys[@]+\${#$sst_ys[@]}}"

  if ((sst_xs_size < sst_ys_size)); then
    sst_min=$sst_xs_size
  else
    sst_min=$sst_ys_size
  fi

  for ((sst_i = 0; sst_i != sst_min; ++sst_i)); do
    sst_xi=${sst_xis[sst_i]}
    sst_yi=${sst_yis[sst_i]}
    if [[ "$sst_xi" =~ $sst_ir && "$sst_yi" =~ $sst_ir ]]; then
      ((sst_xi < sst_yi)) && printf '%s\n' -1 && return
      ((sst_xi > sst_yi)) && printf '%s\n'  1 && return
    else
      [[ "$sst_xi" < "$sst_yi" ]] && printf '%s\n' -1 && return
      [[ "$sst_xi" > "$sst_yi" ]] && printf '%s\n'  1 && return
    fi
  done
  ((sst_xs_size < sst_ys_size)) && printf '%s\n' -1 && return
  ((sst_xs_size > sst_ys_size)) && printf '%s\n'  1 && return
  for ((sst_i = 0; sst_i != sst_min; ++sst_i)); do
    sst_xi=${sst_xis[sst_i]}
    sst_yi=${sst_yis[sst_i]}
    eval sst_x="\${$sst_xs[\$sst_xi]}"
    eval sst_y="\${$sst_ys[\$sst_yi]}"
    [[ "$sst_x" < "$sst_y" ]] && printf '%s\n' -1 && return
    [[ "$sst_x" > "$sst_y" ]] && printf '%s\n'  1 && return
  done
  printf '%s\n'  0

}

readonly -f sst_array_cmp
