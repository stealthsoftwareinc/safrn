#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t autogen_texinfo)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_texinfo() {

  local dd
  local x1
  local x2
  local x3
  local y1
  local y2
  local y2s

  case $# in
    0 | 1)
      sst_barf 'invalid argument count: %d' $#
    ;;
  esac

  x1=$1
  readonly x1
  expect_safe_path "$x1"
  case $x1 in
    *.texi)
    ;;
    *)
      sst_barf '$1 must end in .texi: %s' $x1
    ;;
  esac
  shift

  x2=$(echo $x1 | sed 's/\.texi$//')
  readonly x2

  x3=$(echo $x2 | sed 's/[^0-9A-Z_a-z]/_/g')
  readonly x3

  cat >$x2.am <<EOF
${x3}_TEXINFOS =
EOF

  for dd; do

    expect_safe_path "$dd"

    y2s=:

    for y1 in \
      $dd/*.css \
      $dd/*.texi \
      $dd/*.texi.in \
    ; do

      expect_safe_path "$y1"

      if [[ $y1 == $x1 ]]; then
        continue
      fi

      case $y1 in
        *.texi.in)
          y2=$(echo $y1 | sed 's/\.in$//')
        ;;
        *)
          y2=$y1
        ;;
      esac

      case $y2s in
        *:$y2:*)
          continue
        ;;
      esac

      printf '%s_TEXINFOS += %s\n' $x3 $y2 >>$x2.am

      y2s=$y2s$y2:

    done

  done

  autogen_am_include $x2.am

}

readonly -f autogen_texinfo
