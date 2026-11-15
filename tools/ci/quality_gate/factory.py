import pathlib

from tools.ci.factory import (
	ConfigCoverage,
	ConfigNormal,
	ConfigSanitizer,
	ConfigStress,
	Factory,
)


class QualityGate(Factory):
	def __init__(self) -> None:
		self.jenkinsFile: str | None = None

	def getName(self) -> str:
		return "Quality Gate"

	def getConfigNormal(self, variant: str | None) -> ConfigNormal | None:
		return ConfigNormal()

	def getConfigStress(self, variant: str | None) -> ConfigStress | None:
		return ConfigStress()

	def getConfigCoverage(self, variant: str | None) -> ConfigCoverage | None:
		return ConfigCoverage()

	def getConfigSanitizer(self, variant: str | None) -> ConfigSanitizer | None:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/quality_gate/quality_gate.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / "quality_gate").write_text(self.jenkinsFile)
