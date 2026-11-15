import argparse
import pathlib
import sys
import typing

from bzd_sanitizer.context import Context
from tools.ci.factory import Factory
from tools.ci.gitea.factory import Gitea
from tools.ci.github.factory import GitHub
from tools.ci.jenkins.factory import Jenkins
from tools.ci.quality_gate.factory import QualityGate
from tools.ci.travis.factory import Travis

factories: typing.Final[typing.Iterable[type[Factory]]] = (
	Jenkins,
	QualityGate,
	Travis,
	GitHub,
	Gitea,
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
