import os
import pathlib
import json
import shutil
import typing
import argparse
import re

from bzd.utils.run import localCommand
from bzd.command_extractor.gcc import CommandExtractorGcc, Categories

path_compile_commands = pathlib.Path(
	"toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build/compile_commands.json")
path_linker_command = pathlib.Path(
	"toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build/CMakeFiles/bzd.elf.dir/link.txt")


def copyFiles(output: pathlib.Path, fileList: typing.Sequence[pathlib.Path]) -> None:
	if not output.is_dir():
		os.makedirs(output)
	for f in fileList:
		shutil.copyfile(f, output.joinpath(f.name))


def copyFileTree(output: pathlib.Path, fileList: typing.Sequence[pathlib.Path], pattern: str) -> None:
	regexpr = re.compile(pattern)
	if not output.is_dir():
		os.makedirs(output)
	for f in fileList:
		for root, dirs, files in os.walk(f.as_posix()):
			for name in files:
				path = pathlib.Path(root).joinpath(name)
				if re.match(regexpr, path.as_posix()):
					relPath = path.relative_to(f)
					dst = output.joinpath(relPath)
					if not dst.parent.is_dir():
						os.makedirs(dst.parent)
					shutil.copyfile(path.as_posix(), dst.as_posix())


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
	if outputResolved.is_dir():
		shutil.rmtree(outputResolved)
	os.makedirs(outputResolved)

	# TODO: remove ignoreFailure, the build should succeed.
	#localCommand(cmds = ["idf.py", "build"], cwd = workspace / "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project", ignoreFailure = True)

	gcc = CommandExtractorGcc(cwd=workspace / "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build")

	# Look for the compile command of the main.
	nbFileProcessed = 0
	compile_commands = json.loads((workspace / path_compile_commands).read_text())
	for entry in compile_commands:
		if entry["file"].endswith("toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/main/main.cc"):
			compile_command = entry["command"]
			gcc.parse(compile_command)
			nbFileProcessed += 1
	assert nbFileProcessed == 1, "There should be only 1 file processed."

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
		Categories.flag: ("macro-prefix-map=*"),
		})

	print("==== Copy the following to the copts ===")
	for item in result:
		print(f"\"{item}\",")
	print("========================================")

	gcc = CommandExtractorGcc(cwd=workspace / "toolchains/cc/fragments/esp32_xtensa_lx6_sdk/build/project/build")

	# Look for the linker command
	linker_command = (workspace / path_linker_command).read_text()
	gcc.parse(linker_command)

	result = gcc.values()

	#print(result)

	# Copy all the files to the output directory.
	#copyFiles(outputResolved / "ld", [pathlib.Path(f) for f in result.files.linkerScripts])
	#copyFiles(outputResolved / "lib", [pathlib.Path(f) for f in result.files.libraries])
	#copyFileTree(outputResolved / "include",
	#		[pathlib.Path(f) for f in filter(lambda x: x not in ["."], result.files.includes)], r'.*\.h')

	# Print the inker command:
	#print("Linker commands (lds): {}".format(" ".join(["-T{}".format(p) for p in result.linkerScripts])))
	#print("Linker commands (libs): {}".format(" ".join(["-l{}".format(p) for p in result.libraries])))

	#print(*gcc.librarySearchPaths)
