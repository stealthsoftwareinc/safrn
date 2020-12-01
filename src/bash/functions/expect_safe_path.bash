#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
expect_safe_path)" != function ]]; then
expect_safe_path() {

  local x

  for x; do
    case $x in
      *[!/A-Za-z0-9._-]* | -*)
        barf 'unsafe path: %s' "$x"
      ;;
    esac
  done

}; readonly -f expect_safe_path; fi
