# This file is auto-generated, do not edit as modifications will be overwritten.

bzd_is_shell=`echo "$-" | grep i > /dev/null`

_bzd_echo()
{
    # Only output to interactive shell. This is important as some commands might need a clean stdout, for example rsync...
	if ${bzd_is_shell}; then
		echo "$@"
	fi
}

_bzd_echo -n "Setting-up bzd environment..."

# ---- Content from aliases.sh
# --- ls ----------------------------------------------------------------------
alias ls='ls --color=auto -h'
alias ll='ls -lh'
alias lla='ls -lha'

# ---- grep -------------------------------------------------------------------
alias grep='grep --color=auto'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'

# ---- vim --------------------------------------------------------------------
type vim &> /dev/null
if [ $? -eq 0 ]; then
	# Vim as default
	alias vi=vim
	alias edit='vim'
	git config --global core.editor "vim"
else
	# Vi as default
	alias edit='vi'
	git config --global core.editor "vi"
fi

# Reload the configuration if needed
alias bzd_reload='. ~/.bashrc'


# Only output to interactive shell.
if ${bzd_is_shell}; then

# Load command line fuzzy finder
if [ -f ~/.fzf.bash ]; then
	source ~/.fzf.bash
else
	_bzd_echo "Command line fuzzy finder (fzf) is not installed."
fi

# ---- Content from update.sh
# Auto update script 
bzd_update()
{
    url="https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh"
	wget "$url" -O "~/.bzd/bashrc.sh"
	bzd_reload
}

# ---- Content from git.sh
# Reset the current branch to its state in origin
bzd_git_reset()
{
    git_top_level=$(git rev-parse --show-toplevel)
    if [[ $git_top_level != $(pwd) ]]; then
        echo "This command can only be executed at git top level directory: $git_top_level"
        return 1
    fi

    origin=`git for-each-ref --format='%(upstream:short)' $(git symbolic-ref -q HEAD) 2> /dev/null | head -n1`
    read -p "Do you want to reset your local branch to '$origin' (all modification will be lost)? [y/N] " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git fetch --prune
        git reset --hard $origin
        git clean -fd
        git submodule update --init --recursive
    fi
}

# ---- Content from prompt.sh
# Set the prompt
bzd_parse_git_branch()
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
export PS1="\u@\h \[\033[32m\]\w\[\033[33m\]\$(bzd_parse_git_branch)\[\033[00m\] $ "


fi

_bzd_echo -e "\tdone"
