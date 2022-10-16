#!/usr/bin/env groovy

// Do not modify, this file has been generated by //tools/ci

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
				stage("[normal] clang-tidy")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=dev --config=clang_tidy --platform_suffix=clang_tidy"
					}
				}
				stage("[normal] linux_x86_64_clang prod")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_clang --config=prod --config=cc --platform_suffix=linux_x86_64_clang_prod"
					}
				}
				stage("[normal] linux_x86_64_gcc prod")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=+metadata --config=linux_x86_64_gcc --config=prod --config=cc --platform_suffix=linux_x86_64_gcc_prod"
					}
				}
				stage("[normal] esp32_xtensa_lx6_gcc prod")
				{
					steps
					{
						sh "./tools/bazel run tools/docker_images:xtensa_qemu"
						sh "./tools/bazel test ... --output_groups=+metadata --config=esp32_xtensa_lx6_gcc --config=prod --config=cc --platform_suffix=esp32_xtensa_lx6_gcc_prod"
					}
				}
				stage("[stress] dev (10 runs)")
				{
					steps
					{
						sh "./tools/bazel test ... --build_tests_only --test_tag_filters=stress,-cc-coverage --config=dev --runs_per_test=10 --platform_suffix=stress_dev"
					}
				}
				stage("[stress] prod (10 runs)")
				{
					steps
					{
						sh "./tools/bazel test ... --build_tests_only --test_tag_filters=stress,-cc-coverage --config=prod --runs_per_test=10 --platform_suffix=stress_prod"
					}
				}
				stage("[sanitizer] asan/lsan")
				{
					steps
					{
						sh "./tools/bazel test ... --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=clang_asan_lsan"
					}
				}
				stage("[coverage] C++")
				{
					steps
					{
						lock("coverage")
						{
							sh "./tools/bazel coverage cc/... --config=linux_x86_64_gcc --config=cc --platform_suffix=coverage_cc && ./tools/bazel run tools/coverage --platform_suffix=coverage_cc -- --output bazel-out/coverage_cc"
						}
						archiveArtifacts artifacts: "bazel-out/coverage_cc/**/*", onlyIfSuccessful: true
					}
				}
				stage("[coverage] Python")
				{
					steps
					{
						lock("coverage")
						{
							sh "./tools/bazel coverage ... --config=py --platform_suffix=coverage_py && ./tools/bazel run tools/coverage --platform_suffix=coverage_py -- --output bazel-out/coverage_py"
						}
						archiveArtifacts artifacts: "bazel-out/coverage_py/**/*", onlyIfSuccessful: true
					}
				}
				stage("[coverage] NodeJs")
				{
					steps
					{
						lock("coverage")
						{
							sh "./tools/bazel coverage ... --config=nodejs --platform_suffix=coverage_nodejs && ./tools/bazel run tools/coverage --platform_suffix=coverage_nodejs -- --output bazel-out/coverage_nodejs"
						}
						archiveArtifacts artifacts: "bazel-out/coverage_nodejs/**/*", onlyIfSuccessful: true
					}
				}
				stage("[sanitizer] sanitizer")
				{
					steps
					{
						sh "./sanitize.sh"
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
    post {
        always {
			sh "./info.sh"
        }
    }
}
