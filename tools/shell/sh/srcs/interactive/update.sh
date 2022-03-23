# Auto update script 
bzd-update()
{
    url="https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh"
	wget -q --no-cache "$url" -O ~/.bzd_update_temp
	rm -rfd ~/.bzd && mkdir -p ~/.bzd && mv ~/.bzd_update_temp ~/.bzd/bashrc.sh
	bzd_reload
}
