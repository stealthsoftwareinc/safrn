#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_centos_install_raw)" == function ]]; then
  return
fi

sst_import_function \
  sst_echo_eval \
  sst_get_distro_version \
  sst_quote \
;

sst_centos_install_raw() {

  local package
  local packages
  local version
  local yum

  if (($# == 0)); then
    return
  fi

  yum='yum -q'
  if [[ ! -t 0 ]]; then
    yum+=' -y'
  fi

  if ! command -v sudo >/dev/null; then
    sst_echo_eval "su -c '$yum install sudo'" >&2
  fi
  yum='sudo '$yum

  if [[ "${sst_centos_install_raw_one_time_setup+x}" != x ]]; then
    sst_echo_eval "$yum install epel-release" >&2
    version=$(sst_get_distro_version)
    case $version in
      8)
        sst_echo_eval "$yum install 'dnf-command(config-manager)'" >&2
        sst_echo_eval "$yum config-manager --set-enabled powertools" >&2
      ;;
    esac
    sst_centos_install_raw_one_time_setup=x
  fi

  packages=
  for package; do
    packages+=${packages:+ }$(sst_quote "$package")
  done
  sst_echo_eval "$yum install $packages" >&2

}

readonly -f sst_centos_install_raw
