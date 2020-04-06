#!/usr/bin/env groovy

pipeline
{
	triggers
	{
		cron("@daily")
		pollSCM("H H/8 * * *") // Every 8 hours
	}
	agent
	{
		dockerfile
		{
			filename "tools/jenkins/debian.dockerfile"
			// The source path should be replaced by an environment variable
			args "-v /volume1/docker/jenkins/cache:/cache"
		}
	}
	stages
	{
		stage("Environment")
		{
			steps
			{
				sh "cp tools/jenkins/.bazelrc.ci .bazelrc.ci"
				sh "bazel --version"
				sh "g++ --version"
				sh "python --version"
			}
		}
		/**
		 * Parallelized tests are done here
		 */
		stage("Tests")
		{
			parallel
			{
				stage("local")
				{
					steps
					{
						sh "bazel test ..." 
					}
				}
				stage("linux_x86_64_clang")
				{
					steps
					{
						sh "bazel test ... --config=linux_x86_64_clang --platform_suffix=_linux_x86_64_clang" 
					}
				}
				stage("esp32_xtensa_lx6_gcc")
				{
					steps
					{
						sh "bazel build ... --config=esp32_xtensa_lx6_gcc --platform_suffix=_esp32_xtensa_lx6_gcc" 
					}
				}
				stage("Static analyzers")
				{
					steps
					{
						sh "bazel test ... --config=linux_x86_64_clang --config=sanitizer --config=asan --config=lsan --platform_suffix=_clang_asan_lsan" 
					}
				}
			}
		}
	}
}
