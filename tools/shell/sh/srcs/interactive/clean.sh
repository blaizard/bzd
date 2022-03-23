_bzd_free_size_kb()
{
    total_free=0;
    for i in $(df -k 2> /dev/null | egrep '^/dev' | awk '{print $4}'); do
        total_free=$(($total_free + $i));
    done
    echo ${total_free}
}

# Remove unused files from the computer, such as cache etc.
clean-disk()
{
    total_free_begin=$(_bzd_free_size_kb)

    echo "---- Clean docker images."
    docker system prune -af

    bazel_workspaces=$(find / -name '.*' -prune -false -o -name 'WORKSPACE' 2>/dev/null)
    for workspace in ${bazel_workspaces}; do
        path=$(dirname "${workspace}")
        echo "---- Clean bazel workspace ${path}."
        cd "${path}"
        bazel clean --ui_event_filters=-info,-stdout,-stderr --noshow_progress --expunge
    done

    total_free_end=$(_bzd_free_size_kb)
    total_free_saved=$(($total_free_end - $total_free_begin))
    echo "---- Saved ${total_free_saved}K bytes."
}
