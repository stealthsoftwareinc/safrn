#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_get_distro_version)" == function ]]; then
  return
fi

sst_import_function \
;

sst_get_distro_version() {

  local distro

  sst_expect_argument_count $# 0

  if [[ "${sst_distro_version+x}" != x ]]; then
    distro=$(sst_get_distro)
    sst_distro_version=$(sst_${distro}_get_distro_version)
    if [[ "$sst_distro_version" == "" ]]; then
      sst_barf 'unknown distro version'
    fi
  fi
  printf '%s\n' "$sst_distro_version"

}

readonly -f sst_get_distro_version
