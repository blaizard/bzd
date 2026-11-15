import pathlib
import typing

import yaml
from bdl.generators.json.ast.ast import Ast, Parameters
from targets.docker.deployment.base import Docker


class DeploymentDockerCompose(Docker):
	def __init__(self, ast: Ast, common: typing.Any) -> None:
		super().__init__(ast=ast, symbol="docker.Compose")
		self.common = common

	def makeDockerCompose(self, fqn: str, expression: Parameters) -> tuple[str, set[str]]:
		dockers = self.getInterfaces(fqn, "image")
		path = self.getConfig(fqn).path
		content = pathlib.Path(path).read_text()

		# Update the docker compose file with the images
		data = yaml.safe_load(content)  # type: ignore
		for docker in dockers:
			if docker.service in data["services"]:
				data["services"][docker.service]["image"] = self.common.imageResolve(docker.image)
			else:
				raise KeyError(f"Service '{docker.service}' is not present in '{path}'.")
		content = yaml.dump(data)  # type: ignore

		return content, {docker.image for docker in dockers}
