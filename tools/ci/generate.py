import argparse
import sys
import pathlib
import typing

from tools.ci.factory import Factory
from tools.ci.jenkins.factory import Jenkins
from tools.ci.quality_gate.factory import QualityGate
from tools.ci.travis.factory import Travis
from tools.ci.github.factory import GitHub
from lib.context import Context

factories: typing.Final[typing.Iterable[typing.Type[Factory]]] = (
    Jenkins,
    QualityGate,
    Travis,
    GitHub,
)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Wrapper for CI generation files")
	parser.add_argument("context", type=pathlib.Path, help="The context file path.")
	args = parser.parse_args()

	context = Context.fromFile(args.context)

	for factoryClass in factories:
		factory = factoryClass()
		print(f"Building & installing {factory.getName()}.")
		factory.build()
		factory.install(workspace=context.workspace)

	sys.exit(0)
