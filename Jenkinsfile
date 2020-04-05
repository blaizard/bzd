#!/usr/bin/env groovy

pipeline
{
	agent
	{
		dockerfile
		{
			filename "tools/jenkins/debian.dockerfile"
			args "-v /volume1/docker/jenkins/cache:/cache"
		}
	}
	stages
	{
		stage("Info")
		{
			steps
			{
				sh "bazel --version"
				sh "g++ --version"
			}
		}
		/**
		 * Parallelized tests are done here
		 */
		stage("Tests")
		{
			parallel
			{
				stage("bazel test ...")
				{
					steps
					{
						sh "bazel test ... --disk_cache=/cache/bazel --output_user_root=/cache/output" 
					}
				}
			}
		}
	}
}
