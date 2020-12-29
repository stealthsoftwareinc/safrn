#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# .am_df_INPUT
#
# An optional file to which to write the list of .java files. The file
# will be suitable to be processed by the .am_df_INPUT.df makefile rule.
#
# jardeps
#
# An optional array of .jar file names that dst depends on. These files
# will be expected to exist in ${javadir} or /usr/local/share/java or
# /usr/share/java at build time and after installation.
#
# If you have makefile targets for some of these files, you can list the
# targets instead of just their file names. For example, you could list
# build-aux/downloads/foobar.jar instead of foobar.jar. A target x is
# detected by the existence of a corresponding x.ag.*, x.ac, or x.am
# file. When one or more of these targets are detected, several
# installation targets are generated:
#
#       install-java-$(slug)-jardeps-targets
#
#             Makes all targeted jardeps for dst.
#
#       install-java-$(slug)-jardeps
#
#             Installs all targeted jardeps for dst.
#
#       install-java-jardeps-targets
#
#             Makes all targeted jardeps for every dst,
#             even those for which noinst is true.
#
#       install-java-jardeps
#
#             Installs all targeted jardeps for every dst,
#             even those for which noinst is true.
#
# Do not use jardeps to list .jar dependencies that are built by this
# project. Use built_jardeps instead.
#
# built_jardeps
#
# An optional array of .jar makefile targets that dst depends on that
# are built by this project.
#

if [[ "$(type -t autogen_java_jars)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_java_jars() {

  local ag
  local am_df_INPUT
  local built_jardeps
  local dst
  local jardeps
  local java_slug
  local java_target_slug
  local javac_cp
  local javadoc_slug
  local noinst
  local package
  local package_dir
  local shadowpath
  local slug
  local sourcepath
  local x
  local xs
  local y
  local y1
  local y2
  local y2s
  local y3

  for ag; do

    expect_safe_path "$ag"
    case $ag in
      *.jar.ag.json)
      ;;
      *)
        sst_barf 'expected *.jar.ag.json: %s' $ag
      ;;
    esac

    jq_expect_object . $ag

    jq_expect_string_or_null .am_df_INPUT $ag
    am_df_INPUT=$(jq -r '.am_df_INPUT | select(.)' $ag)

    jq_expect_string .dst $ag
    dst=$(jq -r .dst $ag)
    expect_safe_path "$dst"
    case $dst in
      *.jar)
      ;;
      *)
        sst_barf '%s: .dst: expected *.jar: %s' $ag $dst
      ;;
    esac

    jq_expect_string_or_null .slug $ag
    slug=$(jq -r '.slug | select(.)' $ag)
    readonly slug

    jq_expect_strings_or_null .jardeps $ag
    jardeps=$(jq -r '.jardeps | select(.) | .[]' $ag)
    readonly jardeps

    jq_expect_strings_or_null .built_jardeps $ag
    built_jardeps=$(jq -r '.built_jardeps | select(.) | .[]' $ag)
    readonly built_jardeps

    javac_cp=
    x=$(basename $dst)
    printf '{@}javadir{@}/%s' "$x" >$dst.classpath.im.in
    printf '{@}abs_builddir{@}/%s' "$dst" >$dst.build_tree_classpath.im.in
    for x in $jardeps; do
      x=$(basename $x)
      case $javac_cp in
        ?*)
          javac_cp=$javac_cp\$\(CLASSPATH_SEPARATOR\)
        ;;
      esac
      javac_cp=$javac_cp\${javadir}/$x
      javac_cp=$javac_cp\$\(CLASSPATH_SEPARATOR\)
      javac_cp=$javac_cp/usr/local/share/java/$x
      javac_cp=$javac_cp\$\(CLASSPATH_SEPARATOR\)
      javac_cp=$javac_cp/usr/share/java/$x
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.build_tree_classpath.im.in
      printf '{@}javadir{@}/%s' "$x" >>$dst.build_tree_classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.build_tree_classpath.im.in
      printf '/usr/local/share/java/%s' "$x" >>$dst.build_tree_classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.build_tree_classpath.im.in
      printf '/usr/share/java/%s' "$x" >>$dst.build_tree_classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '{@}javadir{@}/%s' "$x" >>$dst.classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '/usr/local/share/java/%s' "$x" >>$dst.classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '/usr/share/java/%s' "$x" >>$dst.classpath.im.in
    done
    for x in $built_jardeps; do
      case $javac_cp in
        ?*)
          javac_cp=$javac_cp\$\(CLASSPATH_SEPARATOR\)
        ;;
      esac
      javac_cp=$javac_cp$x
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.build_tree_classpath.im.in
      printf '{@}abs_builddir{@}/%s' "$x" >>$dst.build_tree_classpath.im.in
      x=$(basename $x)
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '{@}javadir{@}/%s' "$x" >>$dst.classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '/usr/local/share/java/%s' "$x" >>$dst.classpath.im.in
      printf '@CLASSPATH_SEPARATOR_RAW_LATER@' >>$dst.classpath.im.in
      printf '/usr/share/java/%s' "$x" >>$dst.classpath.im.in
    done
    readonly javac_cp
    printf '\n' >>$dst.classpath.im.in

    case $slug in
      "")
        java_target_slug=java-main
        java_slug=java
        javadoc_slug=javadoc
      ;;
      *)
        java_target_slug=java-$slug
        java_slug=java_$slug
        javadoc_slug=javadoc_$slug
      ;;
    esac
    readonly java_target_slug
    readonly java_slug
    readonly javadoc_slug

    jq_expect_string '.sourcepath' $ag
    sourcepath=$(jq -r '.sourcepath' $ag)
    expect_safe_path "$sourcepath"

    jq_expect_string_or_null '.shadowpath' $ag
    shadowpath=$(jq -r '.shadowpath | select(.)' $ag)
    case $shadowpath in
      "")
        # Make $shadowpath/* glob to nothing.
        shadowpath=autogen
      ;;
      *)
        expect_safe_path "$shadowpath"
        case $sourcepath in
          $shadowpath*)
            sst_barf '%s: shadowpath must not be an initial substring of sourcepath' $ag
          ;;
        esac
        case $shadowpath in
          $sourcepath*)
            sst_barf '%s: sourcepath must not be an initial substring of shadowpath' $ag
          ;;
        esac
      ;;
    esac

    package=$(jq -r .package $ag)
    readonly package

    package_dir=$(echo $package | sed 's|\.|/|g')
    readonly package_dir

    noinst=$(jq -r 'select(.noinst == true) | "x"' $ag)
    readonly noinst

    autogen_ac_append <<EOF

]GATBPS_CONFIG_FILE([$dst.classpath.im])[
]GATBPS_CONFIG_LATER([$dst.classpath])[

]GATBPS_CONFIG_FILE([$dst.build_tree_classpath.im])[
]GATBPS_CONFIG_LATER([$dst.build_tree_classpath])[

EOF

    case $slug in
      ?*)
        autogen_ac_append <<EOF

]GATBPS_JAVA([$slug])[

EOF
      ;;
    esac

    autogen_am_append <<EOF
${java_slug}_CLASSPATH = $javac_cp
${java_slug}_dep =
$dst: $built_jardeps
${java_target_slug}: $dst.classpath
${java_target_slug}: $dst.build_tree_classpath
${java_slug}_dst = $dst
${java_slug}_nested =
${java_slug}_noinst = $noinst
${java_slug}_package = $package
${java_slug}_sourcepath = $sourcepath
${java_slug}_src =
${javadoc_slug}_src =
EXTRA_DIST += \$(${javadoc_slug}_src)
jar_classpath_files += $dst.classpath
EOF

    autogen_am_append <<EOF

install-$java_target_slug-jardeps-targets: FORCE
install-$java_target_slug-jardeps: FORCE
install-java-jardeps-targets: FORCE
install-java-jardeps: FORCE

.PHONY: install-$java_target_slug-jardeps-targets
.PHONY: install-$java_target_slug-jardeps
.PHONY: install-java-jardeps-targets
.PHONY: install-java-jardeps

install-java-jardeps-targets: install-$java_target_slug-jardeps-targets

install-java-jardeps: install-$java_target_slug-jardeps
EOF

    for x in $jardeps; do
      for y in $x.ag.* $x.ac $x.am; do
        if test -f $y; then
          autogen_am_append <<EOF

install-$java_target_slug-jardeps-targets: $x

install-$java_target_slug-jardeps-$x: FORCE
install-$java_target_slug-jardeps-$x: $x
	@\$(NORMAL_INSTALL)
	\$(MKDIR_P) '\$(DESTDIR)\$(javadir)'
	\$(INSTALL_DATA) $x '\$(DESTDIR)\$(javadir)'

.PHONY: install-$java_target_slug-jardeps-$x

install-$java_target_slug-jardeps: install-$java_target_slug-jardeps-$x
EOF
          break
        fi
      done
    done

    xs=$(jq -r 'select(.dep) | .dep[]' $ag)
    for x in $xs; do
      autogen_am_append <<EOF
${java_slug}_dep += $x
EOF
    done

    xs=$(find $sourcepath/$package_dir -type d)
    for x in $xs; do
      autogen_am_append <<EOF
${java_slug}_nested += $x/*\\\$\$*.class
EOF
    done

    case $am_df_INPUT in
      ?*)
        >$am_df_INPUT
      ;;
    esac

    y2s=:

    for y1 in \
      $shadowpath/$package_dir/**/*.java.ac \
      $shadowpath/$package_dir/**/*.java.ag \
      $shadowpath/$package_dir/**/*.java.am \
      $shadowpath/$package_dir/**/*.java.im \
      $shadowpath/$package_dir/**/*.java.im.in \
      $shadowpath/$package_dir/**/*.java.in \
      $sourcepath/$package_dir/**/*.java \
      $sourcepath/$package_dir/**/*.java.ac \
      $sourcepath/$package_dir/**/*.java.ag \
      $sourcepath/$package_dir/**/*.java.am \
      $sourcepath/$package_dir/**/*.java.im \
      $sourcepath/$package_dir/**/*.java.im.in \
      $sourcepath/$package_dir/**/*.java.in \
    ; do

      expect_safe_path "$y1"

      y1=$(echo $y1 | sed "s|^$shadowpath/|$sourcepath/|")

      case $y1 in
        *.java)
          y2=$y1
        ;;
        *.java.ac)
          y2=$(echo $y1 | sed 's/\.ac$//')
        ;;
        *.java.ag)
          y2=$(echo $y1 | sed 's/\.ag$//')
        ;;
        *.java.am)
          y2=$(echo $y1 | sed 's/\.am$//')
        ;;
        *.java.im)
          y2=$(echo $y1 | sed 's/\.im$//')
        ;;
        *.java.im.in)
          y2=$(echo $y1 | sed 's/\.im\.in$//')
        ;;
        *.java.in)
          y2=$(echo $y1 | sed 's/\.in$//')
        ;;
        *)
          sst_barf 'missing case'
        ;;
      esac

      case $y2s in
        *:$y2:*)
          continue
        ;;
      esac

      y3=$(echo $y2 | sed 's/\.java$/.class/')

      autogen_ac_append <<EOF

]GATBPS_JAVA_CLASS(
  [$y3],
  [mostlyclean])[

EOF

      autogen_am_append <<EOF
${javadoc_slug}_src += $y2
${java_slug}_src += $y3
EOF

      case $am_df_INPUT in
        ?*)
          cat >>$am_df_INPUT <<EOF
${javadoc_slug}_src += $y2
EOF
        ;;
      esac

      y2s=$y2s$y2:

    done

  done

}

readonly -f autogen_java_jars
