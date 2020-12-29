#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_barf)" == function ]]; then
  return
fi

sst_import_function \
;

sst_barf() {

  set +x

  local file
  local func
  local i
  local line
  local x

  # The first concurrent call to sst_barf to create the sst_barf_lock
  # directory gets to print its message. We also let through any call
  # being made by the standard prelude before it's done setting up
  # sst_root_tmpdir.
  if [[ "${sst_root_tmpdir+x}" == "" ]] || \
     mkdir "$sst_root_tmpdir"/sst_barf_lock 2>/dev/null; then

    # Print the message.
    printf '%s: ' "$0" >&2
    if (($# == 0)); then
      printf 'unknown error' >&2
    else
      printf "$@" >&2
    fi
    printf '\n' >&2

    # Print the stack trace.
    i=0
    while x=$(caller $i); do
      while [[ "$x" ==  [[:blank:]]* ]]; do x=${x#?}; done
      line=${x%%[[:blank:]]*}
      while [[ "$x" == [![:blank:]]* ]]; do x=${x#?}; done
      while [[ "$x" ==  [[:blank:]]* ]]; do x=${x#?}; done
      func=${x%%[[:blank:]]*}
      while [[ "$x" == [![:blank:]]* ]]; do x=${x#?}; done
      while [[ "$x" ==  [[:blank:]]* ]]; do x=${x#?}; done
      file=$x
      printf '  at %s(%s:%s)\n' "$func" "$file" "$line" >&2
      i=$((i + 1))
    done

  else

    # Give the concurrent call that won the mkdir race some time to
    # finish printing. Just a little though, as this case will also
    # occur if only one call is made and it fails for some reason.
    sleep 1

  fi

  exit 1

}

readonly -f sst_barf
