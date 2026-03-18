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
