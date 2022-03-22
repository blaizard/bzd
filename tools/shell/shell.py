import bzd.env
import logging
import sys
import os
import pathlib
import typing

from tools.shell.factory import Factory
from tools.shell.sh.factory import Sh

factories: typing.Final[typing.Iterable[typing.Type[Factory]]] = (Sh, )


def buildAll(workspace: pathlib.Path) -> None:

	for factoryClass in factories:
		factory = factoryClass()
		logging.info(f"Building {factory.getName()}.")
		factory.build(workspace=workspace)


if __name__ == "__main__":

	for factoryClass in factories:

		factory = factoryClass()
		# Look for the type of shell used on the current platform.
		if factory.isCompatible():

			logging.info(f"Building {factory.getName()}.")
			factory.build(workspace=pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ".")))

			logging.info(f"Installing {factory.getName()}.")
			factory.install()

			sys.exit(0)

	logging.error("Unsupported shell type.")
