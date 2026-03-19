# Show tmux sessions if any exist
if command -v tmux >/dev/null 2>&1; then
	sessions=$(tmux list-sessions -F '#S' 2>/dev/null)
	if [ -n "$sessions" ]; then
		printf 'tmux session(s): %s\n' "$(echo "$sessions" | paste -sd ', ' -)"
	fi

	# If we are inside a tmux session.
	if [ -n "$TMUX" ]; then
		__session_names+=("$(tmux display-message -p '#S')")
	fi
fi
