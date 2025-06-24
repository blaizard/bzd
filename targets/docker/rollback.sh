#!/usr/bin/env bash

set -o pipefail -o errexit

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
docker compose --file "$DOCKER_COMPOSE" up -d

# Delete the current file.
rm "$current"

echo "Rolled back to previous configuration: $previous"
