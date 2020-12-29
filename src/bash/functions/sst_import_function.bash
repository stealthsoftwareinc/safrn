#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_import_function)" == function ]]; then
  return
fi

sst_import_function() {

  local x

  for x; do
    if [[ "$(type -t "$x")" != function ]]; then
      . "$sst_root/functions/$x.bash"
    fi
  done

}

readonly -f sst_import_function
