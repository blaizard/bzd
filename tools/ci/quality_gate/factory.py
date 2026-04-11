import pathlib
import typing

from tools.ci.factory import (
	Factory,
	ConfigNormal,
	ConfigStress,
	ConfigCoverage,
	ConfigSanitizer,
)


class QualityGate(Factory):
	def __init__(self) -> None:
		self.jenkinsFile: typing.Optional[str] = None

	def getName(self) -> str:
		return "Quality Gate"

	def getConfigNormal(self, variant: typing.Optional[str]) -> typing.Optional[ConfigNormal]:
		return ConfigNormal()

	def getConfigStress(self, variant: typing.Optional[str]) -> typing.Optional[ConfigStress]:
		return ConfigStress()

	def getConfigCoverage(self, variant: typing.Optional[str]) -> typing.Optional[ConfigCoverage]:
		return ConfigCoverage()

	def getConfigSanitizer(self, variant: typing.Optional[str]) -> typing.Optional[ConfigSanitizer]:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/quality_gate/quality_gate.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / "quality_gate.sh").write_text(self.jenkinsFile)
