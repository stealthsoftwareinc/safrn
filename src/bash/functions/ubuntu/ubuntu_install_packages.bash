#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t \
ubuntu_install_packages)" != function ]]; then
ubuntu_install_packages() {

  local apt_get
  local v
  local y

  if [[ -t 0 ]]; then
    v=
    y=
  else
    v=DEBIAN_FRONTEND=noninteractive
    y=-y
  fi
  readonly v
  readonly y

  apt_get="$v apt-get -q $y"

  if ! command -v sudo &>/dev/null; then
    printf '%s update && %s install sudo\n' "$apt_get" "$apt_get"
    su -c "$apt_get update && $apt_get install sudo"
  fi

  apt_get="sudo $apt_get"

  printf '%s update\n' "$apt_get"
  eval "$apt_get update"

  printf '%s install %s\n' "$apt_get" "$*"
  eval "$apt_get install \"\$@\""

}; readonly -f ubuntu_install_packages; fi
