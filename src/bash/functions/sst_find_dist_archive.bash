#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

if [[ "$(type -t sst_find_dist_archive)" == function ]]; then
  return
fi

sst_import_function \
;

sst_find_dist_archive() {

  local -r dir="$sst_root_tmpdir/$FUNCNAME.$BASHPID"
  local archive
  local n
  local s
  local similar
  local wc_x
  local wc_y
  local x
  local y

  sst_expect_argument_count $# 0

  for archive in *.tar.gz; do

    # (Re)create our temporary directory.
    rm -f -r "$dir"
    mkdir "$dir"

    # Extract the archive into our temporary directory.
    (cd "$dir" && tar xz) <"$archive"

    # The archive should contain exactly one root entity.
    n=$(cd "$dir" && find . '!' -name . -prune -print | grep -c /)
    if ((n != 1)); then
      continue
    fi

    # The root entity should be a directory.
    s=0
    test -d "$dir"/* || s=$?
    if ((s == 1)); then
      continue
    fi
    if ((s != 0)); then
      sst_barf 'test -d failed'
    fi

    # The directory should contain a few files that match our own. We
    # avoid using the cmp utility because it's not available on some
    # systems (e.g., on the centos:8 Docker image). The wc utility
    # should be good enough.
    for x in configure.ac Makefile.am; do
      similar=0
      for y in "$dir"/*/"$x"; do
        wc_x=$(wc <"$x")
        wc_y=$(wc <"$y")
        if [[ "$wc_x" == "$wc_y" ]]; then
          similar=1
        fi
      done
      if ((!similar)); then
        continue 2
      fi
    done

    # We found it.
    rm -f -r "$dir"
    printf '%s\n' "$archive"
    return

  done

  sst_barf 'distribution archive not found'

}

readonly -f sst_find_dist_archive
