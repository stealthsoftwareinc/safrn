#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t cc)" == function ]]; then
  return
fi

sst_import_function \
;

cc() {

  case ${cc_command+x} in
    "")
      if command -v c99 >/dev/null; then
        cc_command=c99
      elif command -v c89 >/dev/null; then
        cc_command=c89
      elif command -v cc >/dev/null; then
        cc_command=cc
      else
        sst_barf 'no C compiler found'
      fi
      readonly cc_command
    ;;
  esac

  command $cc_command "$@"

}

readonly -f cc
