import typing

from bdl.generators.json.ast.ast import Ast, Parameters


class Docker:

	def __init__(self, ast: Ast, symbol: str) -> None:
		self.ast = ast
		self.expressions = {
		    fqn: entity.expression.parameters
		    for fqn, entity in ast.registry.items() if entity.expression.symbol == symbol
		}

	def makeDockerComposeForAllInstances(self) -> typing.Tuple[typing.Dict[str, str], typing.Set[str]]:
		"""Generate content for the docker compose files.

		This includes the docker compose yaml content as string keyed per instance and a set
		containing all images.
		"""
		dockerComposeFiles = {}
		images = set()
		for fqn, expression in self.expressions.items():
			currentDockerCompose, currentImages = self.makeDockerCompose(fqn, expression)
			dockerComposeFiles[fqn] = currentDockerCompose
			images.update(currentImages)
		return dockerComposeFiles, images

	def getInterfaces(self, fqn: str, interface: str) -> typing.List[Parameters]:
		"""Gather object instances of a specific type."""

		return [
		    workload.expression.parameters for workload in self.ast.workloads
		    if workload.expression.symbol == f"{fqn}.{interface}"
		]

	def getConfig(self, fqn: str) -> Parameters:
		"""Gather object configuration."""

		return self.ast.registry[fqn].expression.parameters
