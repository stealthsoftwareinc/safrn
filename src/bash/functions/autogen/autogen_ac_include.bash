#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_ac_include [<file>]...
#

if [[ "$(type -t autogen_ac_include)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_ac_include() {
  local x
  for x; do
    case ${autogen_ac_include_seen=,} in
      *,$x,*)
      ;;
      *)
        autogen_ac_append <<EOF
]m4_include([$x])[
EOF
        autogen_ac_include_seen=$autogen_ac_include_seen$x,
      ;;
    esac
  done
}

readonly -f autogen_ac_include
