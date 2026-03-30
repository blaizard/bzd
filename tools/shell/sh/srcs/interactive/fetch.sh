bzd_make_binary "bzd_fetch" <<EOF
url="\$1"
output="\$2"

mkdir -p "\$(dirname "\$output")"
if command -v wget >/dev/null 2>&1; then
	wget -q --no-cache "\$url" -O "\$output"
elif command -v curl >/dev/null 2>&1; then
	curl -S -s -H "Cache-Control: no-cache, no-store" -H "Pragma: no-cache" -o "\$output" "\$url"
else
	echo "Cannot download, neither 'wget' nor 'curl' is available." >&2
	exit 1
fi
EOF
