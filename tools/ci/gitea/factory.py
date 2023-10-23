import pathlib
import typing

from tools.ci.factory import (
    Factory,
    ConfigNormal,
    ConfigStress,
    ConfigCoverage,
    ConfigSanitizer,
)


class Gitea(Factory):

	def __init__(self) -> None:
		self.jenkinsFile: typing.Optional[str] = None

	def getName(self) -> str:
		return "Gitea Actions"

	def getConfigNormal(self) -> typing.Optional[ConfigNormal]:
		return ConfigNormal()

	def getConfigStress(self) -> typing.Optional[ConfigStress]:
		return ConfigStress(runs=10)

	def getConfigCoverage(self) -> typing.Optional[ConfigCoverage]:
		return ConfigCoverage(exclude={"cc"})

	def getConfigSanitizer(self) -> typing.Optional[ConfigSanitizer]:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/gitea/ci.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / ".gitea/workflows/ci.yml").write_text(self.jenkinsFile)
