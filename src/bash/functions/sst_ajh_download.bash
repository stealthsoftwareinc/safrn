#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_ajh_download)" == function ]]; then
  return
fi

sst_import_function \
;

sst_ajh_download() {

  local dir
  local ff
  local tt
  local x1
  local x2
  local x3
  local x4

  case $# in
    1)
    ;;
    *)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  dir=$1
  readonly dir

  for x1 in $dir/**/*.ag.json; do

    x2=$(sst_expect_ag_json "$x1")
    x3=$(basename $x2)
    x4=$(sst_underscore_slug $x3)

    tt=$(jq .type $x1)

    case $tt in

      null)

        autogen_ac_append <<EOF

]m4_define(
  [${x4}_urls_default],
  [[ ]dnl
[$dir/local/$x3 ]dnl
EOF

        jq -r '
          .urls[]
          | gsub("\\$"; "$][$][")
          | "['\'\\\\\'\''" + . + "'\'\\\\\'\'' ]dnl"
        ' $x1 | autogen_ac_append

        autogen_ac_append <<EOF
])[

case $][{${x4}_urls+x} in
  "")
    ${x4}_urls=']${x4}_urls_default['
  ;;
esac
readonly ${x4}_urls

]AC_ARG_VAR(
  [${x4}_urls],
  [
    the URLs from which to download the
    $x3
    file (default:
    ${x4}_urls=']${x4}_urls_default[')
  ])[

]GATBPS_WGET(
  [$x2],
  [
    [\$(${x4}_urls)],
  ],
  [
EOF

        jq -r '
          .hashes | to_entries[] | "    [" + .key + ":" + .value + "],"
        ' $x1 | autogen_ac_append

        autogen_ac_append <<EOF
  ],
  [clean])[

EOF

      ;;

      \"copy\")

        ff=$(jq -r .file $x1)

        autogen_ac_append <<EOF

]GATBPS_CP(
  [$x2],
  [$ff],
  [file],
  [clean])[

EOF

      ;;

      *)

        sst_barf 'unknown type: %s' "$tt"

      ;;

    esac

  done

}

readonly -f sst_ajh_download
