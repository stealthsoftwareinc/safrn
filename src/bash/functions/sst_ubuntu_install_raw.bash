#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_ubuntu_install_raw)" == function ]]; then
  return
fi

sst_import_function \
;

sst_ubuntu_install_raw() {

  local apt_get

  if (($# == 0)); then
    return
  fi

  if [[ -t 0 ]]; then
    apt_get='apt-get -q'
  else
    apt_get='DEBIAN_FRONTEND=noninteractive apt-get -q -y'
  fi

  if ! command -v sudo >/dev/null; then
    if [[ "${sst_apt_get_is_updated+x}" == "" ]]; then
      printf '%s update && %s install sudo\n' "$apt_get" "$apt_get" >&2
      su -c "$apt_get update && $apt_get install sudo" >&2
      sst_apt_get_is_updated=x
    else
      printf '%s install sudo\n' "$apt_get" >&2
      su -c "$apt_get install sudo" >&2
    fi
  fi

  apt_get="sudo $apt_get"

  if [[ "${sst_apt_get_is_updated+x}" == "" ]]; then
    printf '%s update\n' "$apt_get" >&2
    eval "$apt_get"' update' >&2
    sst_apt_get_is_updated=x
  fi

  printf '%s install %s\n' "$apt_get" "$*" >&2
  eval "$apt_get"' install "$@"' >&2

}

readonly -f sst_ubuntu_install_raw
