import argparse
import pathlib
import typing
import json
import tempfile
import subprocess
import os
import re

from bzd.utils.run import localBazelBinary
from bzd.utils.semver import Semver, SemverMatcher
from dataclasses import dataclass

Requirements = typing.Dict[str, typing.Optional[str]]
Json = typing.Dict[str, typing.Any]


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


@dataclass
class Package:
	"""Description of a package."""

	name: str
	version: Semver
	integrity: str
	dependencies: typing.Set["Package"]
	os: typing.Optional[typing.List[str]] = None
	cpu: typing.Optional[typing.List[str]] = None
	alias: typing.Optional[str] = None

	def makeAlias(self, alias: str) -> "Package":
		"""Create a clone of this package that is an alias."""

		return Package(
		    name=self.name,
		    version=self.version,
		    integrity=self.integrity,
		    dependencies=self.dependencies,
		    alias=alias,
		)

	@property
	def package(self) -> str:
		if self.alias:
			return f"{self.alias}@npm:{self.name}@{str(self.version)}"
		return f"{self.name}@{str(self.version)}"

	def __str__(self) -> str:
		return self.package

	# Overloads needed for set __hash__, __eq__ and __ne__

	def __hash__(self) -> int:
		return hash(str(self))

	def __eq__(self, other: object) -> bool:
		if not isinstance(other, Package):
			return NotImplemented
		return str(self) == str(other)

	def __ne__(self, other: object) -> bool:
		return self != other

	def __repr__(self) -> str:
		return f"<Package {str(self)}>"


class Packages:
	"""Data structure to access packages."""

	def __init__(self) -> None:
		self.packages: typing.Dict[str, Package] = {}
		self.packagesByName: typing.Dict[str, typing.Dict[Semver, Package]] = {}

	def add(self, package: Package) -> None:
		assert package.package not in self.packages, f"Package '{package.package}' is already added to the dictionary."
		self.packages[package.package] = package
		self.packagesByName.setdefault(package.name, {})
		self.packagesByName[package.name][package.version] = package

	def get(self, package: str) -> Package:
		return self.packages[package]

	def getMatching(self, name: str, constraints: str) -> Package:
		matcher = SemverMatcher("*" if constraints == "latest" else constraints)
		possibleVersions = self.packagesByName[name].keys()
		maybeVersion = matcher.matchLatest(possibleVersions)
		assert maybeVersion is not None, f"Couldn't find a matching version for {name} with {matcher} and {possibleVersions}."
		return self.packagesByName[name][maybeVersion]


class RequirementsFactory:

	def __init__(self, packageLock: Json) -> None:
		self.packageLock = packageLock
		self.packages = self.packagesParser()

	def packagesParser(self) -> Packages:
		return self.packagesParserV3(self.packageLock)

	@staticmethod
	def packagesParserV3(packageLock: Json) -> Packages:
		"""Parse v3 of packages-lock.json."""

		packagesByName: typing.Dict[str, typing.Dict[Package, typing.List[typing.Dict[str, typing.Any]]]] = {}
		for path, data in packageLock["packages"].items():
			name = path.split("node_modules/")[-1]
			if not name:
				continue
			packagesByName.setdefault(name, {})
			package = Package(name=name,
			                  version=Semver.fromString(data["version"]),
			                  integrity=data["integrity"],
			                  os=data.get("os"),
			                  cpu=data.get("cpu"),
			                  dependencies=set())
			packagesByName[name][package] = []
			# We take optional dependencies because of `rollup` which declares some of the things as optional.
			# It is fine, be we should filter everything by platform/cpu as well.
			dependencies = data.get("dependencies", {}) | data.get("optionalDependencies", {})
			for alias, matchData in dependencies.items():
				# Packages may have aliases in the form: case-1.5.3@npm:case@1.5.3
				if matchData.startswith("npm:"):
					packageName, constraint = matchData[4:].rsplit("@", maxsplit=1)
					packagesByName[name][package].append({
					    "alias": alias,
					    "name": packageName,
					    "matcher": SemverMatcher(constraint)
					})
				else:
					packagesByName[name][package].append({
					    "alias": None,
					    "name": alias,
					    "matcher": SemverMatcher(matchData)
					})

		# Resolve the dependencies.
		packages = Packages()
		for name, packageVersions in packagesByName.items():
			for package, dependencies in packageVersions.items():
				for dependency in dependencies:
					assert dependency[
					    "name"] in packagesByName, f"The dependency '{dependency['name']}' from package '{package}' is not found."
					versionsPackages = {p.version: p for p in packagesByName[dependency["name"]].keys()}
					maybeVersion = dependency["matcher"].matchLatest(versionsPackages.keys())
					assert maybeVersion is not None, f"Couldn't find a matching version for '{dependency['name']}' with {dependency['matcher']} and {versionsPackages.keys()}."
					if dependency["alias"]:
						package.dependencies.add(versionsPackages[maybeVersion].makeAlias(alias=dependency["alias"]))
					else:
						package.dependencies.add(versionsPackages[maybeVersion])
				packages.add(package)

		return packages

	@property
	def version(self) -> int:
		return int(self.packageLock["lockfileVersion"])

	@staticmethod
	def getDependencies(package: Package) -> typing.Set[Package]:
		"""Get all dependencies of a package (including self)."""

		output = set()
		queue = [package]
		while len(queue) > 0:
			current = queue.pop()
			if current in output:
				continue
			output.add(current)
			queue += current.dependencies
		return output

	def make(self, requirements: Requirements) -> typing.Dict[str, typing.Any]:
		"""Build the content of the requirements file.
		
		Format is as follow:
			"express-minify": {
				"dependencies": {
					"clean-css@4.2.4": {
						"integrity": "sha512-EJUDT7nDVFDvaQgAo2G/PJvxmp1o/c6iXLbswsBbUFXi1Nr+AjA2cKmfbKDMjMvzEe75g3P6JkaDDAKk96A85A==",
						"os": ["linux"], // optional
						"cpu": ["arm64"] // optional
					}
					...
				},
				"name": "express-minify",
				"version": "1.0.0"
			},
		"""

		output = {}
		for name, maybeConstraints in requirements.items():
			package = self.packages.getMatching(name=name,
			                                    constraints=maybeConstraints if maybeConstraints else "latest")
			packages = self.getDependencies(package)

			# Build the dependencies
			dependencies = {}
			for d in packages:
				dependencies[d.package] = {
				    "integrity": d.integrity
				} | ({} if d.os is None else {
				    "os": d.os
				}) | ({} if d.cpu is None else {
				    "cpu": d.cpu
				})

			output[name] = {"name": name, "version": str(package.version), "dependencies": dependencies}
		return output


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Compile requiments into a deps lock file.")
	parser.add_argument("--output", type=pathlib.Path, help="The output path for the generated file.")
	parser.add_argument("--output-path", type=pathlib.Path, help="The output path for the generated file as a string.")
	parser.add_argument("--npm", type=pathlib.Path, help="The path of the pnpm tool.", required=True)
	parser.add_argument("srcs", type=pathlib.Path, nargs="+", help="Requirement inputs.")

	args = parser.parse_args()

	# Read and parse all requirements.
	requirements = {k: v for path in args.srcs for k, v in parseRequirements(path.read_text()).items()}

	# Generate the package.json file.
	packageJson = makePackageJson(requirements)

	with tempfile.TemporaryDirectory() as tmpDirname:
		(pathlib.Path(tmpDirname) / "package.json").write_text(packageJson)

		# Run npm to generate the dependency lock file.
		localBazelBinary(
		    str(args.npm),
		    # --slient seem to make the command hang sometimes.
		    ["install", "--package-lock-only", "--include=optional", "--prefix", tmpDirname, tmpDirname],
		    stdout=True,
		    stderr=True)
		packageLock = (pathlib.Path(tmpDirname) / "package-lock.json").read_text()

	requimentsJson = RequirementsFactory(json.loads(packageLock))
	result = requimentsJson.make(requirements)
	serialized = json.dumps(result, sort_keys=True, indent=4)

	if args.output:
		[repoName, *path] = pathlib.Path(args.output).parts
		output = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"]) / pathlib.Path(*path)
		output.write_text(serialized)
	elif args.output_path:
		output = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"]) / pathlib.Path(args.output_path)
		output.write_text(serialized)
	else:
		print(serialized)
