#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t jq_expect_objects)" == function ]]; then
  return
fi

sst_import_function \
;

jq_expect_objects() {

  case $# in
    2)
    ;;
    *)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  jq_expect_types "$1" "$2" object

}

readonly -f jq_expect_objects
