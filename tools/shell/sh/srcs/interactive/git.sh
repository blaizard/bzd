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
        git clean -fd
        git submodule update --init --recursive
    fi
}
