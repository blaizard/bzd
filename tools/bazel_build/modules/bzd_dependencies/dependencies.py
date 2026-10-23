import argparse
import os
import pathlib
import sys
import time
import typing

from dependencies.bazel_dep.bazel_dep import BazelDep
from dependencies.interface import DependencyInterface

AllDependencies: typing.Dict[str, typing.Type[DependencyInterface]] = {"bazel_dep": BazelDep}


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Workspace dependencies updater.")
	parser.add_argument(
		"--use",
		type=str,
		action="append",
		choices=list(AllDependencies),
		default=[],
		metavar="<dependency>",
		help="The type of dependency to use, all are used by default.",
	)
	parser.add_argument(
		"--upgrade",
		action="store_true",
		help="Upgrade the dependencies.",
	)
	parser.add_argument(
		"--workspace",
		type=pathlib.Path,
		default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."),
		help="The path of the workspace.",
	)
	args = parser.parse_args()

	success = True
	for name in args.use if args.use else AllDependencies.keys():
		print(f"==== {name}...", flush=True, end="")
		startTime = time.monotonic()
		try:
			dependency = AllDependencies[name](args.workspace)
			dependencies = dependency.get()
			if args.upgrade:
				dependency.upgrade(dependencies)
				upgraded = dependency.get()
				print(f" UPGRADED ({time.monotonic() - startTime:.1f}s)")
				for change in dependencies.diff(upgraded):
					print(change)
			else:
				print(f" ({time.monotonic() - startTime:.1f}s)")
				print(dependencies)
			passed = True
		except Exception as error:
			print(f"{name}... FAILED ({time.monotonic() - startTime:.1f}s)")
			print(str(error))
			passed = False

		success = success and passed

	sys.exit(0 if success else 1)
