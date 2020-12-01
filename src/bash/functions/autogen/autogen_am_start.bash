#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# autogen_am_start [<file>]
#
# Start constructing an accumulative Automake source file in which to
# collect autogen output. <file> should specify a file that lives either
# in or below the current directory. If <file> is not given, it defaults
# to autogen.am.
#

if [[ "$(type -t \
autogen_am_start)" != function ]]; then
autogen_am_start() {

  case ${autogen_am_start_has_been_called+x} in
    ?*)
      barf 'autogen_am_start has already been called'
    ;;
  esac
  autogen_am_start_has_been_called=x
  readonly autogen_am_start_has_been_called

  case ${autogen_am_file+x} in
    ?*)
      barf 'autogen_am_file is already set'
    ;;
  esac

  case $# in
    0)
      autogen_am_file=autogen.am
    ;;
    1)
      autogen_am_file=$1
    ;;
    *)
      barf 'invalid argument count: %d' $#
    ;;
  esac
  protect_path autogen_am_file
  readonly autogen_am_file

  autogen_print_am_header >$autogen_am_file

  trap_append '
    case $trap_entry_status in
      0)
        case ${autogen_am_finish_has_been_called+x} in
          "")
            barf "you forgot to call autogen_am_finish"
          ;;
        esac
      ;;
    esac
  ' EXIT

  autogen_am_var_append EXTRA_DIST autogen

}; readonly -f autogen_am_start; fi
