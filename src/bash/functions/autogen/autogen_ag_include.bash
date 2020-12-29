#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_ag_include [<file>]...
#
# Execute and add each file to EXTRA_DIST.
#

if [[ "$(type -t autogen_ag_include)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_ag_include() {
  local x
  for x; do
    . $x
    autogen_am_var_append EXTRA_DIST $x
  done
}

readonly -f autogen_ag_include
