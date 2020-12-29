#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_am_distribute)" == function ]]; then
  return
fi

sst_import_function \
;

sst_am_distribute() {

  local x
  for x; do
    sst_expect_source_path "$x"
    autogen_am_var_append_files EXTRA_DIST $x
  done

}

readonly -f sst_am_distribute
