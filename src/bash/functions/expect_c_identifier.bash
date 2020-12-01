#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
expect_c_identifier)" != function ]]; then
expect_c_identifier() {

  local x

  for x; do
    case $x in
      "" | *[!a-zA-Z0-9_]* | [0-9]*)
        barf 'invalid identifier: %s' "$x"
      ;;
    esac
  done

}; readonly -f expect_c_identifier; fi
