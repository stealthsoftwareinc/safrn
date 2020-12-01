#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
jq_inline)" != function ]]; then
jq_inline() {

  local x

  case $# in
    0)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  x=$1
  readonly x
  shift

  jq "$@" <<EOF
$x
EOF

}; readonly -f jq_inline; fi
