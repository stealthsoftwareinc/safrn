#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_object_or_null)" != function ]]; then
jq_expect_object_or_null() {

  case $# in
    2)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_type_or_null "$1" "$2" object

}; readonly -f jq_expect_object_or_null; fi