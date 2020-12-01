#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
sst_expect_ag_json)" != function ]]; then
sst_expect_ag_json() {

  expect_argument_count $# 1-2
  local ag_json="$1"
  local handler="${2-${FUNCNAME[1]}}"
  sst_expect_source_path "$ag_json"
  expect_c_identifier "$handler"
  if [[ $ag_json != *[!/].ag.json ]]; then
    barf 'filename must be of the form foo.ag.json: %s' $ag_json
  fi
  jq_expect_object . $ag_json
  local x
  sst_jq_get_string .handler $ag_json x
  if [[ "$x" != $handler ]]; then
    x=$(sst_jq_quote "$x")
    barf '%s: .handler: expected "%s" but got %s' $ag_json $handler "$x"
  fi
  sst_am_distribute $ag_json
  echo ${ag_json%.ag.json}

}; readonly -f sst_expect_ag_json; fi
