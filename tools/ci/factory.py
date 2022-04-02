import pathlib
import typing
import json
from abc import ABC, abstractmethod

from bzd.template.template import Template


class Factory(ABC):

	categories: typing.Final[typing.Set[str]] = {"normal", "stress", "coverage", "sanitizer"}

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
	def renderTemplate(templatePath: pathlib.Path, onlyCategories: typing.Optional[typing.Set[str]] = None) -> str:
		"""Render a template."""

		assert onlyCategories is None or all(c in Factory.categories
			for c in onlyCategories), "onlyCategories contains unsupported categories."

		rawData = json.loads(pathlib.Path("tools/ci/stages.json").read_text())
		data = []
		for stage in rawData["stages"]:
			item = {"category": "normal"}
			item.update(stage)
			assert item["category"] in Factory.categories
			assert isinstance(item.get("command"), str), "Missing 'command' attribute, must be a string."
			if onlyCategories is None or item["category"] in onlyCategories:
				data.append(item)

		template = Template.fromPath(templatePath)
		return template.render({"stages": data})
