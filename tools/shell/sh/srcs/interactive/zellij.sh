# Auto-start Zellij unless inside VS Code or already in a Zellij session
if [[ "$TERM_PROGRAM" != "vscode" ]] && [[ -z "$ZELLIJ" ]]; then
    if command -v zellij > /dev/null; then
        exec zellij
    fi
fi
