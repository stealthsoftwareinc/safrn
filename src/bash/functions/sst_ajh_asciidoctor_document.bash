#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_ajh_asciidoctor_document)" == function ]]; then
  return
fi

sst_import_function \
;

sst_ajh_asciidoctor_document() {

  local adoc
  local ag_json
  local clean_rule
  local distribute
  local html
  local prefix
  local prereqs
  local s
  local seen
  local x
  local y

  for ag_json; do

    html=$(sst_expect_ag_json "$ag_json")
    sst_expect_extension $ag_json .html.ag.json

    adoc=${html%.html}.adoc
    sst_expect_any_file $adoc{,.ag.json,.ag,.ac,.am,.im.in,.in,.im}

    prefix=$(sst_get_prefix $ag_json)
    if [[ "$prefix" == "" ]]; then
      sst_barf 'document must have its own subdirectory: %s' $ag_json
    fi

    prereqs=$(sst_underscore_slug $html)_prereqs

    sst_jq_get_string_or_null .clean_rule $ag_json clean_rule
    # TODO: check that clean_rule is either "", mostlyclean, clean, distclean, or maintainer-clean

    sst_jq_get_boolean_or_null .distribute $ag_json distribute

    autogen_am_append <<EOF

#-----------------------------------------------------------------------
# $html
#-----------------------------------------------------------------------

$prereqs =
EOF

    if [[ "$distribute" == true ]]; then
      sst_am_distribute $html
      if [[ "$clean_rule" != "" ]]; then
        sst_warn '%s: ignoring clean_rule because distribute is true' $ag_json
      fi
      clean_rule=maintainer-clean
    elif [[ "$clean_rule" == "" ]]; then
      clean_rule=mostlyclean
    fi

    seen=
    for x in $prefix**/*{.ag.json,.ag,.ac,.am,.im.in,.in,.im,}; do
      sst_expect_source_path "$x"

      if [[ $x == $ag_json || $x == $html ]]; then
        continue
      fi

      case $x in
        *.ag.json)
          y=${x%.ag.json}
        ;;
        *.ag)
          y=${x%.ag}
        ;;
        *.ac)
          y=${x%.ac}
        ;;
        *.am)
          y=${x%.am}
        ;;
        *.im.in)
          y=${x%.im.in}
        ;;
        *.in)
          y=${x%.im}
        ;;
        *.im)
          y=${x%.in}
        ;;
        *)
          y=$x
        ;;
      esac

      if [[ " $seen " == *" $y "* ]]; then
        continue
      fi
      seen="$seen $y"

      case $x in
        *.ag.json)
          # TODO: sst_ajh_auto $x
        ;;
        *.ag)
          autogen_ag_include $x
        ;;
        *.ac)
          autogen_ac_include $x
          autogen_am_var_append_files $prereqs $x
        ;;
        *.am)
          autogen_am_include $x
          autogen_am_var_append_files $prereqs $x
        ;;
        *.im.in)
          autogen_ac_append <<<"]GATBPS_CONFIG_FILE([$y.im])["
          autogen_ac_append <<<"]GATBPS_CONFIG_LATER([$y])["
          autogen_am_var_append_files $prereqs $x
        ;;
        *.in)
          autogen_ac_append <<<"]GATBPS_CONFIG_FILE([$y])["
          autogen_am_var_append_files $prereqs $x
        ;;
        *.im)
          autogen_ac_append <<<"]GATBPS_CONFIG_LATER([$y])["
          autogen_am_var_append_files $prereqs $x
        ;;
        *)
          autogen_am_var_append_files $prereqs $x
          sst_am_distribute $x
        ;;
      esac

    done

    autogen_am_append <<EOF

$html: \$($prereqs)
	\$(GATBPS_V_ASCIIDOCTOR)\$(ASCIIDOCTOR) \$(ASCIIDOCTOR_HTML_FLAGS) -o \$@\$(TMPEXT).tmp $adoc
	\$(AM_V_at)mv -f \$@\$(TMPEXT).tmp \$@

.PHONY: $html/clean
$html/clean: FORCE
	-rm -f -r \$(@D) \$(@D)\$(TMPEXT).tmp*

$clean_rule-local: $html/clean

#-----------------------------------------------------------------------
EOF

  done

}

readonly -f sst_ajh_asciidoctor_document
