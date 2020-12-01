#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# Ensure that the standard prelude chooses tmpdir to be inside the
# current directory. This ensures that we can use tmpdir with the
# --mount and --volume options of nested docker commands.
#
# We also need to be careful to do this in portable shell so that we
# maintain the Bash bootstrapping feature of the standard prelude.
#
# Note that tmpdir is not the same as TMPDIR. The former is the
# temporary directory that the standard prelude will be setting up, and
# the latter is specified by POSIX. The standard prelude will construct
# the former inside the latter when the latter is set.
#

TMPDIR=`pwd` || exit $?
readonly TMPDIR
export TMPDIR

#
#

set -e; . src/bash/preludes/standard.bash

#
# Turn on command tracing so we can see what's going on in the job log
# of the GitLab UI. This prepends a green colored "trace: " heading to
# each traced command, which is similar to how GitLab traces the direct
# job commands in green.
#

PS4='+ \[\e[0;32m\]\[\e[1m\]trace:\[\e[0m\] '
readonly PS4
set -x

#
# Make sure that `apt-get -y` is really noninteractive by default. See
# `man 7 debconf` on Debian (after `apt-get install debconf-doc`) or
# view it online at <https://manpages.debian.org/debconf.7>.
#

export DEBIAN_FRONTEND=noninteractive

#
# Log in to the GitLab Docker registry, if possible.
#

if command -v docker &>/dev/null; then
  x1=${CI_REGISTRY_USER:+x}
  x2=${CI_REGISTRY:+x}
  x3=${CI_REGISTRY_PASSWORD:+x}
  if [[ "$x1$x2$x3" == xxx ]]; then
    docker login \
      --username "$CI_REGISTRY_USER" \
      --password-stdin \
      "$CI_REGISTRY" \
      <<<"$CI_REGISTRY_PASSWORD" \
    ;
  fi
fi

#
# Determine whether we're running on an archivist runner.
#

if test -f /archivist.gitlab-username; then
  archivist=true
else
  archivist=false
fi
readonly archivist

case $archivist in
  true)
    u=$(cat /archivist.gitlab-username)
    docker login \
      --username "$u" \
      --password-stdin \
      registry.stealthsoftwareinc.com \
      </archivist.gitlab-password \
    ;
    unset u
  ;;
esac
