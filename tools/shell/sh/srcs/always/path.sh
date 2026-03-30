# Update PATH to include .bzd/bin/
mkdir -p "$HOME/.bzd/bin"
export PATH="$PATH:$HOME/.bzd/bin"

# Add a new binary
#
# Usage:
#   bzd_make_binary <name> <<EOF
#   <content of the binary>
#   EOF
function bzd_make_binary() {
	local name="$1"
	local binary_path="$HOME/.bzd/bin/$name"

	if [[ ! -f "$binary_path" ]]; then
		echo "#!/usr/bin/env bash" > "$binary_path"
		echo "set -euo pipefail" >> "$binary_path"
		echo "self=\$(readlink -f \${BASH_SOURCE[0]})" >> "$binary_path"
		cat >> "$binary_path"
		chmod +x "$binary_path"
	fi
}
