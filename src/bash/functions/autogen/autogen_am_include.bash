#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_am_include [<file>]...
#

if [[ "$(type -t autogen_am_include)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_am_include() {
  local x
  for x; do
    case ${autogen_am_include_seen=,} in
      *,$x,*)
      ;;
      *)
        autogen_am_append <<EOF
include \$(srcdir)/$x
EOF
        autogen_am_include_seen=$autogen_am_include_seen$x,
      ;;
    esac
  done
}

readonly -f autogen_am_include
