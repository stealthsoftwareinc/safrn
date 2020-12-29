#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t make_j)" == function ]]; then
  return
fi

sst_import_function \
;

make_j() {

  case ${make_j_is_supported+x} in
    "")
      if make -j 1 -f /dev/null . >/dev/null 2>&1; then
        make_j_is_supported=x
      else
        make_j_is_supported=
      fi
      readonly make_j_is_supported
    ;;
  esac

  case $make_j_is_supported in
    ?*)
      pick_max_procs
      make -j $max_procs "$@"
    ;;
    "")
      make "$@"
    ;;
  esac

}

readonly -f make_j
