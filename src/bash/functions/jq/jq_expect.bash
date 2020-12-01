#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_expect)" != function ]]; then
jq_expect() {

  local x

  expect_argument_count $# 2-

  x=$(jq "$1" "$2")
  readonly x

  case $x in
    "" | false)
      shift 2
      barf "$@"
    ;;
  esac

}; readonly -f jq_expect; fi
