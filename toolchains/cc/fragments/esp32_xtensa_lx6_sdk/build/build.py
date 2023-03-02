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

path_compile_commands = pathlib.Path(
    "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build/compile_commands.json")
path_linker_command = pathlib.Path(
    "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build/CMakeFiles/bzd.elf.dir/link.txt")


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
	copyFileTree(output / "include", gcc.includeSearchPaths, r'.*\.h')


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Generate the content of the esp32 SDK from esp-idf.")
	parser.add_argument("-o",
	                    "--output",
	                    type=pathlib.Path,
	                    default="./output",
	                    help="Output directory where to store the results.")

	args = parser.parse_args()

	assert "IDF_PATH" in os.environ, "The ESP-IDF must be properly installed, make sure you run esp-idf/export.sh"
	assert "BUILD_WORKSPACE_DIRECTORY" in os.environ, "This script must be run with bazel."
	workspace = pathlib.Path(os.environ["BUILD_WORKSPACE_DIRECTORY"])

	# Create the output directory
	outputResolved = args.output.expanduser().resolve()
	if not outputResolved.is_dir():
		os.makedirs(outputResolved)

	# Cleanup the sub-directories that will be generated
	for name in (
	    "ld",
	    "lib",
	    "include",
	):
		if (outputResolved / name).is_dir():
			shutil.rmtree(outputResolved / name)

	# Cleanup the project
	projectPath = workspace / "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project"
	(projectPath / "sdkconfig").unlink(missing_ok=True)

	# Create the build directory
	buildPath = projectPath / "build"
	if buildPath.is_dir():
		shutil.rmtree(buildPath)
	buildPath.mkdir()

	# Build the project
	localCommand(cmds=["cmake", "..", "-G", "Unix Makefiles"], cwd=buildPath, stdout=True, stderr=True)
	localCommand(cmds=["make"], cwd=buildPath, stdout=True, stderr=True, timeoutS=300)

	commands = {
	    "compile": CommandEntry("compile_flags"),
	    "link": CommandEntry("link_flags"),
	}

	# Look for the compile command of the main.
	compile_commands = json.loads((workspace / path_compile_commands).read_text())
	for entry in compile_commands:
		if entry["file"].endswith("toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/main/main.cc"):
			assert not commands["compile"].isDiscovered, "The compile command has been discovered twice."
			commands["compile"].command = entry["command"]
	assert commands["compile"].isDiscovered, "The compile command was not discovered."

	# Look for the linker command
	commands["link"].command = (workspace / path_linker_command).read_text()

	for name, entry in commands.items():

		print(f"Processing entry '{name}'...")

		gcc = CommandExtractorGcc(cwd=workspace / "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build")
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
				print(f"\"{arg}\",")
		print("=======================================================")

		copyArtifacts(outputResolved, gcc)

	print(f"Artifacts copied to: {outputResolved}")
