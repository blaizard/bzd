#/bin/bash

BAZEL_RUN='./tools/bazel run --ui_event_filters=-info,-stdout,-stderr --noshow_progress'

echo "---- node ---------------------------------------------------------------"
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:node -- --version
echo "---- yarn ---------------------------------------------------------------"
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- --version
${BAZEL_RUN} toolchains/nodejs/linux_x86_64_yarn:yarn -- cache dir
