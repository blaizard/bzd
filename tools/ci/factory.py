import pathlib
import typing
import json
from abc import ABC, abstractmethod
from dataclasses import dataclass, asdict, field

from bzd.template.template import Template


@dataclass
class ConfigBase_:
	"""Common configuration accross all specialization."""

	exclude: typing.Set[str] = field(default_factory=set)


@dataclass
class ConfigNormal(ConfigBase_):
	"""Configuration for normal tests."""

	name: typing.Final[str] = "normal"


@dataclass
class ConfigStress(ConfigBase_):
	"""Configuration for stress tests."""

	name: typing.Final[str] = "stress"
	runs: int = 100


@dataclass
class ConfigCoverage(ConfigBase_):
	"""Configuration for coverage tests."""

	name: typing.Final[str] = "coverage"


@dataclass
class ConfigSanitizer(ConfigBase_):
	"""Configuration for sanitizer tests."""

	name: typing.Final[str] = "sanitizer"


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

	@abstractmethod
	def getConfigNormal(self) -> typing.Optional[ConfigNormal]:
		"""Return the configuration associated with normal tests."""
		pass

	@abstractmethod
	def getConfigStress(self) -> typing.Optional[ConfigStress]:
		"""Return the configuration associated with stress tests."""
		pass

	@abstractmethod
	def getConfigCoverage(self) -> typing.Optional[ConfigCoverage]:
		"""Return the configuration associated with coverage tests."""
		pass

	@abstractmethod
	def getConfigSanitizer(self) -> typing.Optional[ConfigSanitizer]:
		"""Return the configuration associated with sanitizer tests."""
		pass

	def renderTemplate(self, templatePath: pathlib.Path) -> str:
		"""Render a template."""

		configs = {
		    config.name: config
		    for config in [
		        self.getConfigNormal(),
		        self.getConfigStress(),
		        self.getConfigCoverage(),
		        self.getConfigSanitizer(),
		    ] if config is not None
		}

		rawData = json.loads(pathlib.Path("tools/ci/stages.json").read_text())
		data = []
		for stage in rawData["stages"]:
			item = {"category": "normal", "tags": []}
			item.update(stage)
			assert isinstance(item.get("name"), str), "Missing 'name' attribute, must be a string."
			assert isinstance(item.get("commands"), list), "Missing 'commands' attribute, must be a list."
			if item["category"] in configs:
				config = configs[typing.cast(str, item["category"])]
				# Check if not in excluded list.
				if any(tag in item["tags"] for tag in config.exclude):
					continue
				# Perform substitutions.
				item["name"] = typing.cast(str, item["name"]).format(**asdict(config))
				item["commands"] = [line.format(**asdict(config)) for line in item["commands"]]
				data.append(item)

		template = Template.fromPath(templatePath)
		return template.render({"stages": data})
