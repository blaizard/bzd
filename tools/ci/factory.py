import pathlib
import typing
import json
from abc import ABC, abstractmethod

from bzd.template.template import Template


class Factory(ABC):

	@abstractmethod
	def getName(self) -> str:
		"""Give the name of the shell."""
		pass

	@abstractmethod
	def build(self) -> None:
		"""Build the CI files."""
		pass

	@abstractmethod
	def install(self, workspace: pathlib.Path) -> None:
		"""Install the CI files to their actual destination."""
		pass

	@staticmethod
	def renderTemplate(templatePath: pathlib.Path) -> str:
		"""Render a template."""

		rawData = json.loads(pathlib.Path("tools/ci/stages.json").read_text())
		data = []
		for stage in rawData["stages"]:
			default = {"category": "normal"}
			default.update(stage)
			data.append(default)

		template = Template.fromPath(templatePath)
		return template.render({"stages": data})
