#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_ac_append
#

if [[ "$(type -t \
autogen_ac_append)" != function ]]; then
autogen_ac_append() {

  case ${autogen_ac_start_has_been_called+x} in
    "")
      barf 'autogen_ac_start has not been called'
    ;;
  esac

  case ${autogen_ac_finish_has_been_called+x} in
    ?*)
      barf 'autogen_ac_finish has been called'
    ;;
  esac

  case $# in
    0)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  cat >>$autogen_ac_file

}; readonly -f autogen_ac_append; fi
