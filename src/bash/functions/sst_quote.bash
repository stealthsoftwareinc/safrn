#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_quote)" == function ]]; then
  return
fi

sst_import_function \
  sst_join \
;

sst_quote() {

  if (($# == 0)); then
    sed "s/'/'\\\\''/g; 1s/^/'/; \$s/\$/'/"
  else
    sst_join "$@" | sst_quote
  fi

}

readonly -f sst_quote
