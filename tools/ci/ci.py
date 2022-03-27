import bzd.env
import logging
import sys
import os
import pathlib
import typing

from tools.ci.factory import Factory
from tools.ci.jenkins.factory import Jenkins

factories: typing.Final[typing.Iterable[typing.Type[Factory]]] = (Jenkins, )

if __name__ == "__main__":

	for factoryClass in factories:
		factory = factoryClass()
		logging.info(f"Building & installing {factory.getName()}.")
		factory.build()
		factory.install(workspace=pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ".")))
