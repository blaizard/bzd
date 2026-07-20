# Set the prompt
_bzd_parse_git_branch()
{
	local branch
    branch=$(git branch --show-current 2>/dev/null)
    [ -z "$branch" ] && return

	local upstream
    upstream=$(git rev-parse --abbrev-ref @{u} 2>/dev/null)

	local extra
	if [ -n "$upstream" ]; then
        local distance
        distance=$(git rev-list --left-right --count "${upstream}...${branch}" 2>/dev/null)
        
        if [ -n "$distance" ]; then
            # 3. Use pure Bash to parse the numbers—no awk required!
            local behind ahead
            read -r behind ahead <<< "$distance"
            [ "$behind" -gt 0 ] && extra=" -$behind"
            [ "$ahead" -gt 0 ] && extra="$extra +$ahead"
        fi
    fi

	echo -en " (${branch}${extra})"
}

_bzd_last_non_zero_return_code()
{
	return_code="$?"
	if [ $return_code != 0 ]; then
		echo -n "x "
	fi
}

_bzd_prepend()
{
	[[ ! -v IN_NIX_SHELL ]] || echo -n "(nix-shell) "
}

_bzd_sessions()
{
	if [ ${#__session_names[@]} -gt 0 ]; then
		echo -n "(${__session_names[@]}) "
	fi
}

case "$TERM" in
xterm*|rxvt*|konsole*|tmux*)
	PS1="\$(_bzd_sessions)\[\e[0;31m\]\$(_bzd_last_non_zero_return_code)\[\e[0m\]\$(_bzd_prepend)\u@\h \[\e[32m\]\w\[\e[33m\]\$(_bzd_parse_git_branch)\[\e[0m\] $ "
	;;
*)
	;;
esac
