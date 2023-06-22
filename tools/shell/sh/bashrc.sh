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

# ---- Content from google_chrome.sh
bzd_google_chrome() {
    # Start chrome in an isolated sandbox and it will not affect the main chrome profile.
    google-chrome --disable-site-isolation-trials --disable-web-security --allow-file-access-from-files --user-data-dir=/tmp
}

# ---- Content from update.sh
# Auto update script 
bzd_update()
{
    url="https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh"
	wget -q --no-cache "$url" -O ~/.bzd_update_temp
	rm -rfd ~/.bzd && mkdir -p ~/.bzd && mv ~/.bzd_update_temp ~/.bzd/bashrc.sh
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
        # Note: do not delete untracked files as they might be used (for local configuration for example).
        git clean -fd
        git submodule update --init --recursive
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

# ---- Content from math.sh
# simple math extension to calculate from the command line
bzd_math() {
    py_code="from math import *; print(${@})"
    python -c "${py_code}"
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

case "$TERM" in
xterm*|rxvt*|konsole*)
	PS1="\u@\h \[\033[32m\]\w\[\033[33m\]\$(_bzd_parse_git_branch)\[\033[00m\] $ "
	;;
*)
	;;
esac


fi

_bzd_echo -e "\tdone"

# Load command line fuzzy finder
if [ -f ~/.fzf.bash ]; then
	source ~/.fzf.bash
else
	_bzd_echo "Command line fuzzy finder (fzf) is not installed."
fi
