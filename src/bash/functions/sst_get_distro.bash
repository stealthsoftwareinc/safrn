#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_get_distro)" == function ]]; then
  return
fi

sst_import_function \
;

sst_get_distro() {

  local s
  local id

  sst_expect_argument_count $# 0

  if [[ "${sst_get_distro_cache-}" != "" ]]; then
    echo $sst_get_distro_cache
    return
  fi

  #
  # Note that the uname utility isn't very useful here because it
  # returns host information when used in a Docker container.
  #

  if [[ -f /etc/os-release ]]; then

    id=$(
      sed -n '
        /^ID=/ {
          s/^ID=//
          s/"//g
          p
          q
        }
      ' /etc/os-release
    )

    case $id in
      alpine | \
      arch | \
      centos | \
      debian | \
      fedora | \
      rhel | \
      ubuntu | \
      $'\n')
        sst_get_distro_cache=$id
        echo $sst_get_distro_cache
        return
      ;;
    esac

  fi

  if command -v cygcheck >/dev/null; then
    sst_get_distro_cache=cygwin
    echo $sst_get_distro_cache
    return
  fi

  sst_get_distro_cache=unknown
  echo $sst_get_distro_cache

}

readonly -f sst_get_distro
