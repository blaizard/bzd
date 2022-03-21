import bzd.env
import logging
import sys
import os
import pathlib

from tools.shell.sh.factory import Sh

if __name__ == "__main__":

	for shellClass in (Sh, ):

		shell = shellClass()
		# Look for the type of shell used on the current platform.
		if shell.isCompatible():

			logging.info(f"Building {shell.getName()}.")
			shell.build(workspace=pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ".")))

			logging.info(f"Installing {shell.getName()}.")
			shell.install()

			sys.exit(0)

	logging.error("Unsupported shell type.")
