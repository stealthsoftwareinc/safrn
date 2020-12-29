#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# array_contains <result> <array> <value>
#
# If <array> is unset, it will be considered to be an empty array
# instead of producing an unbound variable error.
#

if [[ "$(type -t array_contains)" == function ]]; then
  return
fi

sst_import_function \
;

array_contains() {

  #
  # Before Bash 4.4, "${x[@]}" causes an error when x is an empty array
  # and set -u is enabled. The workaround is to write ${x[@]+"${x[@]}"}
  # instead. See <https://stackoverflow.com/q/7577052>.
  #

  if (($# == 3)); then
    sst_expect_basic_identifier "$1"
    sst_expect_basic_identifier "$2"
    eval '
      local r'$1$2'=0
      local x'$1$2'
      for x'$1$2' in ${'$2'[@]+"${'$2'[@]}"}; do
        if [[ "$x'$1$2'" == "$3" ]]; then
          r'$1$2'=1
          break
        fi
      done
      '$1'=$r'$1$2'
    '
  else
    sst_expect_argument_count $# 3
  fi

}

readonly -f array_contains
