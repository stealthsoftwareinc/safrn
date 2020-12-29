#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_am_append
#

if [[ "$(type -t autogen_am_append)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_am_append() {

  case ${autogen_am_start_has_been_called+x} in
    "")
      sst_barf 'autogen_am_start has not been called'
    ;;
  esac

  case ${autogen_am_finish_has_been_called+x} in
    ?*)
      sst_barf 'autogen_am_finish has been called'
    ;;
  esac

  case $# in
    0)
    ;;
    *)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  cat >>$autogen_am_file

}

readonly -f autogen_am_append
