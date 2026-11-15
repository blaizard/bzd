import pathlib

from tools.ci.factory import (
	ConfigCoverage,
	ConfigNormal,
	ConfigSanitizer,
	ConfigStress,
	Factory,
)


class GitHub(Factory):
	def __init__(self) -> None:
		self.jenkinsFile: str | None = None

	def getName(self) -> str:
		return "GitHub Actions"

	def getConfigNormal(self, variant: str | None) -> ConfigNormal | None:
		return ConfigNormal()

	def getConfigStress(self, variant: str | None) -> ConfigStress | None:
		return None

	def getConfigCoverage(self, variant: str | None) -> ConfigCoverage | None:
		return ConfigCoverage(exclude={"cc"})

	def getConfigSanitizer(self, variant: str | None) -> ConfigSanitizer | None:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/github/ci.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / ".github/workflows/ci.yml").write_text(self.jenkinsFile)
