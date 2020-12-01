#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# warn [<format> [<argument]...]
#

if [[ "$(type -t \
warn)" != function ]]; then
warn() {

  printf '%s: ' "$0" >&2

  case $# in
    0)
      printf 'unknown warning' >&2
    ;;
    *)
      printf "$@" >&2
    ;;
  esac

  printf '\n' >&2

}; readonly -f warn; fi
