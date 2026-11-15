import pathlib
import typing

from bdl.generators.json.ast.ast import Ast, Parameters
from bzd.template.template import Template
from bzd.utils.secret import maybeDecrypt
from targets.docker.deployment.base import Docker


class DeploymentDockerTraefik(Docker):
	def __init__(self, ast: Ast, common: typing.Any) -> None:
		super().__init__(ast=ast, symbol="docker.Traefik")
		self.common = common

	def makeDockerCompose(self, fqn: str, expression: Parameters) -> tuple[str, set[str]]:
		dockers = self.getInterfaces(fqn, "add")
		template = Template.fromPath(pathlib.Path(__file__).parent / "traefik.yml.btl")
		content = template.render({"dockers": dockers, "maybeDecrypt": maybeDecrypt}, self.common)
		return content, {docker.image for docker in dockers}
