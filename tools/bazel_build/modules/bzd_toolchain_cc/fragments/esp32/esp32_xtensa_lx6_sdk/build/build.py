import os
import pathlib
import json
import shutil
import typing
import argparse
import re
import dataclasses

from bzd.utils.run import localCommand
from bzd.command_extractor.gcc import CommandExtractorGcc, Categories, ItemLibrary


def copyFiles(output: pathlib.Path, paths: typing.Iterable[pathlib.Path]) -> None:
	if not output.is_dir():
		os.makedirs(output)
	for f in paths:
		shutil.copyfile(f, output.joinpath(f.name))


def copyFileTree(output: pathlib.Path, paths: typing.Iterable[pathlib.Path], pattern: str) -> None:
	regexpr = re.compile(pattern)
	if not output.is_dir():
		os.makedirs(output)
	for f in paths:
		for root, dirs, files in os.walk(f.as_posix()):
			for name in files:
				path = pathlib.Path(root).joinpath(name)
				if re.match(regexpr, path.as_posix()):
					relPath = path.relative_to(f)
					dst = output.joinpath(relPath)
					if not dst.parent.is_dir():
						os.makedirs(dst.parent)
					shutil.copyfile(path.as_posix(), dst.as_posix())


def copyDirectory(output: pathlib.Path, directory: pathlib.Path) -> None:
	copyFileTree(output, [directory], ".*")


@dataclasses.dataclass
class CommandEntry:
	destination: str
	command: typing.Optional[str] = None

	@property
	def isDiscovered(self) -> bool:
		return self.command is not None


def copyArtifacts(output: pathlib.Path, gcc: CommandExtractorGcc) -> None:
	copyFiles(output / "ld", [pathlib.Path(f) for f in gcc.linkerScripts])
	copyFiles(output / "lib", [pathlib.Path(f) for f in gcc.libraries])
	copyFileTree(output / "include", gcc.includeSearchPaths, r".*\.h")


def envToDict(env: str) -> typing.Dict[str, str]:
	result: typing.Dict[str, str] = {}
	for line in env.split("\n"):
		key, value = line.split("=", maxsplit=1)
		result[key.strip()] = value.strip()
	return result


_TARGETS = {"esp32": "esp32_xtensa_lx6_sdk", "esp32s3": "esp32s3_xtensa_lx7_sdk"}

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Generate the content of the esp32 SDK from esp-idf.")
	parser.add_argument(
	    "--output",
	    type=pathlib.Path,
	    default="./output",
	    help="Output directory where to store the results.",
	)
	parser.add_argument(
	    "--project",
	    type=pathlib.Path,
	    default=pathlib.Path(__file__).parent / "project",
	    help="The project path.",
	)
	parser.add_argument(
	    "--env",
	    help="Environment variables of the host.",
	)
	parser.add_argument(
	    "--rebuild",
	    action="store_true",
	    help="Rebuild the whole project from scratch.",
	)
	parser.add_argument(
	    "target",
	    choices=_TARGETS.keys(),
	    help="Target chip (esp32, esp32s3, ...).",
	)

	args = parser.parse_args()

	assert ("IDF_PATH" in os.environ), "The ESP-IDF must be properly installed, make sure you run esp-idf/export.sh"
	assert ("BUILD_WORKSPACE_DIRECTORY" in os.environ), "This script must be run with bazel."
	workspace = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"])

	# Create the output directory
	outputResolved = args.output.expanduser().resolve()
	outputSDK = outputResolved / _TARGETS[args.target]
	project = outputResolved / "project"
	buildPath = project / "build"
	if outputSDK.is_dir():
		shutil.rmtree(outputSDK)
	outputSDK.mkdir(parents=True, exist_ok=True)

	# Cleanup the project
	if args.rebuild:
		if project.is_dir():
			shutil.rmtree(project)

	# Copy the important files to the output dir.
	copyDirectory(project, args.project)

	buildPath.mkdir(exist_ok=True)

	# Build the project
	env = envToDict(args.env)
	localCommand(cmds=["cmake", "..", f"-DIDF_TARGET={args.target}", "-G", "Unix Makefiles"],
	             cwd=buildPath,
	             stdout=True,
	             stderr=True,
	             env=env)
	localCommand(cmds=["make"], cwd=buildPath, stdout=True, stderr=True, env=env, timeoutS=300)

	commands = {
	    "compile": CommandEntry("compile_flags"),
	    "link": CommandEntry("link_flags"),
	}

	# Look for the compile command of the main.
	compile_commands = json.loads((project / "build/compile_commands.json").read_text())
	for entry in compile_commands:
		if pathlib.Path(entry["file"]) == project / "main/main.cc":
			assert not commands["compile"].isDiscovered, "The compile command has been discovered twice."
			commands["compile"].command = entry["command"]
	assert commands["compile"].isDiscovered, "The compile command was not discovered."

	# Look for the linker command
	commands["link"].command = (project / "build/CMakeFiles/bzd.elf.dir/link.txt").read_text()

	for name, entry in commands.items():
		print(f"Processing entry '{name}'...")

		gcc = CommandExtractorGcc(cwd=project / "build")
		gcc.parse(entry.command)
		result = gcc.values(
		    exclude={
		        Categories.includeSearchPath: None,
		        Categories.librarySearchPath: None,
		        Categories.unhandled: None,
		        Categories.standard: None,
		        Categories.optimisation: None,
		        Categories.outputPath: None,
		        Categories.debug: None,
		        Categories.compileOnly: None,
		        Categories.warning: None,
		        Categories.flag: {r"macro-prefix-map=.*"},
		    })

		print(f"==== Copy the following to '{entry.destination}' ===")
		for item in result:
			argstr = ""
			if item.category == Categories.library:
				assert isinstance(item, ItemLibrary)
				if item.name not in {"main_stub"}:
					argstr = f"-l{item.name}"
			else:
				argstr = f"{item}"

			for arg in argstr.strip().split():
				print(f'"{arg}",')
		print("=======================================================")

		copyArtifacts(outputSDK, gcc)
	copyFiles(outputSDK / "bin",
	          [buildPath / "bootloader" / "bootloader.bin", buildPath / "partition_table" / "partition-table.bin"])

	print(f"Artifacts copied to: {outputSDK}")
