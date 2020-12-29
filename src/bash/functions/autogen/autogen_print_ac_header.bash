#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_print_ac_header
#

if [[ "$(type -t autogen_print_ac_header)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_print_ac_header() {

  if (($# != 0)); then
    sst_barf 'invalid argument count: %d' $#
  fi

  cat <<"EOF"
dnl
dnl For the copyright information for this file, please search up the
dnl directory tree for the first COPYING file.
dnl

dnl
dnl This file was generated by autogen.
dnl

EOF

}

readonly -f autogen_print_ac_header
