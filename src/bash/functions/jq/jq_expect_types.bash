#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect_types)" != function ]]; then
jq_expect_types() {

  case $# in
    3)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  case $3 in
    array | boolean | null | number | object | string)
    ;;
    *)
      barf '$3: invalid type: %s' "$3"
    ;;
  esac

  jq_expect "
    $1 | (type == \"array\" and (map(type == \"$3\") | all))
  " "$2" '%s: %s: expected array of %ss' "$2" "$1" $3

}; readonly -f jq_expect_types; fi
