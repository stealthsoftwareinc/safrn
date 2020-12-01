#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
sst_mkdir_p_only)" != function ]]; then
sst_mkdir_p_only() {

  local x
  for x; do
    if [[ ! -e "$x" ]]; then
      mkdir -p -- "$x"
      rmdir -- "$x"
    fi
  done

}; readonly -f sst_mkdir_p_only; fi
