#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_ubuntu_get_distro_version)" == function ]]; then
  return
fi

sst_import_function \
;

sst_ubuntu_get_distro_version() {

  local version

  sst_expect_argument_count $# 0

  version=

  if [[ -f /etc/os-release ]]; then
    version=$(sed -n '
      /^VERSION_ID=/ {
        s/^VERSION_ID=//
        s/"//g
        s/^16.04$/16.04/p
        s/^18.04$/18.04/p
        s/^20.04$/20.04/p
        q
      }
    ' /etc/os-release)
  fi

  printf '%s\n' "$version"

}

readonly -f sst_ubuntu_get_distro_version
