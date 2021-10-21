#/bin/bash

BAZEL_RUN='./tools/bazel run --ui_event_filters=-info,-warning,-stdout,-stderr --noshow_progress'

echo "---- environment --------------------------------------------------------"
echo -n "id: `id`"
printenv | sort
df -h
echo "---- docker -------------------------------------------------------------"
docker info
echo "---- local python -------------------------------------------------------"
python --version
echo "---- local java ---------------------------------------------------------"
java --version
echo "---- bazel --------------------------------------------------------------"
./tools/bazel version
echo "---- node ---------------------------------------------------------------"
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:node -- --version
echo "---- yarn ---------------------------------------------------------------"
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- --version
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- cache dir
