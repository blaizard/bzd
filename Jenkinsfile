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
			args "-v $CACHE_PATH:/cache"
			args "--network host"
		}
	}
	stages
	{
		stage("Environment")
		{
			steps
			{
				sh "cp tools/ci/jenkins/.bazelrc.local .bazelrc.local"
				sh "python --version"
				sh "java --version"
				sh "./tools/bazel --version"
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
				stage("linux_x86_64_clang")
				{
					steps
					{
						sh "./tools/bazel test ... --output_groups=default,metadata --config=linux_x86_64_clang --platform_suffix=_linux_x86_64_clang" 
					}
				}
				stage("esp32_xtensa_lx6_gcc")
				{
					steps
					{
						sh "./tools/bazel build ... --output_groups=default,metadata --config=esp32_xtensa_lx6_gcc --platform_suffix=_esp32_xtensa_lx6_gcc" 
					}
				}
				stage("Static analyzers")
				{
					steps
					{
						sh "./tools/bazel test ... --config=linux_x86_64_clang --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan" 
					}
				}
				stage("Coverage")
				{
					steps
					{
						sh "./tools/bazel coverage ..."
						archiveArtifacts artifacts: "bazel-out/coverage/**/*", fingerprint: true
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
			}
		}
	}
}
