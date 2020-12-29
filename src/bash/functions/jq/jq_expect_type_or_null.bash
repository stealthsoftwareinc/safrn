#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t jq_expect_type_or_null)" == function ]]; then
  return
fi

sst_import_function \
;

jq_expect_type_or_null() {

  case $# in
    3)
    ;;
    *)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  case $3 in
    array | boolean | null | number | object | string)
    ;;
    *)
      sst_barf '$3: invalid type: %s' "$3"
    ;;
  esac

  sst_jq_expect "
    $1 | (type == \"null\" or type == \"$3\")
  " '%s: %s: expected %s or null' "$2" "$1" $3 <"$2"

}

readonly -f jq_expect_type_or_null
