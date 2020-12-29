#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_expect_any_file)" == function ]]; then
  return
fi

sst_import_function \
;

sst_expect_any_file() {

  local x
  local xs

  for x; do
    if [[ -f "$x" ]]; then
      return
    fi
  done

  xs=
  if (($# != 0)); then
    for x; do
      xs=$xs${xs:+, }$(sst_quote "$x")
    done
  fi

  sst_barf 'at least one of the following paths must be a file: {%s}' "$xs"

}

readonly -f sst_expect_any_file
