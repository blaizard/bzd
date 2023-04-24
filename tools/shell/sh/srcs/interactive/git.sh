# Reset the current branch to its state in origin
bzd_git_reset()
{
    git_top_level=$(git rev-parse --show-toplevel)
    if [[ $git_top_level != $(pwd) ]]; then
        echo "This command can only be executed at git top level directory: $git_top_level"
        return 1
    fi

    origin=`git for-each-ref --format='%(upstream:short)' $(git symbolic-ref -q HEAD) 2> /dev/null | head -n1`
    read -p "Do you want to reset your local branch to '$origin' (all modification will be lost)? [y/N] " -n 1 -r
    echo ""
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        git fetch --prune
        git reset --hard $origin
        # Note: do not delete untracked files as they might be used (for local configuration for example).
        git clean -fd
        git submodule update --init --recursive
    fi
}

# Automate git bisect
bzd_git_bisect()
{
    echo -n "Hash of a good commit: "
    read hashGood

    echo -n "Hash of a bad commit: "
    read hashBad

    echo -n "Command to run to test a commit: "
    read testCommit

    tmpFile=$(mktemp)
    trap '{ rm -f -- "${tmpFile}"; }' EXIT
    cat <<EOF > "${tmpFile}"
echo "================================================================================"
${testCommit}
EOF

    git bisect start
    git bisect bad ${hashBad}
    git bisect good ${hashGood}
    git bisect run bash "${tmpFile}"
    echo "== Summary ====================================================================="
    git bisect log
}
