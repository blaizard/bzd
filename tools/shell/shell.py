import sys
import os
import pathlib
import typing

from tools.shell.factory import Factory
from tools.shell.sh.factory import Sh

factories: typing.Final[typing.Iterable[typing.Type[Factory]]] = (Sh,)


def buildAll(workspace: pathlib.Path) -> None:
	for factoryClass in factories:
		factory = factoryClass()
		print(f"Building {factory.getName()}.")
		factory.build(workspace=workspace)


def checkAll(workspace: pathlib.Path) -> bool:
	isValid = True
	for factoryClass in factories:
		factory = factoryClass()
		print(f"Checking {factory.getName()}.")
		isValid = factory.check(workspace=workspace) and isValid
	return isValid


if __name__ == "__main__":
	for factoryClass in factories:
		factory = factoryClass()
		# Look for the type of shell used on the current platform.
		if factory.isCompatible():
			print(f"Building {factory.getName()}.")
			factory.build(workspace=pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ".")))

			print(f"Installing {factory.getName()}.")
			factory.install()

			sys.exit(0)

	print("Unsupported shell type.")
	sys.exit(1)
