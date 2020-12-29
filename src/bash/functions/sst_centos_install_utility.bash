#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_centos_install_utility)" == function ]]; then
  return
fi

sst_import_function \
  sst_get_distro_version \
;

sst_centos_install_utility() {

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

        cat:6 | cat:7 | cat:8)
          program=cat
          package=coreutils
        ;;

        gawk:6 | gawk:7 | gawk:8)
          program=gawk
          package=gawk
        ;;

        git:6 | git:7)
          program=git
          package=git
        ;;

        git:8)
          program=git
          package=git-core
        ;;

        gpg2:6 | gpg2:7 | gpg2:8)
          program=gpg2
          package=gnupg2
        ;;

        jq:6 | jq:7 | jq:8)
          program=jq
          package=jq
        ;;

        make:6 | make:7 | make:8)
          program=make
          package=make
        ;;

        mv:6 | mv:7 | mv:8)
          program=mv
          package=coreutils
        ;;

        ssh:6 | ssh:7 | ssh:8)
          program=ssh
          package=openssh-clients
        ;;

        ssh-keygen:6 | ssh-keygen:7 | ssh-keygen:8)
          program=ssh-keygen
          package=openssh
        ;;

        sshpass:6 | sshpass:7 | sshpass:8)
          program=sshpass
          package=sshpass
        ;;

        tar:6 | tar:7 | tar:8)
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

  sst_centos_install_raw "${!packages[@]}"

}

readonly -f sst_centos_install_utility
