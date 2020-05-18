#!/bin/bash
set -e

cwd=`pwd`
result=`git submodule foreach --quiet 'echo $path $(git status --porcelain | wc -l)'`
for line in "$result"; do
    data=($line)
    if [ ${data[1]} -ne "0" ]; then
        cd "${data[0]}"
        git status
        read -p "${data[0]} is dirty, do you wish to commit changes? [N] " yn
        case $yn in
            [Yy]*)
                git add .
                git commit "$@"
                break;;
            * ) break;;
        esac
        cd "$cwd"
    fi
done

# Push all submodules
result=`git submodule foreach --quiet 'echo $path $(git diff origin/master..HEAD --name-only | wc -l)'`
for line in "$result"; do
    data=($line)
    if [ ${data[1]} -ne "0" ]; then
        cd "${data[0]}"
        echo "Pushing ${data[0]}..."
        git push
        cd "$cwd"
    fi
done

# Add current changes
git add .
git commit "$@"
git push
