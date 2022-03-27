#!/usr/bin/env groovy

pipeline
{
	triggers
	{
		cron("@weekly")
		pollSCM("0 3 * * *") // Every day at 3:00am if there is any change.
	}
	agent
	{
		dockerfile
		{
			filename "tools/ci/jenkins/Dockerfile"
			// Only a single args argument can be used
			args "-v /cache:/cache -e DOCKER_HOST=tcp://localhost:2375 --network host"
		}
	}
	stages
	{
		stage("Environment")
		{
			steps
			{
				sh "cp tools/ci/jenkins/.bazelrc.local .bazelrc.local"
				sh "git submodule foreach --recursive git clean -xfd"
				sh "git submodule foreach --recursive git reset --hard"
				sh "git submodule update --init --recursive"
				sh "ls -ll /"
				sh "./info.sh"
			}
		}
		// Parallelized tests are done here
		stage("Tests")
		{
			parallel
			{
				stage("normal linux_x86_64_clang dev + clang-tidy")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=dev --config=clang_tidy --platform_suffix=_linux_x86_64_clang_dev"
					}
				}
				stage("normal linux_x86_64_clang prod")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=prod --platform_suffix=_linux_x86_64_clang_prod"
					}
				}
				stage("normal linux_x86_64_gcc")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_gcc --config=prod --config=cc --platform_suffix=_linux_x86_64_gcc_prod"
					}
				}
				stage("normal esp32_xtensa_lx6_gcc")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=esp32_xtensa_lx6_gcc --config=prod --config=cc --platform_suffix=_esp32_xtensa_lx6_gcc_prod"
					}
				}
				stage("stress dev")
				{
					steps
					{
						sh "./tools/bazel test ... --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=dev --runs_per_test=100 --platform_suffix=_linux_x86_64_clang_dev"
					}
				}
				stage("stress prod")
				{
					steps
					{
						sh "./tools/bazel test ... --build_tag_filters=stress --test_tag_filters=stress --config=linux_x86_64_clang --config=prod --runs_per_test=100 --platform_suffix=_linux_x86_64_clang_prod"
					}
				}
				stage("sanitizer asan/lsan")
				{
					steps
					{
						sh "./tools/bazel test ... --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan"
					}
				}
				stage("coverage C++")
				{
					steps
					{
						sh "./tools/bazel coverage cc/... --config=linux_x86_64_gcc --config=cc --platform_suffix=_coverage_cc && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_cc"
						archiveArtifacts artifacts: "bazel-out/coverage_cc/**/*", onlyIfSuccessful: true
					}
				}
				stage("coverage Python")
				{
					steps
					{
						sh "./tools/bazel coverage ... --config=py --platform_suffix=_coverage_py && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_py"
						archiveArtifacts artifacts: "bazel-out/coverage_py/**/*", onlyIfSuccessful: true
					}
				}
				stage("coverage NodeJs")
				{
					steps
					{
						sh "./tools/bazel coverage ... --config=nodejs --platform_suffix=_coverage_nodejs && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_nodejs"
						archiveArtifacts artifacts: "bazel-out/coverage_nodejs/**/*", onlyIfSuccessful: true
					}
				}
			}
		}
		// Auto publish some artifacts
		stage("Publish")
		{
			steps
			{
				sh "./tools/bazel run apps/dashboard:push"
				sh "./tools/bazel run apps/family:push"
				sh "./tools/bazel run apps/artifacts:push"
			}
		}
	}
}
