_bzd_free_size_kb()
{
    total_free=0;
    for i in $(df -k --output=avail | tail -n +2); do
        total_free=$(($total_free + $i));
    done
    echo $((total_free * 1024))
}

# Check the disk space.
bzd_disk_space()
{
    # If there is no arguments use these directories.
    [ "$#" -eq 0 ] && set -- /home /var /opt /usr/local /tmp

    echo "---- Checking files larger than 500M in: $@"
    sudo find "$@" -type f -size +500M -printf "%s %p\n" 2>/dev/null \
        | sort -nr \
        | numfmt --to=iec --field=1

    echo "---- 30 largest directories in: $@"
    sudo du -xh "$@" 2>/dev/null | sort -hr | head -30
}

# Remove unused files from the computer, such as cache etc.
bzd_disk_clean()
{
    total_free_begin=$(_bzd_free_size_kb)

    echo "---- Clean docker images."
    docker system prune -af

    echo "---- Clean bazel cache."
    sudo rm -rfd ~/.cache/bazel ~/.cache/bazelisk ~/.bazel /tmp/_bazel_$USER

    if command -v apt-get >/dev/null 2>&1; then
        echo "---- Clean apt cache/old kernels."
        sudo apt-get clean
        sudo apt-get autoremove --purge -y
    fi

    echo "---- Remove old kernels."
    sudo journalctl --vacuum-time=7d

    echo "---- Truncate large logs."
    sudo find /var/log -type f -size +100M -exec truncate -s 0 {} \;

    echo "---- Clean crash dumps."
    sudo rm -rf /var/crash/*

    echo "---- Clean /tmp."
    sudo find /tmp -mindepth 1 -mtime +3 -delete

    echo "---- Clean thumbnails.."
    rm -rf ~/.cache/thumbnails/*

    echo "---- Clean pip cache.."
    rm -rf ~/.cache/pip

    if command -v npm >/dev/null 2>&1; then
        echo "---- Clean npm cache."
        npm cache clean --force 2>/dev/null
    fi

    echo "---- Clean cargo registry."
    rm -rf ~/.cargo/registry ~/.cargo/git

    if command -v bzd_sandbox >/dev/null 2>&1; then
        echo "---- Clean bzd_sandbox."
        bzd_sandbox clean
    fi

    if command -v go >/dev/null 2>&1; then
        echo "---- Clean go build cache."
        go clean -cache -modcache -testcache 2>/dev/null
    fi

    if command -v snap >/dev/null 2>&1; then
        echo "---- Clean snap."
        sudo rm -rfd /var/lib/snapd/cache/*
    fi

    if command -v flatpak >/dev/null 2>&1; then
        echo "---- Clean flatpak."
        flatpak uninstall --unused -y
    fi

    total_free_end=$(_bzd_free_size_kb)
    total_free_saved=$(($total_free_end - $total_free_begin))
    echo "---- Saved $(numfmt --to=iec $total_free_saved)."
}
