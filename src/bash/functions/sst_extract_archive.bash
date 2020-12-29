#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_extract_archive)" == function ]]; then
  return
fi

sst_import_function \
;

sst_extract_archive() {

  sst_expect_argument_count $# 1

  local -r x="$1"
  local y

  case $x in
    *.tar)
      tar xf "$x"
    ;;
    *.tar.gz)
      tar xzf "$x"
    ;;
    *.tar.xz)
      tar xJf "$x"
    ;;
    *.zip)
      unzip "$x"
    ;;
    *)
      y=$(sst_quote "$x" | sst_csf)
      sst_csf y
      sst_barf 'unknown archive file extension: %s' "$y"
    ;;
  esac

}

readonly -f sst_extract_archive
