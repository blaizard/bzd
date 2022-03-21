# Auto update script 
bzd_update()
{
    url="https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh"
	wget -q "$url" -O ~/.bzd/bashrc.sh
	bzd_reload
}
