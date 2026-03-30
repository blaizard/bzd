bzd_make_binary bzd_sandbox <<EOF
tmp="\${self}.tmp"
bzd_fetch "https://raw.githubusercontent.com/blaizard/bzd/master/sandbox.py" > "\$tmp"
chmod +x "\$tmp"
mv "\$tmp" "\$self"
exec "\$self" "\$@"
EOF
