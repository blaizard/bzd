#/bin/bash

BAZEL_RUN='./tools/bazel run --ui_event_filters=-info,-warning,-stdout,-stderr --noshow_progress'

echo "==== user ==============================================================="
echo "id: `id`"
echo "==== disk ==============================================================="
df -h
echo "==== environment ========================================================"
printenv | sort
echo "==== docker ============================================================="
docker info
echo "==== bazel =============================================================="
./tools/bazel version
