#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t autogen_java_programs)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_java_programs() {

  local arg
  local class
  local dst
  local jar
  local jar_base
  local schema
  local src

  for arg; do

#    schema=$(
#      jq -r '
#        select(type == "object") |
#        select(.schema == "autogen_java_program") |
#        1
#      ' $arg
#    )
#    case $schema in
#      "")
#        sst_barf
#      ;;
#    esac

    class=$(jq -r '.class | select(type == "string")' $arg)

    jar=$(jq -r '.jar | select(type == "string")' $arg)

    src=$(jq -r '.src | select(type == "string")' $arg)
    case $src in
      "")
        src=build-aux/autogen_java_program.src
      ;;
    esac

    dst=$(jq -r '.dst | select(type == "string")' $arg)
    case $dst in
      "")
        dst=$(echo $arg | sed 's/\.ag\.json$//')
      ;;
    esac

    jar_base=$(basename $jar)

    sed "
      s|%class%|$class|g
      s|%jar%|$jar|g
      s|%jar_base%|$jar_base|g
    " <$src >$dst.im.in

    cat >$dst.ac <<EOF
GATBPS_CONFIG_FILE([$dst.im])
GATBPS_CONFIG_LATER([$dst])
EOF

    cat >$dst.am <<EOF
bin_SCRIPTS += $dst
EOF

    autogen_ac_include $dst.ac
    autogen_am_include $dst.am

  done

}

readonly -f autogen_java_programs
