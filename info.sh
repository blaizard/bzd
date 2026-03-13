#/bin/bash

echo "==== user ==============================================================="
echo "id: `id`"
echo "==== disk ==============================================================="
df -h
echo "==== root directory ====================================================="
ls -lla / || echo "Warning: Failed to list the root directory."
echo "==== current working directory =========================================="
pwd
ls -lla
echo "==== environment ========================================================"
printenv | sort
echo "==== docker ============================================================="
docker info
echo "==== git ================================================================"
git status
echo "==== bazel =============================================================="
./tools/bazel version
echo "==== buildstamp ========================================================="
./tools/bazel_build/buildstamp/stamp.sh
