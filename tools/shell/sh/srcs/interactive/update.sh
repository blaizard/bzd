# Auto update script 
bzd_update()
{
	bzd_fetch "https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh" ~/.bzd_update_temp
	rm -rfd ~/.bzd/bin && mkdir -p ~/.bzd && mv ~/.bzd_update_temp ~/.bzd/bashrc.sh
	bzd_reload
}
