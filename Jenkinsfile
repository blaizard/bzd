#!/usr/bin/env groovy
/**
 * It requires the following Jenkins plugins to work:
 * - Warnings Next Generation
 * - JUnit Plugin
 * - valgrind
 */
pipeline
{
	agent
	{
		dockerfile
		{
			filename "tools/jenkins/debian.dockerfile"
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
		 * The actual tests are done here
		 */
		stage("Tests")
		{
			parallel
			{
				stage("bazel test ...")
				{
					steps
					{
						sh "bazel test ..." 
					}
				}
			}
		}
	}
}
