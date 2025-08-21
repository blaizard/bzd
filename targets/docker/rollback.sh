#!/usr/bin/env bash

set -o pipefail -o errexit

# Move to the directory of this script.
cd -- "$( dirname -- "${BASH_SOURCE[0]}" )"

DIRECTORY="./docker-compose"
DOCKER_COMPOSE="./docker-compose.yml"

# Check if the directory exists.
if [ ! -d "$DIRECTORY" ]; then
    echo "Error: Directory '$DIRECTORY' not found."
    exit 1
fi

# Get the all files in the directory that match the pattern.
readarray -t files < <(command ls -1F "$DIRECTORY/"*.yml)

# Ensure that there are at least two files to roll back.
file_count="${#files[@]}"
if [ "$file_count" -lt 2 ]; then
    echo "Error: Directory '$DIRECTORY' contains less than 2 files. Found $file_count."
    exit 1
fi

# Get the last two files.
current="${files[$((file_count - 1))]}"
previous="${files[$((file_count - 2))]}"

# Revert the current file to the previous one
# and rollback the containers.
cp "$previous" "$DOCKER_COMPOSE"
if ! docker compose --file "$DOCKER_COMPOSE" up -d; then

    # On failure, restore the current file.
    echo "Error: Failed to roll back to $previous"
    echo "Restoring $current..."

    cp "$current" "$DOCKER_COMPOSE"
    docker compose --file "$DOCKER_COMPOSE" up -d

    exit 1
fi


# Delete the current file.
rm "$current"
echo "Rolled back to $previous"
