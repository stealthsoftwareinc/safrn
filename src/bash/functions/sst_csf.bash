#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_csf)" == function ]]; then
  return
fi

sst_import_function \
;

sst_csf() {

  if (($# == 0)); then
    cat
    echo x
  else
    while (($# != 0)); do
      sst_expect_basic_identifier "$1"
      eval "$1=\${$1:0:-1}"
      eval "$1=\${$1%$'\n'}"
      shift
    done
  fi

}

readonly -f sst_csf
