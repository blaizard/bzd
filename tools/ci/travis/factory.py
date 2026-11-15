import pathlib

from tools.ci.factory import (
	ConfigCoverage,
	ConfigNormal,
	ConfigSanitizer,
	ConfigStress,
	Factory,
)


class Travis(Factory):
	def __init__(self) -> None:
		self.jenkinsFile: str | None = None

	def getName(self) -> str:
		return "Travis CI"

	def getConfigNormal(self, variant: str | None) -> ConfigNormal | None:
		return ConfigNormal(exclude={"esp32"})

	def getConfigStress(self, variant: str | None) -> ConfigStress | None:
		return None

	def getConfigCoverage(self, variant: str | None) -> ConfigCoverage | None:
		return None

	def getConfigSanitizer(self, variant: str | None) -> ConfigSanitizer | None:
		return ConfigSanitizer(exclude={"static_analyzer"})

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/travis/.travis.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / ".travis.yml").write_text(self.jenkinsFile)
