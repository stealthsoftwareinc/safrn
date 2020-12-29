#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_install_utility)" == function ]]; then
  return
fi

sst_import_function \
;

sst_install_utility() {

  declare -g -A sst_utility_overrides
  declare -g -A sst_utility_prefixes
  declare -g -A sst_utility_programs
  declare -g -A sst_utility_suffixes

  local distro
  local override
  local utility

  if (($# == 0)); then
    return
  fi

  #
  # Allow environment variable overrides. For example, allow the GIT
  # environment variable to override the git utility.
  #

  for utility; do
    if [[ "${sst_utility_programs[$utility]+x}" == "" ]]; then
      override=$(sst_environment_slug <<<"$utility")
      eval override=\${$override-}
      if [[ "$override" != "" ]]; then
        sst_utility_overrides[$utility]=$override
        sst_utility_programs[$utility]=
      fi
    fi
  done

  #
  # Install any utilities that haven't already been installed or
  # overridden by environment variables.
  #

  distro=$(sst_get_distro)
  eval sst_${distro}_install_utility '"$@"'

  #
  # Define any wrapper functions that haven't already been defined.
  #
  # It's important that these wrapper functions behave nicely regardless
  # of whether set -e is enabled, as idioms that temporarily suspend the
  # set -e state, like "utility || s = $?" or "if utility; then", should
  # continue to behave as expected.
  #

  for utility; do
    if [[ "$(type -t $utility)" != function ]]; then
      eval $utility'() {
        if [[ "${sst_utility_overrides['$utility']+x}" != "" ]]; then
          eval " ${sst_utility_overrides['$utility']}" '\''"$@"'\''
        else
          eval " ${sst_utility_prefixes['$utility']} \
                 command \
                 ${sst_utility_programs['$utility']} \
                 ${sst_utility_suffixes['$utility']}" '\''"$@"'\''
        fi
      }; readonly -f '$utility
    fi
  done

}

readonly -f sst_install_utility
