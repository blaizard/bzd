# Auto update script 
bzd_update()
{
    url="https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh"
	if which wget >/dev/null; then
		wget -q --no-cache "$url" -O ~/.bzd_update_temp
	elif which curl >/dev/null; then
		curl -S -s -H "Cache-Control: no-cache, no-store" -H "Pragma: no-cache" -o ~/.bzd_update_temp "$url"
	else
		echo "Cannot download, neither wget nor curl is available."
		return 1
	fi
	rm -rfd ~/.bzd && mkdir -p ~/.bzd && mv ~/.bzd_update_temp ~/.bzd/bashrc.sh
	bzd_reload
}
