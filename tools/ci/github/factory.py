import pathlib
import typing

from tools.ci.factory import Factory, ConfigNormal, ConfigStress, ConfigCoverage, ConfigSanitizer


class GitHub(Factory):

	def __init__(self) -> None:
		self.jenkinsFile: typing.Optional[str] = None

	def getName(self) -> str:
		return "GitHub Actions"

	def getConfigNormal(self) -> typing.Optional[ConfigNormal]:
		return ConfigNormal(exclude=["esp32"])

	def getConfigStress(self) -> typing.Optional[ConfigStress]:
		return None

	def getConfigCoverage(self) -> typing.Optional[ConfigCoverage]:
		return None

	def getConfigSanitizer(self) -> typing.Optional[ConfigSanitizer]:
		return ConfigSanitizer()

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/github/ci.btl"))

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / ".github/workflows/ci.yml").write_text(self.jenkinsFile)
