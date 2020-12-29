#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t autogen_ac_finish)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_ac_finish() {

  case ${autogen_ac_start_has_been_called+x} in
    "")
      sst_barf 'autogen_ac_start has not been called'
    ;;
  esac

  case ${autogen_ac_finish_has_been_called+x} in
    ?*)
      sst_barf 'autogen_ac_finish has already been called'
    ;;
  esac
  autogen_ac_finish_has_been_called=x
  readonly autogen_ac_finish_has_been_called

  case $# in
    0)
    ;;
    *)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  cat >>$autogen_ac_file <<EOF

]
EOF

}

readonly -f autogen_ac_finish
