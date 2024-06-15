import typing
import pathlib

from targets.docker.deployment.base import Docker
from bdl.generators.json.ast.ast import Ast, Parameters
from bzd.template.template import Template


class DockerTraefik(Docker):

	def __init__(self, ast: Ast, common: typing.Any) -> None:
		super().__init__(ast=ast, symbol="docker.Traefik")
		self.common = common

	def makeDockerCompose(self, fqn: str, expression: Parameters) -> typing.Tuple[str, typing.Set[str]]:
		dockers = self.getInterfaces(fqn, "add")
		template = Template.fromPath(pathlib.Path(__file__).parent / "traefik.yml.btl")
		content = template.render({"dockers": dockers}, self.common)
		return content, set([docker.image for docker in dockers])
