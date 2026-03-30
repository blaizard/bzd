# This file is auto-generated, do not edit as modifications will be overwritten.

# ---- Content from aliases.sh
# --- locale ------------------------------------------------------------------
export LANG="${LANG:-en_US.UTF-8}"
export LC_CTYPE="${LC_CTYPE:-en_US.UTF-8}"
export LC_COLLATE="${LC_COLLATE:-en_US.UTF-8}"

# --- ls ----------------------------------------------------------------------
alias ls='ls --color=auto -h'
alias ll='ls -lh'
alias lla='ls -lha'

# Remove green background on directories
export LS_COLORS=$LS_COLORS:'ow=1;34:'

# ---- grep -------------------------------------------------------------------
alias grep='grep --color=auto'
alias egrep='egrep --color=auto'
alias fgrep='fgrep --color=auto'

# ---- vim --------------------------------------------------------------------
type vim &> /dev/null
if [ $? -eq 0 ]; then
	# Vim as default
	alias vi=vim
	alias edit="vim"
	export EDITOR="${EDITOR:-vim}"
	git config --global core.editor "vim" 2> /dev/null
else
	# Vi as default
	alias edit='vi'
	export EDITOR="${EDITOR:-vi}"
	git config --global core.editor "vi" 2> /dev/null
fi

# Reload the configuration if needed
alias bzd_reload='. ~/.bashrc'

# ---- Content from path.sh
# Update PATH to include .bzd/bin/
mkdir -p "$HOME/.bzd/bin"
export PATH="$PATH:$HOME/.bzd/bin"

# Add a new binary
#
# Usage:
#   bzd_make_binary <name> <<EOF
#   <content of the binary>
#   EOF
function bzd_make_binary() {
	local name="$1"
	local binary_path="$HOME/.bzd/bin/$name"

	if [[ ! -f "$binary_path" ]]; then
		echo "#!/usr/bin/env bash" > "$binary_path"
		echo "set -euo pipefail" >> "$binary_path"
		echo "self=\$(readlink -f \${BASH_SOURCE[0]})" >> "$binary_path"
		cat >> "$binary_path"
		chmod +x "$binary_path"
	fi
}


# Variables to be used by the prompt.
__session_names=()

# Only output to interactive shell.
if [[ "$TERM" != "dumb" ]] && [[ $- == *i* ]]; then

# ---- Content from clean.sh
_bzd_free_size_kb()
{
    total_free=0;
    for i in $(df -k 2> /dev/null | egrep '^/dev' | awk '{print $4}'); do
        total_free=$(($total_free + $i));
    done
    echo ${total_free}
}

# Remove unused files from the computer, such as cache etc.
bzd_clean_disk()
{
    total_free_begin=$(_bzd_free_size_kb)

    echo "---- Clean docker images."
    docker system prune -af

    bazel_workspaces=$(find / -name '.*' -prune -false -o -name 'WORKSPACE' 2>/dev/null)
    for workspace in ${bazel_workspaces}; do
        path=$(dirname "${workspace}")
        echo "---- Clean bazel workspace ${path}."
        cd "${path}"
        bazel clean --ui_event_filters=-info,-stdout,-stderr --noshow_progress --expunge
    done

    total_free_end=$(_bzd_free_size_kb)
    total_free_saved=$(($total_free_end - $total_free_begin))
    echo "---- Saved ${total_free_saved}K bytes."
}

# ---- Content from fetch.sh
bzd_make_binary "bzd_fetch" <<EOF
url="\$1"
output="\$2"

mkdir -p "\$(dirname "\$output")"
if command -v wget >/dev/null 2>&1; then
	wget -q --no-cache "\$url" -O "\$output"
elif command -v curl >/dev/null 2>&1; then
	curl -S -s -H "Cache-Control: no-cache, no-store" -H "Pragma: no-cache" -o "\$output" "\$url"
else
	echo "Cannot download, neither 'wget' nor 'curl' is available." >&2
	exit 1
fi
EOF

# ---- Content from fzf.sh
# Install fzf, a general-purpose command-line fuzzy finder.
bzd_install_fzf()
{
	git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf
	~/.fzf/install
	eval "$(fzf --bash)"
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
        # Note: do not delete untracked files as they might be used (for local configuration for example).
        git clean -ffd
        if git submodule status >/dev/null 2>&1; then
            git submodule foreach --recursive "git reset --hard && git clean -ffd"
            git submodule sync --recursive
            git submodule update --init --recursive
        fi
    fi
}

# Automate git bisect
bzd_git_bisect()
{
    echo -n "Hash of a good commit: "
    read hashGood

    echo -n "Hash of a bad commit: "
    read hashBad

    echo -n "Command to run to test a commit: "
    read testCommit

    tmpFile=$(mktemp)
    trap '{ rm -f -- "${tmpFile}"; }' EXIT
    cat <<EOF > "${tmpFile}"
echo "================================================================================"
${testCommit}
EOF

    git bisect start
    git bisect bad ${hashBad}
    git bisect good ${hashGood}
    git bisect run bash "${tmpFile}"
    echo "== Summary ====================================================================="
    git bisect log
}

# ---- Content from prompt.sh
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

# ---- Content from sandbox.sh
bzd_make_binary bzd_sandbox <<EOF
tmp="\${self}.tmp"
bzd_fetch "https://raw.githubusercontent.com/blaizard/bzd/master/sandbox.py" > "\$tmp"
chmod +x "\$tmp"
mv "\$tmp" "\$self"
exec "\$self" "\$@"
EOF

# ---- Content from tmux.sh
# Show tmux sessions if any exist
if command -v tmux >/dev/null 2>&1; then
	sessions=$(tmux list-sessions -F '#S' 2>/dev/null)
	if [ -n "$sessions" ]; then
		printf 'tmux session(s): %s\n' "$(echo "$sessions" | paste -sd ', ' -)"
	fi

	# If we are inside a tmux session.
	if [ -n "$TMUX" ]; then
		__session_names+=("$(tmux display-message -p '#S')")
	fi
fi

# ---- Content from update.sh
# Auto update script 
bzd_update()
{
	bzd_fetch "https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh" ~/.bzd_update_temp
	rm -rfd ~/.bzd && mkdir -p ~/.bzd && mv ~/.bzd_update_temp ~/.bzd/bashrc.sh
	bzd_reload
}


fi
