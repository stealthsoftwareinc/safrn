#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_expect_source_path)" == function ]]; then
  return
fi

sst_import_function \
;

sst_expect_source_path() {

  local e
  local x

  for x; do

    e=

    case $x in

      "")
        e='source paths must not be empty'
      ;;

      *[!/A-Za-z0-9._-]*)
        e='source paths must only contain /A-Za-z0-9._- characters'
      ;;

      /*)
        e='source paths must not begin with a / character'
      ;;

      */)
        e='source paths must not end with a / character'
      ;;

      *//*)
        e='source paths must not contain repeated / characters'
      ;;

      . | ./* | */./* | */.)
        e='source paths must not contain any . components'
      ;;

      .. | ../* | */../* | */..)
        e='source paths must not contain any .. components'
      ;;

      -* | */-*)
        e='source path components must not begin with a - character'
      ;;

    esac

    if [[ "$e" != "" ]]; then
      x=$(jq_quote "$x")
      sst_barf '%s: %s' "$e" "$x"
    fi

  done

}

readonly -f sst_expect_source_path
