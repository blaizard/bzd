#!/bin/bash
set -e

result=`git submodule foreach --quiet 'echo $path $(git status --porcelain | wc -l)'`
for line in "$result"; do
    data=($line)
    if [ ${data[1]} -ne "0" ]; then
        echo "${data[0]} is dirty"
        # cd "${data[0]}"
        # git commit $@
        # git push
    fi
done
