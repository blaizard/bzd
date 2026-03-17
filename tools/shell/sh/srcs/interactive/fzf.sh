# Install fzf, a general-purpose command-line fuzzy finder.
bzd_fzf_install()
{
	git clone --depth 1 https://github.com/junegunn/fzf.git ~/.fzf
	~/.fzf/install
	eval "$(fzf --bash)"
}
