#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_arrays)" != function ]]; then
jq_expect_arrays() {

  case $# in
    2)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_types "$1" "$2" array

}; readonly -f jq_expect_arrays; fi
