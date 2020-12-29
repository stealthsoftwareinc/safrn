#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t autogen_java_tests)" == function ]]; then
  return
fi

sst_import_function \
;

autogen_java_tests() {

  local ag
  local built_jardeps
  local java_target_slug
  local java_slug
  local dst
  local jcl
  local jcl_is_in_jardeps
  local package
  local slug

  autogen_java_jars "$@"

  for ag; do

    dst=$(jq -r .dst $ag)

    jq_expect_string .jcl $ag
    jcl=$(jq -r .jcl $ag)
    expect_safe_path "$jcl"
    case $jcl in
      ?*.jar)
      ;;
      *)
        sst_barf '%s: .jcl: expected *.jar: %s' $ag $jcl
      ;;
    esac
    case $jcl in
      */*)
        sst_barf '%s: .jcl: no slashes allowed: %s' $ag $jcl
      ;;
    esac

    jq_expect_strings .jardeps $ag
    jcl_is_in_jardeps=$(jq -r '
      .jardeps | map(sub("^.*/"; "")) | contains(["'$jcl'"])
    ' $ag)
    case $jcl_is_in_jardeps in
      false)
        sst_barf '%s: .jcl must appear in .jardeps' $ag
      ;;
    esac

    package=$(jq -r .package $ag)

    slug=$(jq -r '.slug | select(.)' $ag)
    case $slug in
      "")
        java_target_slug=java-main
        java_slug=java
      ;;
      *)
        java_target_slug=java-$slug
        java_slug=java_$slug
      ;;
    esac

    built_jardeps=$(jq -r '.built_jardeps | select(.) | .[]' $ag)

    autogen_am_append <<EOF

check-$java_target_slug: FORCE
check-$java_target_slug: $dst
	if test -f \$(javadir)/$jcl; then \
  jar=\$(javadir)/$jcl; \
elif test -f /usr/local/share/java/$jcl; then \
  jar=/usr/local/share/java/$jcl; \
else \
  jar=/usr/share/java/$jcl; \
fi; \\
\$(JAVA) -jar \$\$jar -cp $dst:\$(${java_slug}_CLASSPATH) -p $package -n '^.*\$\$'

check-java: FORCE
check-java: check-$java_target_slug

.PHONY: check-$java_target_slug
.PHONY: check-java
EOF

  done

}

readonly -f autogen_java_tests
