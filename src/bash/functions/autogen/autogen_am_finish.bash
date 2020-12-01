#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
autogen_am_finish)" != function ]]; then
autogen_am_finish() {

  case ${autogen_am_start_has_been_called+x} in
    "")
      barf 'autogen_am_start has not been called'
    ;;
  esac

  case ${autogen_am_finish_has_been_called+x} in
    ?*)
      barf 'autogen_am_finish has already been called'
    ;;
  esac
  autogen_am_finish_has_been_called=x
  readonly autogen_am_finish_has_been_called

  case $# in
    0)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

}; readonly -f autogen_am_finish; fi
