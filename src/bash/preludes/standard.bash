#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# Bootstrap into the PATH-resolved bash. Also unset SST_BASH_BOOTSTRAP
# so that we don't prevent other scripts from bootstrapping.
#

case ${SST_BASH_BOOTSTRAP+x}y$# in
  y0) SST_BASH_BOOTSTRAP=x bash - "$0"     ; exit $? ;;
  y*) SST_BASH_BOOTSTRAP=x bash - "$0" "$@"; exit $? ;;
esac
unset SST_BASH_BOOTSTRAP

#
#

set -e || exit

#
#

set -E -u -o pipefail

#
# Ensure that Bash's POSIX compatibility mode is disabled. This mode has
# no purpose for us, as we're intentionally using Bash, not merely using
# Bash as a realization of the POSIX shell. Failing to ensure this mode
# is disabled can lead to inconvenient behavior, such as the ability to
# use "-" characters in function names being disabled.
#

set +o posix

#
#

export LC_ALL=C

#
#

shopt -s \
  dotglob \
  globstar \
  nullglob \
;

#
# Set sst_root to an absolute path to the root directory of the Bash
# library. If the Bash library is rooted to the root system directory,
# sst_root will be the empty string. Note that this allows paths to be
# uniformly written like "$sst_root/foo".
#

sst_root=${BASH_SOURCE%/*/*}
if [[ "$sst_root" != "" && "$sst_root" != /* ]]; then
  sst_root=$PWD/$sst_root
fi
readonly sst_root
export sst_root

#
# Import the sst_import_function function.
#

. "$sst_root/functions/sst_import_function.bash"

#
# Import all functions needed by this prelude.
#

sst_import_function \
  sst_barf \
  sst_expect_basic_identifier \
  sst_install_utility \
  sst_quote \
  sst_trap_append \
  sst_warn \
;

#
# Import all functions.
#
# DEPRECATED: This behavior will eventually be removed. All new code
# should always use the sst_import_function function to import all
# necessary functions.
#

for x in "$sst_root/functions"/**/*.bash; do
  . "$x"
done

#
#

trap 'sst_barf "command failed: %s" "$BASH_COMMAND"' ERR
trap 'readonly trap_entry_status=$?' EXIT

#
# Set rundir to an absolute path to the directory from which we were
# run.
#

rundir=$PWD
readonly rundir

#
# If we're running in GitLab CI, force TMPDIR to be CI_BUILDS_DIR. For a
# Docker-based executor that forwards its Docker socket, this has a good
# chance at allowing TMPDIR to be mounted into other containers, as such
# an executor is probably configured to forward CI_BUILDS_DIR as well.
#

if [[ "${CI_BUILDS_DIR-}" != "" ]]; then
  export TMPDIR="$CI_BUILDS_DIR"
fi

#
# We want to provide the calling script with an absolute path to an
# empty directory that it can use for temporary files. However, this
# prelude and other preludes that wrap this prelude also need to use
# temporary files, so name collisions are a problem. To fix this, each
# prelude uses its temporary directory as needed, and before returning
# to the calling script (which may be a wrapping prelude), creates an
# empty temporary subdirectory for the calling script to use.
#

if tmpdir=$(mktemp -d); then
  if [[ "$tmpdir" != /* ]]; then
    tmpdir=$PWD/$tmpdir
  fi
else
  tmpdir=${TMPDIR:-/tmp}
  if [[ "$tmpdir" != /* ]]; then
    tmpdir=$PWD/$tmpdir
  fi
  mkdir -p "$tmpdir"
  n=10
  while ((n-- > 0)); do
    d=$(tr -d -c a-zA-Z0-9 </dev/urandom | head -c 10) || :
    d=$tmpdir/tmp${d:+.$d}.$BASHPID.$RANDOM
    mkdir "$d" || continue
    tmpdir=$d
    break
  done
  if ((n < 0)); then
    sst_barf 'failed to construct tmpdir'
  fi
fi
chmod 700 "$tmpdir"
readonly sst_root_tmpdir="$tmpdir"
sst_trap_append 'rm -f -r "$sst_root_tmpdir" || :' EXIT

#
# Set sst_interactive to indicate whether we're running interactively.
#

if test -t 0; then
  sst_interactive=1
else
  sst_interactive=0
fi

#
# Make sure "apt-get -y" is fully noninteractive when we're running
# noninteractively on Debian. See "man 7 debconf" (after running
# "apt-get install debconf-doc") or view it online at
# <https://manpages.debian.org/debconf.7>.
#

if ((!sst_interactive)); then
  export DEBIAN_FRONTEND=noninteractive
fi

#
# Log in to the GitLab Container Registry, if possible.
#

if [[ "${CI_REGISTRY+x}" != "" ]]; then
  if command -v docker >/dev/null; then
    docker login \
      --username "$CI_REGISTRY_USER" \
      --password-stdin \
      "$CI_REGISTRY" \
      <<<"$CI_REGISTRY_PASSWORD" \
      >/dev/null \
    ;
  fi
fi

#
# Set up our SSH credentials as specified by the SSH_SECRET_KEY and
# SSH_PASSPHRASE environment variables.
#
# If SSH_SECRET_KEY is unset or empty, no setup is performed. Otherwise,
# SSH_SECRET_KEY should be either the text of a secret key or a path to
# a secret key file, and SSH_PASSPHRASE should be the passphrase of the
# key. If the key has no passphrase, SSH_PASSPHRASE should be unset or
# empty.
#
# SSH_SECRET_KEY and SSH_PASSPHRASE can also be overridden by setting
# SSH_SECRET_KEY_VAR and SSH_PASSPHRASE_VAR to the names of different
# environment variables to use. For example, if your secret key is in
# MY_KEY, you can set SSH_SECRET_KEY_VAR=MY_KEY to use it. It may be
# unclear why you'd want to do this instead of just directly setting
# SSH_SECRET_KEY=$MY_KEY. Either approach will work, but the indirect
# approach is sometimes convenient for certain environments that may
# have challenging overriding behavior, such as GitLab CI.
#

if [[ "${SSH_SECRET_KEY_VAR-}" != "" ]]; then
  sst_expect_basic_identifier "$SSH_SECRET_KEY_VAR"
  eval SSH_SECRET_KEY=\$$SSH_SECRET_KEY_VAR
fi

if [[ "${SSH_PASSPHRASE_VAR-}" != "" ]]; then
  sst_expect_basic_identifier "$SSH_PASSPHRASE_VAR"
  eval SSH_PASSPHRASE=\$$SSH_PASSPHRASE_VAR
fi

if [[ "${SSH_SECRET_KEY-}" == "" ]]; then

  if [[ "${SSH_PASSPHRASE-}" != "" ]]; then
    sst_warn 'SSH_PASSPHRASE is set without SSH_SECRET_KEY'
  fi

else

  cat <<'EOF' >"$tmpdir"/ssh_config
IdentitiesOnly yes
PasswordAuthentication no
PreferredAuthentications publickey
StrictHostKeyChecking no
UserKnownHostsFile /dev/null
EOF
  chmod 400 "$tmpdir"/ssh_config

  if [[ "$SSH_SECRET_KEY" == ----* ]]; then
    cat <<<"$SSH_SECRET_KEY" >"$tmpdir"/ssh_secret_key
  else
    cat <"$SSH_SECRET_KEY" >"$tmpdir"/ssh_secret_key
  fi
  chmod 400 "$tmpdir"/ssh_secret_key

  if [[ "${SSH_PASSPHRASE-}" == "" ]]; then

    sst_install_utility ssh ssh-keygen

    if ! ssh-keygen -y -f "$tmpdir"/ssh_secret_key >/dev/null; then
      sst_barf 'invalid SSH_SECRET_KEY'
    fi

  else

    cat <<<"$SSH_PASSPHRASE" >"$tmpdir"/ssh_passphrase
    chmod 400 "$tmpdir"/ssh_passphrase

    sst_install_utility ssh ssh-keygen sshpass

    x=$(sst_quote "$tmpdir"/ssh_passphrase)
    sst_utility_suffixes[sshpass]+=' -f '$x
    sst_utility_suffixes[sshpass]+=' -P assphrase'

    if ! sshpass \
         ssh-keygen -y -f "$tmpdir"/ssh_secret_key >/dev/null; then
      sst_barf 'invalid SSH_SECRET_KEY or SSH_PASSPHRASE'
    fi

  fi

  x1=$(sst_quote "$tmpdir"/ssh_config)
  x2=$(sst_quote "$tmpdir"/ssh_secret_key)
  sst_utility_suffixes[ssh]+=' -F '$x1
  sst_utility_suffixes[ssh]+=' -o IdentityFile='$x2

  if [[ "${SSH_PASSPHRASE-}" != "" ]]; then
    sst_utility_suffixes[ssh]=" \
      ${sst_utility_suffixes[sshpass]} \
      ${sst_utility_programs[ssh]} \
      ${sst_utility_suffixes[ssh]} \
    "
    sst_utility_programs[ssh]=${sst_utility_programs[sshpass]}
    sst_utility_prefixes[ssh]+=${sst_utility_prefixes[sshpass]}
  fi

  #
  # Set and export GIT_SSH_COMMAND instead of prepending it to
  # ${sst_utility_prefixes[git]} so that git commands run by other
  # scripts will also use our SSH credentials. Note that git does not
  # necessarily need to be installed here, as we're simply setting an
  # environment variable that git will use if it is in fact installed.
  #

  export GIT_SSH_COMMAND=" \
    ${sst_utility_prefixes[ssh]} \
    command \
    ${sst_utility_programs[ssh]} \
    ${sst_utility_suffixes[ssh]} \
  "

fi

#
# Set up our GPG credentials as specified by the GPG_SECRET_KEY and
# GPG_PASSPHRASE environment variables.
#
# If GPG_SECRET_KEY is unset or empty, no setup is performed. Otherwise,
# GPG_SECRET_KEY should be either the text of a secret key or a path to
# a secret key file, and GPG_PASSPHRASE should be the passphrase of the
# key. If the key has no passphrase, GPG_PASSPHRASE should be unset or
# empty.
#
# GPG_SECRET_KEY and GPG_PASSPHRASE can also be overridden by setting
# GPG_SECRET_KEY_VAR and GPG_PASSPHRASE_VAR to the names of different
# environment variables to use. The behavior and rationale for these
# overrides are the same as for the analogous SSH_* overrides.
#

if [[ "${GPG_SECRET_KEY_VAR-}" != "" ]]; then
  sst_expect_basic_identifier "$GPG_SECRET_KEY_VAR"
  eval GPG_SECRET_KEY=\$$GPG_SECRET_KEY_VAR
fi

if [[ "${GPG_PASSPHRASE_VAR-}" != "" ]]; then
  sst_expect_basic_identifier "$GPG_PASSPHRASE_VAR"
  eval GPG_PASSPHRASE=\$$GPG_PASSPHRASE_VAR
fi

if [[ "${GPG_SECRET_KEY-}" == "" ]]; then

  if [[ "${GPG_PASSPHRASE-}" != "" ]]; then
    sst_warn 'GPG_PASSPHRASE is set without GPG_SECRET_KEY'
  fi

else

  sst_install_utility git gpg2

  mkdir "$tmpdir"/gpg_home
  chmod 700 "$tmpdir"/gpg_home

  x=$(sst_quote "$tmpdir"/gpg_home)
  sst_utility_suffixes[gpg2]+=' --batch'
  sst_utility_suffixes[gpg2]+=' --homedir '$x
  sst_utility_suffixes[gpg2]+=' --no-tty'
  sst_utility_suffixes[gpg2]+=' --quiet'

  #
  # The --pinentry-mode option was added in GnuPG 2.1, so we can't use
  # it in GnuPG 2.0.x. The exact commit in the GnuPG Git repository is
  # b786f0e12b93d8d61eea18c934f5731fe86402d3.
  #

  x=$(gpg2 --version | sed -n '1s/^[^0-9]*//p')
  if [[ "$x" != 2.0* ]]; then
    sst_utility_suffixes[gpg2]+=' --pinentry-mode loopback'
  fi

  if [[ "$GPG_SECRET_KEY" == ----* ]]; then
    cat <<<"$GPG_SECRET_KEY" >"$tmpdir"/gpg_secret_key
  else
    cat <"$GPG_SECRET_KEY" >"$tmpdir"/gpg_secret_key
  fi
  chmod 400 "$tmpdir"/gpg_secret_key
  gpg2 --import "$tmpdir"/gpg_secret_key

  if [[ "${GPG_PASSPHRASE-}" != "" ]]; then
    cat <<<"$GPG_PASSPHRASE" >"$tmpdir"/gpg_passphrase
    chmod 400 "$tmpdir"/gpg_passphrase
    x=$(sst_quote "$tmpdir"/gpg_passphrase)
    sst_utility_suffixes[gpg2]+=' --passphrase-file='$x
  fi

  cat <<EOF >"$tmpdir"/gpg_program
#! /bin/sh -
case \$# in
  0) ${sst_utility_prefixes[gpg2]} \
     ${sst_utility_programs[gpg2]} \
     ${sst_utility_suffixes[gpg2]}      ; exit \$? ;;
  *) ${sst_utility_prefixes[gpg2]} \
     ${sst_utility_programs[gpg2]} \
     ${sst_utility_suffixes[gpg2]} "\$@"; exit \$? ;;
esac
EOF
  chmod +x "$tmpdir"/gpg_program
  x=$(sst_quote "$tmpdir"/gpg_program)
  sst_utility_suffixes[git]+=' -c gpg.program='$x

  r='[0-9A-Fa-f]'
  r="[ 	]*$r$r$r$r"
  r="$r$r$r$r$r$r$r$r$r$r"
  x=$(gpg2 --fingerprint | sed -n '
    /'"$r"'/ {
      s/.*\('"$r"'\).*/\1/
      s/[ 	]//g
      p
      q
    }
  ')
  sst_utility_suffixes[git]+=' -c user.signingKey=0x'$x
  sst_utility_suffixes[git]+=' -c commit.gpgSign=true'
  sst_utility_suffixes[git]+=' -c tag.gpgSign=true'

fi

#
# Set up our Git name and email.
#
# These variables can be overridden by using the standard GIT_AUTHOR_*
# and GIT_COMMITTER_* environment variables. For more information, see
# "man git-commit" and "man git-commit-tree", or view them online at
# <https://git-scm.com/docs/git-commit> and
# <https://git-scm.com/docs/git-commit-tree>.
#
# GIT_AUTHOR_* and GIT_COMMITTER_* can be further overridden by setting
# GIT_AUTHOR_*_VAR and GIT_COMMITTER_*_VAR to the names of different
# environment variables to use. The behavior and rationale for these
# overrides are the same as for the analogous SSH_* overrides.
#

for x in \
  GIT_AUTHOR_DATE \
  GIT_AUTHOR_EMAIL \
  GIT_AUTHOR_NAME \
  GIT_COMMITTER_DATE \
  GIT_COMMITTER_EMAIL \
  GIT_COMMITTER_NAME \
; do

  #
  # Override $x with ${x}_VAR if it's set.
  #

  eval y=\${${x}_VAR+x}
  if [[ "$y" != "" ]]; then
    eval y=\${${x}_VAR}
    sst_expect_basic_identifier "$y"
    eval $x=\$$y
  fi

  #
  # Ensure that $x is exported if it's set.
  #

  eval y=\${$x+x}
  if [[ "$y" != "" ]]; then
    export $x
  fi

done

#
# If we're in a GitLab CI job, fill in various unset GIT_* environment
# variables using the job information.
#

if [[ "${CI_JOB_URL-}" != "" ]]; then
  if [[ "${GIT_AUTHOR_EMAIL+x}" == "" ]]; then
    export GIT_AUTHOR_EMAIL="$GITLAB_USER_EMAIL"
  fi
  if [[ "${GIT_AUTHOR_NAME+x}" == "" ]]; then
    export GIT_AUTHOR_NAME="$GITLAB_USER_NAME"
  fi
  if [[ "${GIT_COMMITTER_EMAIL+x}" == "" ]]; then
    export GIT_COMMITTER_EMAIL=""
  fi
  if [[ "${GIT_COMMITTER_NAME+x}" == "" ]]; then
    export GIT_COMMITTER_NAME="$CI_JOB_URL"
  fi
fi

#
# Create an empty temporary subdirectory for the calling script to use.
#

tmpdir=$tmpdir/x
mkdir "$tmpdir"
