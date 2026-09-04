#!/usr/bin/env bash

set -o pipefail -o errexit

# Move to the directory of this script.
cd -- "$( dirname -- "${BASH_SOURCE[0]}" )"

DIRECTORY="./versions"
DOCKER_COMPOSE="./docker-compose.yml"

# Timeout in seconds for the docker compose up command, to prevent indefinite hangs.
TIMEOUT_S=600

# Check if the directory exists.
if [ ! -d "$DIRECTORY" ]; then
    echo "Error: Directory '$DIRECTORY' not found."
    exit 1
fi

# Get all the version directories that contain a docker compose file.
versions=()
while IFS= read -r entry; do
    version="${entry%/}"
    if [ -f "$DIRECTORY/$version/docker-compose.yml" ]; then
        versions+=("$version")
    fi
done < <(command ls -1F "$DIRECTORY/")

# Ensure that there are at least two versions to roll back.
file_count="${#versions[@]}"
if [ "$file_count" -lt 2 ]; then
    echo "Error: Directory '$DIRECTORY' contains less than 2 versions. Found $file_count."
    exit 1
fi

# Get the last two versions.
current="${versions[$((file_count - 1))]}"
previous="${versions[$((file_count - 2))]}"

# Revert the current version to the previous one
# and rollback the containers.
cp "$DIRECTORY/$previous/docker-compose.yml" "$DOCKER_COMPOSE"
if ! timeout "$TIMEOUT_S" docker compose --file "$DOCKER_COMPOSE" up -d; then

    # On failure, restore the current version.
    echo "Error: Failed to roll back to $previous"
    echo "Restoring $current..."

    cp "$DIRECTORY/$current/docker-compose.yml" "$DOCKER_COMPOSE"
    timeout "$TIMEOUT_S" docker compose --file "$DOCKER_COMPOSE" up -d

    exit 1
fi


# Delete the current version.
rm -rf "$DIRECTORY/$current"
echo "Rolled back to $previous"
