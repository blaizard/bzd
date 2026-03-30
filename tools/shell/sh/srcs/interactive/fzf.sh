# Install fzf, a general-purpose command-line fuzzy finder.
bzd_install_fzf()
{
	git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf
	~/.fzf/install
	eval "$(fzf --bash)"
}
