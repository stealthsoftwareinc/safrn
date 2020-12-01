#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_booleans)" != function ]]; then
jq_expect_booleans() {

  case $# in
    2)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_types "$1" "$2" boolean

}; readonly -f jq_expect_booleans; fi
