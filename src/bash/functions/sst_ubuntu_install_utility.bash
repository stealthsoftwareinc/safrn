#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_ubuntu_install_utility)" == function ]]; then
  return
fi

sst_import_function \
  sst_get_distro_version \
;

sst_ubuntu_install_utility() {

  declare -g -A sst_utility_prefixes
  declare -g -A sst_utility_programs
  declare -g -A sst_utility_suffixes

  local distro_version
  local package
  local -A packages
  local program
  local utility

  if (($# == 0)); then
    return
  fi

  distro_version=$(sst_get_distro_version)
  readonly distro_version

  for utility; do
    if [[ "${sst_utility_programs[$utility]+x}" == "" ]]; then
      case $utility:$distro_version in

        cat:16.04 | cat:18.04 | cat:20.04)
          program=cat
          package=coreutils
        ;;

        gawk:16.04 | gawk:18.04 | gawk:20.04)
          program=gawk
          package=gawk
        ;;

        git:16.04 | git:18.04 | git:20.04)
          program=git
          package=git
        ;;

        gpg1:16.04)
          program=gpg
          package=gnupg
        ;;

        gpg1:18.04 | gpg1:20.04)
          program=gpg1
          package=gnupg1
        ;;

        gpg2:16.04 | gpg2:18.04 | gpg2:20.04)
          program=gpg2
          package=gnupg2
        ;;

        jq:16.04 | jq:18.04 | jq:20.04)
          program=jq
          package=jq
        ;;

        make:16.04 | make:18.04 | make:20.04)
          program=make
          package=make
        ;;

        mv:16.04 | mv:18.04 | mv:20.04)
          program=mv
          package=coreutils
        ;;

        ssh:16.04 | ssh:18.04 | ssh:20.04)
          program=ssh
          package=openssh-client
        ;;

        ssh-keygen:16.04 | ssh-keygen:18.04 | ssh-keygen:20.04)
          program=ssh-keygen
          package=openssh-client
        ;;

        sshpass:16.04 | sshpass:18.04 | sshpass:20.04)
          program=sshpass
          package=sshpass
        ;;

        tar:16.04 | tar:18.04 | tar:20.04)
          program=tar
          package=tar
        ;;

        *)
          sst_barf 'missing install info for %s' "$FUNCNAME:$utility:$distro_version"
        ;;

      esac
      sst_utility_prefixes[$utility]=
      sst_utility_programs[$utility]=$program
      sst_utility_suffixes[$utility]=
      if [[ "$(type -t $program)" != file ]]; then
        packages[$package]=x
      fi
    fi
  done

  sst_ubuntu_install_raw "${!packages[@]}"

}

readonly -f sst_ubuntu_install_utility
