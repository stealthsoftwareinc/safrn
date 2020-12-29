#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_jq_expect)" == function ]]; then
  return
fi

sst_import_function \
  sst_barf \
  sst_expect_argument_count \
;

sst_jq_expect() {

  local condition
  local result

  sst_expect_argument_count $# 1-

  condition=$1
  readonly condition

  result=$(jq " $condition")
  readonly result

  if [[ "$result" == true ]]; then
    :
  elif [[ "$result" == false ]]; then
    shift
    sst_barf "$@"
  else
    condition=$(sst_quote "$condition")
    result=$(sst_quote "$result")
    sst_barf '<condition> = %s output %s instead of true or false' "$condition" "$result"
  fi

}

readonly -f sst_jq_expect
