import argparse
import os
import pathlib
import sys
import time

from bzd.utils.run import localBazelTarget

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Workspace dependencies updater.")
	parser.add_argument("--action", type=str, action="append", default=[], help="Actions to be used.")
	parser.add_argument(
		"--workspace",
		type=pathlib.Path,
		default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."),
		help="The path of the workspace.",
	)
	args = parser.parse_args()

	success = True
	for action in args.action:
		print(f"{action}...", end="", flush=True)
		startTime = time.monotonic()

		result = localBazelTarget(
			action,
			[str(args.workspace)],
			ignoreFailure=True,
			timeoutS=600,
			cwd=args.workspace,
		)

		passed = result.getReturnCode() == 0
		elapsedTime = time.monotonic() - startTime
		print(f" {'PASSED' if passed else 'FAILED'} ({elapsedTime:.1f}s)")

		if not passed:
			print(result.getOutput(), end="")
			success = False

	sys.exit(0 if success else 1)
