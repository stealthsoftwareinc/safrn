#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_array_from_zterm)" == function ]]; then
  return
fi

sst_import_function \
  sst_expect_argument_count \
  sst_expect_basic_identifier \
  sst_quote \
;

sst_array_from_zterm() {

  local sst_xs
  local sst_r
  local sst_t
  local sst_v

  sst_expect_argument_count $# 1

  sst_xs=$1

  sst_expect_basic_identifier "$sst_xs"

  sst_r=$(sst_quote <<<"$sst_root")

  sst_t=$sst_root_tmpdir/$FUNCNAME.sst_t
  if [[ ! -f "$sst_t" ]]; then
    cat >"$sst_t" <<EOF
      set -e
      . $sst_r/preludes/standard.bash
      sst_import_function sst_quote
      for x; do
        x=\$(sst_quote <<<"\$x")
        printf '%s ' "\$x"
      done
EOF
  fi

  sst_v='('$(xargs -0 bash "$sst_t")')'
  eval "$sst_xs=$sst_v"

}

readonly -f sst_array_from_zterm
