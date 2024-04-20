import argparse
import json
import pathlib
import typing
import sys
import re

from tools.bdl.generators.json.ast.ast import Ast, Parameters
from bzd.template.template import Template


class Getter:

	def __init__(self, ast: Ast) -> None:
		self.ast = ast

	@property
	def dockers(self) -> typing.Iterable[Parameters]:
		for workload in ast.workloads:
			if workload.expression.symbol == "traefik.docker":
				yield workload.expression.parameters

	@staticmethod
	def hostToName(host: str) -> str:
		"""Convert a hostname into a sanitized name."""
		return re.sub(r"[^a-zA-Z0-9]+", '_', host)


class DeploySSH:

	def __init__(self, string: str) -> None:
		self.connection = DeploySSH.fromString(string)

	@staticmethod
	def fromString(string: str) -> None:
		"""Decompose a SSH string and gets its components.
		
		A SSH string must have the following format:
		<username>:<password>@<url>:<port>, where only <url> is mandatory.
		"""

		def splitLocation(location: str) -> typing.Tuple[str, int]:
			items = location.split(":")
			if len(items) == 1:
				return items[0], 22
			elif len(items) == 2:
				return items[0], int(items[1])
			raise Exception("Location parsing error")

		def splitAuthentication(authentication: str) -> typing.Tuple[str, typing.Optional[str]]:
			items = authentication.split(":")
			if len(items) == 1:
				return items[0], None
			elif len(items) == 2:
				return items[0], items[1]
			raise Exception("Authentication parsing error")

		items = string.split("@")
		try:
			if len(items) == 1:
				url, port = splitLocation(items[0])
			elif len(items) == 2:
				username, password = splitAuthentication(items[0])
				url, port = splitLocation(items[1])
			raise Exception("String parsing error")
		except Exception as e:
			raise Exception(
			    f"{str(e)}: SSH string must have the following format: '<username>:<password>@<url>:<port>', not '{string}'."
			)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Traefik deployment.")
	parser.add_argument("--json", type=pathlib.Path, help="Path of the manifest.")
	args = parser.parse_args()

	data = json.loads(args.json.read_text())
	ast = Ast(data)

	template = Template.fromPath(pathlib.Path(__file__).parent / "docker_compose.yml.btl")
	content = template.render(Getter(ast))

	print(content)
