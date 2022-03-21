bzd_is_shell=`echo "$-" | grep i > /dev/null`

_bzd_echo()
{
	if ${bzd_is_shell}; then
		echo "$@"
	fi
}

# Only output to interactive shell. This is important as some commands might need a clean stdout, for example rsync...
_bzd_echo -n "Setting-up bzd environment..."

# Import aliases
source ~/.bzd/aliases.sh

# Only output to interactive shell.
if ${bzd_is_shell}; then

	# Load command line fuzzy finder
	if [ -f ~/.fzf.bash ]; then
		source ~/.fzf.bash
	else
		_bzd_echo "Command line fuzzy finder (fzf) is not installed."
	fi

	source ~/.bzd/prompt.sh
	source ~/.bzd/git.sh
fi

_bzd_echo -e "\tdone"
