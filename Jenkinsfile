#!/usr/bin/env groovy

pipeline
{
	triggers
	{
		cron("@weekly")
		pollSCM("H H/8 * * *") // Every 8 hours
	}
	agent
	{
		dockerfile
		{
			filename "tools/ci/jenkins/debian.dockerfile"
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
		/**
		 * Parallelized tests are done here
		 */
		stage("Tests")
		{
			parallel
			{
				stage("linux_x86_64_clang dev")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang --config=dev --platform_suffix=_linux_x86_64_clang_dev" 
					}
				}
				stage("linux_x86_64_clang prod")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang --config=prod --platform_suffix=_linux_x86_64_clang_prod" 
					}
				}
				stage("linux_x86_64_gcc")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=cc --config=linux_x86_64_gcc --platform_suffix=_linux_x86_64_gcc" 
					}
				}
				stage("esp32_xtensa_lx6_gcc")
				{
					steps
					{
						sh "./tools/bazel build ... --output_groups=default,metadata --config=cc --config=esp32_xtensa_lx6_gcc --platform_suffix=_esp32_xtensa_lx6_gcc" 
					}
				}
				stage("Static analyzers")
				{
					steps
					{
						sh "./tools/bazel test ... --config=linux_x86_64_clang --config=cc --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan" 
					}
				}
				stage("Coverage")
				{
					steps
					{
						//sh "./tools/bazel coverage ... --config=linux_x86_64_clang --config=cc && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_cc"
						//archiveArtifacts artifacts: "bazel-out/coverage_cc/**/*", onlyIfSuccessful: true
						sh "./tools/bazel coverage ... --config=nodejs && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_nodejs"
						archiveArtifacts artifacts: "bazel-out/coverage_nodejs/**/*", onlyIfSuccessful: true
					}
				}
				stage("Sanitize")
				{
					steps
					{
						sh "./sanitize.sh"
					}
				}
				stage("Stress")
				{
					steps
					{
						sh "./tools/bazel test ... --config=linux_x86_64_clang --build_tag_filters=stress --test_tag_filters=stress --runs_per_test=100 --platform_suffix=_linux_x86_64_clang" 
					}
				}
			}
		}
		/**
		 * Auto publish some artifacts
		 */
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
