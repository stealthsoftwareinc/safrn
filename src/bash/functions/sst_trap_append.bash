#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# sst_trap_append <arg> <sigspec>...
#

if [[ "$(type -t sst_trap_append)" == function ]]; then
  return
fi

sst_import_function \
;

sst_trap_append() {

  local arg
  local sigspec
  local x

  case $# in
    0 | 1)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  arg=$1
  readonly arg
  shift

  for sigspec; do

    x=$(trap -p -- "$sigspec")

    case $x in
      ?*)
        eval "set -- $x"
        shift $(($# - 2))
        x=$1$'\n'$arg
      ;;
      *)
        x=$arg
      ;;
    esac

    trap -- "$x" "$sigspec"

  done

}

readonly -f sst_trap_append
