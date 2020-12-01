#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# protect_path <var>
#

if [[ "$(type -t \
protect_path)" != function ]]; then
protect_path() {

  local var
  local x

  case $# in
    1)
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac

  var=$1
  readonly var
  expect_c_identifier "$var"

  eval x=\$$var
  case $x in
    -*)
      eval $var=./\$x
    ;;
  esac

}; readonly -f protect_path; fi
