#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# Re-execute the script with plain bash. This has two purposes.
#
# First, if the script uses a /bin/bash shebang and runs on a system
# where /bin/bash is an older version of Bash but plain bash maps to a
# newer version installed somewhere like /usr/local/bin/bash, then this
# re-executes the script with the newer version.
#
# Second, since this paragraph of code is actually written in portable
# shell, the script can use a /bin/sh shebang and still get re-executed
# with plain bash. This is more portable than using a /bin/bash shebang
# because /bin/sh exists more reliably than /bin/bash.
#

case ${SST_BASH_BOOTSTRAP-} in
  "")
    export SST_BASH_BOOTSTRAP=x
    case $# in
      0)
        bash - "$0"
        exit $?
      ;;
      *)
        bash - "$0" "$@"
        exit $?
      ;;
    esac
  ;;
esac

#
#

set -Eeuo pipefail

export LC_ALL=C

shopt -s \
  dotglob \
  globstar \
  nullglob \
;

#
# Load the functions.
#
# Before Bash 4.4, "${x[@]}" causes an error when x is an empty array
# and set -u is enabled. The workaround is to write ${x[@]+"${x[@]}"}
# instead. See <https://stackoverflow.com/q/7577052>.
#

x=${BASH_SOURCE[0]%/*/*}/functions/**/*.bash
eval '
  bash_function_files=()
  for x in '"${bash_function_files-$x}"'; do
    for y in ${bash_function_files[@]+"${bash_function_files[@]}"}; do
      if [[ "$x" == "$y" ]]; then
        continue 2
      fi
    done
    bash_function_files+=("$x")
    . "${x/#-/./-}"
  done
'
readonly bash_function_files

#
# Check for the functions needed by the rest of this prelude.
#

for x in \
  array_contains \
  barf \
  trap_append \
; do
  if [[ "$(type -t $x)" != function ]]; then
    printf '%s: missing function: %s\n' "$0" $x >&2
    exit 1
  fi
done

#
#

trap 'barf "command failed: %s" "$BASH_COMMAND"' ERR
trap 'readonly trap_entry_status=$?' EXIT

#
#

x=${BASH_SOURCE[0]%/*}/**/*.bash
eval '
  bash_prelude_files=()
  for x in '"${bash_prelude_files-$x}"'; do
    array_contains y bash_prelude_files "$x"
    if ((!y)); then
      bash_prelude_files+=("$x")
    fi
  done
'
readonly bash_prelude_files

#
# Set rundir to an absolute path to the directory from which the script
# was run.
#

rundir=$PWD
readonly rundir

#
# Set root_tmpdir to an absolute path to a directory to use for
# temporary files. The directory will be deleted when the script exits.
#
# Callers should not use root_tmpdir directly, as it will contain
# temporary files used by the prelude, so file name collisions are a
# risk. Instead, they should use tmpdir, which the prelude guarantees
# will be an empty directory somewhere under root_tmpdir.
#

if root_tmpdir=$(mktemp -d); then
  if [[ "$root_tmpdir" != /* ]]; then
    root_tmpdir=$PWD/$root_tmpdir
  fi
else
  n=10
  while ((--n >= 0)); do
    root_tmpdir=${TMPDIR:-/tmp}/$RANDOM$RANDOM
    if [[ "$root_tmpdir" != /* ]]; then
      root_tmpdir=$PWD/$root_tmpdir
    fi
    mkdir "$root_tmpdir" && break
  done
  if ((n < 0)); then
    barf 'unable to choose tmpdir'
  fi
fi
readonly root_tmpdir
chmod 700 "$root_tmpdir"
trap_append 'rm -f -r "$root_tmpdir" || :' EXIT
tmpdir="$root_tmpdir"

#
#

readonly SSH_SECRET_KEY
readonly SSH_PASSPHRASE

case ${SSH_SECRET_KEY:+x}${SSH_PASSPHRASE:+y} in

  x)
    barf 'SSH_SECRET_KEY set without SSH_PASSPHRASE'
  ;;

  y)
    barf 'SSH_PASSPHRASE set without SSH_SECRET_KEY'
  ;;

  xy)

    cat >"$tmpdir"/ssh_secret_key <<EOF
$SSH_SECRET_KEY
EOF

    cat >"$tmpdir"/ssh_passphrase <<EOF
$SSH_PASSPHRASE
EOF

    cat >"$tmpdir"/ssh_config <<EOF
IdentitiesOnly yes
PasswordAuthentication no
PreferredAuthentications publickey
StrictHostKeyChecking no
UserKnownHostsFile /dev/null
EOF

    chmod 400 \
      "$tmpdir"/ssh_secret_key \
      "$tmpdir"/ssh_passphrase \
      "$tmpdir"/ssh_config \
    ;

    x='^-----BEGIN .* PRIVATE KEY-----$'
    x=$(sed -n "1s/$x/x/p" "$tmpdir"/ssh_secret_key)
    if [[ "$x" == "" ]]; then
      barf 'invalid SSH_SECRET_KEY'
    fi

    if command -v sshpass &>/dev/null; then
      if command -v ssh-keygen &>/dev/null; then
        if ! {
          sshpass \
            -P assphrase \
            -f "$tmpdir"/ssh_passphrase \
            ssh-keygen \
            -y \
            -f "$tmpdir"/ssh_secret_key \
            &>/dev/null \
          ;
        }; then
          barf 'invalid SSH_SECRET_KEY or SSH_PASSPHRASE'
        fi
      fi
    fi

    eval ssh_tmpdir_$$=\$tmpdir
    readonly ssh_tmpdir_$$
    export ssh_tmpdir_$$

    GIT_SSH_COMMAND=' \
      sshpass \
        -P assphrase \
        -f "$ssh_tmpdir_'$$'"/ssh_passphrase \
        ssh \
        -F "$ssh_tmpdir_'$$'"/ssh_config \
        -o IdentityFile="$ssh_tmpdir_'$$'"/ssh_secret_key \
    '
    readonly GIT_SSH_COMMAND
    export GIT_SSH_COMMAND

  ;;
esac

tmpdir="$tmpdir"/caller
mkdir "$tmpdir"
