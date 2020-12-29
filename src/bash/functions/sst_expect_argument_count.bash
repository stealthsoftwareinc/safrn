#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_expect_argument_count)" == function ]]; then
  return
fi

sst_import_function \
;

sst_expect_argument_count() {
  local bad
  bad=1
  local regex_number
  regex_number='^(0|[1-9][0-9]*)$'
  local regex_rangemin
  regex_rangemin='^(0|[1-9][0-9]*)-$'
  local regex_range
  regex_range='^(0|[1-9][0-9]*)-(0|[1-9][0-9]*)$'
  local count
  local arg
  local min
  local max

  if (($# < 2)); then
    sst_barf 'bad argument count to sst_expect_argument_count itself: %s (expected 2-)' "$#"
  fi
  count=$1

  if [[ ! "$count" =~ $regex_number ]]; then
    sst_barf 'bad argument count syntax: "%s"' "$count"
  fi

  shift
  for arg; do
    if [[ "$arg" =~ $regex_number ]]; then
      if (($arg == $count)); then
        bad=0
      fi
    elif [[ "$arg" =~ $regex_rangemin ]]; then
      if ((${BASH_REMATCH[1]} <= $count)); then
        bad=0
      fi
    elif [[ "$arg" =~ $regex_range ]]; then
      min=${BASH_REMATCH[1]}
      max=${BASH_REMATCH[2]}
      if ((min > max)); then
        sst_barf 'bad argument count predicate syntax: "%s"' $arg
      fi
      if ((min <= count && count <= max)); then
        bad=0
      fi
    else
      sst_barf 'bad argument count syntax: "%s"' "$arg"
    fi
  done

  if ((bad)); then
    sst_barf 'bad argument count: %s (expected %s)' "$count" "$*"
  fi
}

readonly -f sst_expect_argument_count
