import bzd.env
import argparse
import logging
import sys
import pathlib
import typing

from tools.ci.factory import Factory
from tools.ci.jenkins.factory import Jenkins

factories: typing.Final[typing.Iterable[typing.Type[Factory]]] = (Jenkins, )

if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Wrapper for CI generation files")
	parser.add_argument("workspace", type=pathlib.Path, help="Workspace to be processed.")
	args = parser.parse_args()

	for factoryClass in factories:
		factory = factoryClass()
		logging.info(f"Building & installing {factory.getName()}.")
		factory.build()
		factory.install(workspace=args.workspace)

	sys.exit(0)
