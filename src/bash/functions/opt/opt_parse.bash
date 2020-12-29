#
# For the copyright information for this file, please search up the
# directory tree for the first COPYING file.
#

#
# opt_parse <argv> <sopts> <lopts> [<ctx>]
#
# Parse command line options like the GNU getopt_long function.
#
# argv should be the name of an array that holds the arguments from
# which to parse options.
#
# sopts should be a short option string as described by the GNU getopt
# function. The leading + feature and the : and :: argument specifiers
# are supported.
#
# lopts should be the name of an array that serves as the long options
# array described by the GNU getopt_long function. Each element should
# be a long option name without the leading -- and optionally followed
# by a = or == argument specifier. = means required_argument, == means
# optional_argument, and omitting them means no_argument.
#
# When long options are parsed, unique prefixes are always accepted as
# shorthand for the full option names.
#
# The POSIXLY_CORRECT variable is ignored. Whether to permute argv is
# completely controlled by whether sopts has a leading +.
#
# Various opt_* variables are used to keep state between calls. If
# desired, you can specify the ctx argument to change the prefix of
# these variable names to something other than opt_. This discussion
# generally refers to these variables by their default names.
#
# If the return value is zero, then the next option was parsed. opt_opt
# is set to the option name, including a leading - for short options or
# a leading -- for long options. When a long option is given as a unique
# prefix, opt_opt will be the prefix, not the full option name. opt_ret
# is set to ? if the option name is unknown or an ambiguous prefix, to :
# if a required or forbidden argument constraint is broken, or to the
# full option name if the option is given properly. In the last case,
# opt_has is set to + or - if the option does or does not have an
# argument, and if it does, opt_arg is set to the argument. When opt_ret
# is ?, the rest of the current argv element will be ignored by the next
# call.
#
# If the return value is nonzero, then there are no more options.
# opt_ind is set to the index of the first non-option element in argv,
# or to the number of elements in argv if there are no non-option
# elements. opt_has is set to + if the options terminator was used and
# improperly given an argument, or to - if not.
#
# After the function returns nonzero, the next call will start a new
# parsing session. You can also force the next call to start a new
# parsing session by setting opt_ret to the empty string.
#
# Standard error messages are printed if opt_err is unset or empty.
#

if [[ "$(type -t opt_parse)" == function ]]; then
  return
fi

sst_import_function \
;

opt_parse() {

  eval "
    [[ $1 == argv ]] || local -n argv=$1
    [[ $3 == lopts ]] || local -n lopts=$3
  "
  local sopts="$2"
  if [[ $# -ge 4 && $4 != opt_ ]]; then
    local -n opt_arg=$4arg
    local -n opt_cut=$4cut
    local -n opt_err=$4err
    local -n opt_has=$4has
    local -n opt_ind=$4ind
    local -n opt_pos=$4pos
    local -n opt_ret=$4ret
  fi

  # Start a new parsing session if opt_ret is unset or empty.
  if [[ "${opt_ret-}" == "" ]]; then
    opt_cut=()
    opt_ind=0
    opt_pos=0
  fi

  if ((opt_ind == ${#argv[@]})); then
    opt_ret=
    opt_has=-
    argv+=("${opt_cut[@]}")
    return 1
  fi

  local error=${opt_err:+:}\ printf
  local arg=${argv[$opt_ind]}
  local i n

  if ((opt_pos == 0)); then

    if [[ "$arg" != -?* ]]; then
      if [[ "$sopts" == +* ]]; then
        opt_ret=
        opt_has=-
        return 1
      fi
      for ((i = opt_ind + 1; i != ${#argv[@]}; ++i)); do
        arg=${argv[$i]}
        if [[ "$arg" == -?* ]]; then
          break
        fi
      done
      if ((i == ${#argv[@]})); then
        opt_ret=
        opt_has=-
        argv=(
          "${argv[@]:0:$opt_ind}"
          "${opt_cut[@]}"
          "${argv[@]:$opt_ind}"
        )
        return 1
      fi
      n=$((i - opt_ind))
      opt_cut+=("${argv[@]:$opt_ind:$n}")
      for ((; i != ${#argv[@]}; ++i)); do
        argv[$((i - n))]=${argv[$i]}
      done
      argv=("${argv[@]:0:$((${#argv[@]} - n))}")
    fi

    if [[ "$arg" == -- ]]; then
      opt_ret=
      opt_has=-
      opt_ind=$((opt_ind + 1))
      argv=(
        "${argv[@]:0:$opt_ind}"
        "${opt_cut[@]}"
        "${argv[@]:$opt_ind}"
      )
      return 1
    fi

    if [[ "$arg" == --=* ]]; then
      $error 'the options terminator forbids an argument\n' >&2
      opt_ret=
      opt_has=+
      opt_ind=$((opt_ind + 1))
      argv=(
        "${argv[@]:0:$opt_ind}"
        "${opt_cut[@]}"
        "${argv[@]:$opt_ind}"
      )
      return 1
    fi

    if [[ "$arg" == --* ]]; then

      opt_opt=${arg%%=*}

      local has_arg=
      for ((i = 0; i != ${#lopts[@]}; ++i)); do
        if [[ "--${lopts[$i]}" == "$opt_opt"*== ]]; then
          if [[ "$has_arg" != "" ]]; then
            $error 'ambiguous option: %s\n' "$opt_opt" >&2
            opt_ret=?
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_ret=--${lopts[$i]%==}
          has_arg=optional_argument
        elif [[ "--${lopts[$i]}" == "$opt_opt"*= ]]; then
          if [[ "$has_arg" != "" ]]; then
            $error 'ambiguous option: %s\n' "$opt_opt" >&2
            opt_ret=?
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_ret=--${lopts[$i]%=}
          has_arg=required_argument
        elif [[ "--${lopts[$i]}" == "$opt_opt"* ]]; then
          if [[ "$has_arg" != "" ]]; then
            $error 'ambiguous option: %s\n' "$opt_opt" >&2
            opt_ret=?
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_ret=--${lopts[$i]}
          has_arg=no_argument
        fi
      done

      case $has_arg in
        optional_argument)
          if [[ "$arg" == *=* ]]; then
            opt_has=+
            opt_arg=${arg#*=}
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_has=-
          opt_ind=$((opt_ind + 1))
          return 0
        ;;
        required_argument)
          if [[ "$arg" == *=* ]]; then
            opt_has=+
            opt_arg=${arg#*=}
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          if ((opt_ind + 1 == ${#argv[@]})); then
            $error 'option requires an argument: %s\n' "$opt_ret" >&2
            opt_ret=:
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_has=+
          opt_arg=${argv[$((opt_ind + 1))]}
          opt_ind=$((opt_ind + 2))
          return 0
        ;;
        no_argument)
          if [[ "$arg" == *=* ]]; then
            $error 'option forbids an argument: %s\n' "$opt_ret" >&2
            opt_ret=:
            opt_ind=$((opt_ind + 1))
            return 0
          fi
          opt_has=-
          opt_ind=$((opt_ind + 1))
          return 0
        ;;
      esac

      $error 'unknown option: %s\n' "$opt_opt" >&2
      opt_ret=?
      opt_ind=$((opt_ind + 1))
      return 0

    fi

    opt_pos=1

  fi

  opt_opt=-${arg:$opt_pos:1}
  opt_ret=$opt_opt
  if [[ "$sopts" == *"${arg:$opt_pos:1}"::* ]]; then
    if ((${#arg} > opt_pos + 1)); then
      opt_has=+
      opt_arg=${arg:$((opt_pos + 1))}
      opt_ind=$((opt_ind + 1))
      opt_pos=0
      return 0
    fi
    opt_has=-
    opt_ind=$((opt_ind + 1))
    opt_pos=0
    return 0
  elif [[ "$sopts" == *"${arg:$opt_pos:1}":* ]]; then
    if ((${#arg} > opt_pos + 1)); then
      opt_has=+
      opt_arg=${arg:$((opt_pos + 1))}
      opt_ind=$((opt_ind + 1))
      opt_pos=0
      return 0
    fi
    if ((opt_ind + 1 == ${#argv[@]})); then
      $error 'option requires an argument: %s\n' "$opt_opt" >&2
      opt_ret=:
      opt_ind=$((opt_ind + 1))
      opt_pos=0
      return 0
    fi
    opt_has=+
    opt_arg=${argv[$((opt_ind + 1))]}
    opt_ind=$((opt_ind + 2))
    opt_pos=0
    return 0
  elif [[ "$sopts" == *"${arg:$opt_pos:1}"* ]]; then
    opt_has=-
    if ((${#arg} == opt_pos + 1)); then
      opt_ind=$((opt_ind + 1))
      opt_pos=0
    else
      opt_pos=$((opt_pos + 1))
    fi
    return 0
  fi

  $error 'unknown option: %s\n' "$opt_opt" >&2
  opt_ret=?
  opt_ind=$((opt_ind + 1))
  opt_pos=0
  return 0

}

readonly -f opt_parse
