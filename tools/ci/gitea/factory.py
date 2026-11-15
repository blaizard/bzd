import pathlib

from tools.ci.factory import (
	ConfigCoverage,
	ConfigNormal,
	ConfigSanitizer,
	ConfigStress,
	Factory,
)


class Gitea(Factory):
	def __init__(self) -> None:
		self.content: dict[str, str] | None = None

	def getName(self) -> str:
		return "Gitea Actions"

	def getConfigNormal(self, variant: str | None) -> ConfigNormal | None:
		return ConfigNormal()

	def getConfigStress(self, variant: str | None) -> ConfigStress | None:
		if variant == "pr_gate":
			return None
		return ConfigStress(runs=10)

	def getConfigCoverage(self, variant: str | None) -> ConfigCoverage | None:
		if variant == "pr_gate":
			return None
		return ConfigCoverage(exclude={"cc"})

	def getConfigSanitizer(self, variant: str | None) -> ConfigSanitizer | None:
		return ConfigSanitizer()

	def build(self) -> None:
		self.content = {
			"ci_cd": self.renderTemplate(pathlib.Path("tools/ci/gitea/ci_cd.btl"), variant="ci_cd"),
			"pr_gate": self.renderTemplate(pathlib.Path("tools/ci/gitea/pr_gate.btl"), variant="pr_gate"),
		}

	def install(self, workspace: pathlib.Path) -> None:
		assert self.content
		(workspace / ".gitea/workflows/ci_cd.yml").write_text(self.content["ci_cd"])
		(workspace / ".gitea/workflows/pr_gate.yml").write_text(self.content["pr_gate"])
