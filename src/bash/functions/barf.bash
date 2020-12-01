#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
barf)" != function ]]; then
barf() {
  set +x

  local i
  local x

  if [[ ! -f "$tmpdir"/barf ]]; then

    >"$tmpdir"/barf

    printf '%s: ' "$0" >&2
    case $# in
      0)
        printf 'unknown error' >&2
      ;;
      *)
        printf "$@" >&2
      ;;
    esac
    printf '\n' >&2

    printf '%s: stack trace:\n' "$0" >&2
    i=0
    while x=$(caller $i); do
      printf '  %s\n' "$x" >&2
      i=$(($i + 1))
    done

  fi

  exit 1

}; readonly -f barf; fi
