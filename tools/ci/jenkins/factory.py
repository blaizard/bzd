import pathlib
import typing

from tools.ci.factory import Factory


class Jenkins(Factory):

	def __init__(self) -> None:
		self.jenkinsFile: typing.Optional[str] = None

	def getName(self) -> str:
		return "Jenkins"

	def build(self) -> None:
		self.jenkinsFile = self.renderTemplate(pathlib.Path("tools/ci/jenkins/Jenkinsfile.btl"),
			onlyCategories={"normal", "coverage", "sanitizer"})

	def install(self, workspace: pathlib.Path) -> None:
		assert self.jenkinsFile
		(workspace / "Jenkinsfile").write_text(self.jenkinsFile)
