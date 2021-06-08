#!/usr/bin/python

import argparse
import logging
import os
import sys
import bzd.env
import time

from bzd.utils.run import localBazelBinary

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Generate compilation database")
	parser.add_argument("--workspace",
		dest="workspace",
		type=str,
		default=os.environ.get("BUILD_WORKSPACE_DIRECTORY", "."),
		help="Full path of the workspace to be sanitized.")
	parser.add_argument("--action", dest="actions", action="append", default=[], help="Action(s) to be performed")

	args = parser.parse_args()

	logging.info("Running sanitizer in '{}'...".format(args.workspace))
	noError = True
	for action in args.actions:
		startTime = time.monotonic()
		result = localBazelBinary(action, ignoreFailure=True, args=[args.workspace], timeoutS=300)
		elapsedTime = time.monotonic() - startTime
		noError = noError and (result.getReturnCode() == 0)
		if result.getReturnCode() != 0:
			print(result.getOutput())
			# Print those lines after the output to ensure they are visible on the screen.
			logging.error("Failed action '{}' ({:.1f}s)".format(action, elapsedTime))
			logging.error("Run: bazel run {} -- \"{}\"".format(action, args.workspace))
		else:
			logging.info("Completed action '{}' ({:.1f}s)".format(action, elapsedTime))

	# Return the error code
	sys.exit(0 if noError else 1)
