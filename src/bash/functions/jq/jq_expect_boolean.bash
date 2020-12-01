#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_boolean)" != function ]]; then
jq_expect_boolean() {

  case $# in
    2)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_type "$1" "$2" boolean

}; readonly -f jq_expect_boolean; fi