#!/usr/bin/python3

import argparse
import os
import pathlib
import subprocess

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Update the rpath of all files within a directory.")
	parser.add_argument("path", help="The directory in which the rpath should be updated.")

	args = parser.parse_args()

	directory = pathlib.Path(args.path).resolve()

	# Build the executables and libraries map.
	executables: dict[pathlib.Path, set[str]] = {}
	libraries: dict[str, pathlib.Path] = {}

	for root, dirs, files in os.walk(directory):
		rootPath = pathlib.Path(root).resolve()
		for filename in files:
			f = rootPath / filename
			result = subprocess.run(["ldd", str(f)], capture_output=True, check=False)
			# If the operation fails, it means that this is not a dynamic executable/library.
			if result.returncode != 0:
				continue
			libs = {line.strip().split()[0].decode() for line in result.stdout.split(b"\n")[1:] if line.strip()}
			path = f.relative_to(directory)
			executables[path] = libs
			if path.name in libraries:
				print(
					f"Conflict: 2 libraries with the same name: '{path}' and '{libraries[path.name] / path.name}', using the latter."
				)
			else:
				libraries[path.name] = path.parent

	# Do the mapping.
	assigned: dict[str, pathlib.Path] = {}
	unassigned: dict[str, set[pathlib.Path]] = {}
	for path, libs in executables.items():
		for lib in libs:
			if lib in libraries:
				origin = pathlib.Path("$ORIGIN") / os.path.relpath(libraries[lib], path.parent)
				subprocess.run(["patchelf", "--remove-rpath", directory / path], check=True)
				subprocess.run(
					[
						"patchelf",
						"--force-rpath",
						"--set-rpath",
						origin,
						directory / path,
					],
					check=True,
				)
				assigned[lib] = libraries[lib] / lib
			else:
				unassigned.setdefault(lib, set()).add(path)

	print("Resolved libraries:")
	for lib in sorted(assigned):
		print(f"- {lib} -> {assigned[lib]}")
	print("Un-resolved libraries:")
	for lib in sorted(unassigned):
		print(f"- {lib} ({len(unassigned[lib])} files): {', '.join([str(p) for p in unassigned[lib]])[:100]}...")
