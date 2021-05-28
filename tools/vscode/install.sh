#!/bin/bash

SCRIPTPATH="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"
VSCODE_EXTENSION="$HOME/.vscode/extensions"

rm -rfd "${VSCODE_EXTENSION}/bdl"
cp -r "${SCRIPTPATH}/bdl" "${VSCODE_EXTENSION}"

# https://stackoverflow.com/questions/42116486/where-can-i-find-a-list-of-all-possible-keys-for-tm-themes-for-syntax-highlighti
# https://stackoverflow.com/questions/54356746/how-do-i-add-a-bare-bones-syntax-to-vscode
