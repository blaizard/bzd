# Set the prompt
_bzd_parse_git_branch()
{
	branch=`git branch 2> /dev/null | sed -e '/^[^*]/d' -e 's/* \(.*\)/\1/'`
	if [ ! -z "${branch}" ]; then
		origin=`git for-each-ref --format='%(upstream:short)' $(git symbolic-ref -q HEAD) 2> /dev/null | head -n1`
		extra=""
		if [ ! -z "${origin}" ]; then
			distance=`git rev-list --left-right --count ${origin}...${branch} 2> /dev/null`
			if [ ! -z "${distance}" ]; then
				behind=`echo -n "${distance}" | awk '{print $1}'`
				ahead=`echo -n "${distance}" | awk '{print $2}'`
				if [ ${behind} -gt 0 ]; then extra=" -${behind}"; fi
				if [ ${ahead} -gt 0 ]; then extra="${extra} +${ahead}"; fi
			fi
		fi
		echo -en " (${branch}${extra})"
	fi
}

_bzd_last_non_zero_return_code()
{
	return_code="$?"
	if [ $return_code != 0 ]; then
		echo -en "\033[31mx\033[00m "
	fi
}

_bzd_prepend()
{
	[[ ! -v IN_NIX_SHELL ]] || echo -n "(nix-shell) "
}

case "$TERM" in
xterm*|rxvt*|konsole*)
	PS1="\$(_bzd_last_non_zero_return_code)\$(_bzd_prepend)\u@\h \[\033[32m\]\w\[\033[33m\]\$(_bzd_parse_git_branch)\[\033[00m\] $ "
	;;
*)
	;;
esac
