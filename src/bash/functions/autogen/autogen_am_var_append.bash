#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_am_var_append <name> [<text>...]
#

if [[ "$(type -t \
autogen_am_var_append)" != function ]]; then
autogen_am_var_append() {

  local name
  local text

  if (( $# == 0 )); then
    barf 'invalid argument count: %d' $#
  fi

  name=$1
  readonly name
  shift
  expect_c_identifier "$name"

  for text; do
    autogen_am_append <<EOF
$name += $text
EOF
  done

}; readonly -f autogen_am_var_append; fi
