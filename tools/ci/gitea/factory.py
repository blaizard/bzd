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
		self.content: typing.Optional[typing.Dict[str, str]] = None

	def getName(self) -> str:
		return "Gitea Actions"

	def getConfigNormal(self, variant: typing.Optional[str]) -> typing.Optional[ConfigNormal]:
		return ConfigNormal()

	def getConfigStress(self, variant: typing.Optional[str]) -> typing.Optional[ConfigStress]:
		if variant == "pr":
			return None
		return ConfigStress(runs=10)

	def getConfigCoverage(self, variant: typing.Optional[str]) -> typing.Optional[ConfigCoverage]:
		if variant == "pr":
			return None
		return ConfigCoverage(exclude={"cc"})

	def getConfigSanitizer(self, variant: typing.Optional[str]) -> typing.Optional[ConfigSanitizer]:
		return ConfigSanitizer()

	def build(self) -> None:
		self.content = {
			"deployment": self.renderTemplate(pathlib.Path("tools/ci/gitea/deployment.btl"), variant="deployment"),
			"pr": self.renderTemplate(pathlib.Path("tools/ci/gitea/pr.btl"), variant="pr"),
		}

	def install(self, workspace: pathlib.Path) -> None:
		assert self.content
		(workspace / ".gitea/workflows/deployment.yml").write_text(self.content["deployment"])
		(workspace / ".gitea/workflows/pr.yml").write_text(self.content["pr"])
