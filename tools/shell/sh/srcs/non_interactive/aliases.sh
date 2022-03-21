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
