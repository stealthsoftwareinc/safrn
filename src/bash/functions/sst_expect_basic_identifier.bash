#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_expect_basic_identifier)" == function ]]; then
  return
fi

sst_import_function \
;

sst_expect_basic_identifier() {

  local arg
  local regex

  regex='[a-zA-Z_][a-zA-Z_0-9]*'
  for arg; do
    if [[ ! "$arg" =~ $regex ]]; then
      sst_barf 'invalid basic identifier: %s' "$arg"
    fi
  done

}

readonly -f sst_expect_basic_identifier
