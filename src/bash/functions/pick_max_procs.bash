#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t pick_max_procs)" == function ]]; then
  return
fi

sst_import_function \
;

pick_max_procs() {

  local n

  case ${max_procs+x} in
    ?*)
      return
    ;;
  esac

  max_procs=1

  if [ -f /proc/cpuinfo ]; then
    if n=$(grep '^processor' /proc/cpuinfo | wc -l); then
      max_procs=$n
    fi
  fi

  readonly max_procs

}

readonly -f pick_max_procs
