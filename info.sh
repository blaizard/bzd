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
echo "==== local python ======================================================="
python --version
echo "==== local java ========================================================="
java --version
echo "==== bazel =============================================================="
./tools/bazel version
echo "==== node ==============================================================="
echo "version: $(${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:node -- --version)"
echo "==== yarn ==============================================================="
echo "version: $(${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- --version)"
echo "cache:"
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- cache dir
