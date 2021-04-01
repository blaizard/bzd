#!/usr/bin/env groovy

// Seems to be the only way to conditionaly set a param for the dockerfile, see https://issues.jenkins-ci.org/browse/JENKINS-43911
node("master")
{
	stage("Docker")
	{
		CACHE_PATH = "${env.CACHE_PATH}" 
		echo "CACHE_PATH = $CACHE_PATH"
	}
}

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
			// additionalBuildArgs "--pull"
			filename "tools/ci/jenkins/debian.dockerfile"
			//args "-v $CACHE_PATH:/cache"
			args "-v /cache:/cache"
			//args "-v /var/run/docker.sock:/var/run/docker.sock"
			//args "-v /etc/localtime:/etc/localtime:ro"
			args "-e DOCKER_HOST=tcp://localhost:2375"
			//args "--network dind"
			//args "-v /usr/local/bin/docker:/usr/bin/docker"
			//args "--privileged"
			args "--network host"
		}
	}
	stages
	{
		stage("Environment")
		{
			steps
			{
				sh "id"
				sh "printenv"
				sh "ls -ll /"
				sh "ls -ll /cache"
				sh "cp tools/ci/jenkins/.bazelrc.local .bazelrc.local"
				sh "python --version"
				sh "java --version"
				sh "./tools/bazel --version"
				sh "git submodule foreach --recursive git clean -xfd"
				sh "git submodule foreach --recursive git reset --hard"
				sh "git submodule update --init --recursive"
			}
		}
		/**
		 * Parallelized tests are done here
		 */
		stage("Tests")
		{
			parallel
			{
			/*	stage("linux_x86_64_clang")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang --platform_suffix=_linux_x86_64_clang" 
					}
				}*/
				stage("linux_x86_64_gcc")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_gcc --platform_suffix=_linux_x86_64_gcc" 
					}
				}
			/*	stage("esp32_xtensa_lx6_gcc")
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
				}*/
				stage("Coverage")
				{
					steps
					{
						sh "./tools/bazel coverage ... --config=cc && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_cc"
						archiveArtifacts artifacts: "bazel-out/coverage_cc/**/*", onlyIfSuccessful: true
						sh "./tools/bazel coverage ... --config=nodejs && ./tools/bazel run tools/coverage -- --output bazel-out/coverage_nodejs"
						archiveArtifacts artifacts: "bazel-out/coverage_nodejs/**/*", onlyIfSuccessful: true
					}
				}
				/*stage("Sanitize")
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
				}*/
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
