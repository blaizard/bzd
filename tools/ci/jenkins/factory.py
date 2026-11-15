import pathlib

from tools.ci.factory import (
	ConfigCoverage,
	ConfigNormal,
	ConfigSanitizer,
	ConfigStress,
	Factory,
)


class Jenkins(Factory):
	def __init__(self) -> None:
		self.jenkinsFile: str | None = None

	def getName(self) -> str:
		return "Jenkins"

	def getConfigNormal(self, variant: str | None) -> ConfigNormal | None:
		return ConfigNormal()

	def getConfigStress(self, variant: str | None) -> ConfigStress | None:
		return ConfigStress(runs=10)

	def getConfigCoverage(self, variant: str | None) -> ConfigCoverage | None:
		return ConfigCoverage(exclude={"cc"})

	def getConfigSanitizer(self, variant: str | None) -> ConfigSanitizer | None:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/jenkins/Jenkinsfile.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / "Jenkinsfile").write_text(self.jenkinsFile)
