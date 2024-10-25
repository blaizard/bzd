import argparse
import pathlib
import typing
import json
import tempfile
import subprocess
import os

import yaml
from bzd.utils.run import localBazelBinary

Requirements = typing.Dict[str, typing.Optional[str]]


def parseRequirements(requirements: str) -> Requirements:
	"""Parse a requirements file."""

	result: Requirements = {}
	for requirement in requirements.split("\n"):
		requirementSplit = requirement.strip().split("=")
		if len(requirementSplit) == 1:
			[name] = requirementSplit
			if name:
				result[name.strip()] = None
		elif len(requirementSplit) == 2:
			[name, version] = requirementSplit
			result[name.strip()] = version.strip()
		else:
			raise Exception(f"The requirement file is malformed: '{requirement}'")
	return result


def makePackageJson(requirements: Requirements) -> str:
	"""Generate the content of a package.json file from the requirements."""

	content = {
	    "name": "bzd",
	    "version": "1.0.0",
	    "private": True,
	    "dependencies": {
	        name: "latest" if maybeVersion is None else maybeVersion
	        for name, maybeVersion in requirements.items()
	    },
	}
	return json.dumps(content, indent=4)


def makeRequirementsJsonFromPnpmLock(pnpmLock: str) -> str:
	"""Generate the requirements.json."""

	data = yaml.safe_load(pnpmLock)  # type: ignore

	# Build a make with packages and their dependencies.
	# {mocha@10.0.0: {integrity: <>, dependencies: ["hello@1.0.2", ...]}}
	packages: typing.Dict[str, typing.Any] = {}

	def packageToString(name: str, version: str) -> str:
		"""Clean-up a name/version."""

		[cleanedVersion, *_] = version.split("(", maxsplit=1)
		return f"{name}@{cleanedVersion}"

	def assertPackage(package: str) -> None:
		"""Assert that a package exists."""
		assert package in packages, f"The package {package} does not exists."

	def getDependencies(package: str) -> typing.Dict[str, str]:
		"""Get all dependencies and their integrity of a package (including self)."""

		output = {}
		queue = [package]
		while len(queue) > 0:
			current = queue.pop()
			if current in output:
				continue
			output[current] = packages[current]["integrity"]
			queue += packages[current]["dependencies"]
		return output

	# Get all packages and their version.
	# Note their can be multiple version of the same package.
	for package, metadata in data.get("packages").items():
		integrity = metadata.get("resolution", {}).get("integrity", None)
		packages[package] = {"integrity": integrity, "dependencies": []}

	# Populate the dependencies.
	for package, metadata in data.get("snapshots").items():

		# Get the package.
		[name, version] = package.rsplit("@", maxsplit=1)
		package = packageToString(name, version)
		assertPackage(package)

		# Fill in the dependencies.
		for name, version in metadata.get("dependencies", {}).items():
			dependency = packageToString(name, version)
			assertPackage(dependency)
			packages[package]["dependencies"].append(dependency)

	# Generate the output.
	output = {}
	for importer in data.get("importers").values():
		for name, metadata in importer.get("dependencies", {}).items():
			version = metadata["version"]
			package = packageToString(name, version)
			assertPackage(package)
			output[name] = getDependencies(package)

	return json.dumps(output, indent=4)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Compile requiments into a deps lock file.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.", required=True)
	parser.add_argument("--pnpm", type=pathlib.Path, help="The path of the pnpm tool.", required=True)
	parser.add_argument("srcs", type=pathlib.Path, nargs="+", help="Requirement inputs.")

	args = parser.parse_args()

	# Read and parse all requirements.
	requirements = {k: v for path in args.srcs for k, v in parseRequirements(path.read_text()).items()}

	# Generate the package.json file.
	packageJson = makePackageJson(requirements)

	with tempfile.TemporaryDirectory() as tmpDirname:
		(pathlib.Path(tmpDirname) / "package.json").write_text(packageJson)

		# Run pnpm to generate the dependency lock file.
		localBazelBinary(str(args.pnpm), ["install", "--color", "--lockfile-only", "--dir", tmpDirname],
		                 stdout=True,
		                 stderr=True)
		pnpmLock = (pathlib.Path(tmpDirname) / "pnpm-lock.yaml").read_text()

	requimentsJson = makeRequirementsJsonFromPnpmLock(pnpmLock)

	[repoName, *path] = pathlib.Path(args.output).parts
	output = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"]) / pathlib.Path(*path)

	output.write_text(requimentsJson)
