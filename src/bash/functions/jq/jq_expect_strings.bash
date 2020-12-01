#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_strings)" != function ]]; then
jq_expect_strings() {

  case $# in
    2)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_types "$1" "$2" string

}; readonly -f jq_expect_strings; fi
