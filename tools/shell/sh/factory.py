import os
import logging
import pathlib
import typing
import re
import shutil

from tools.shell.factory import Factory


class Sh(Factory):

	def __init__(self) -> None:
		self.path: typing.Optional[pathlib.Path] = None

	def getName(self) -> str:
		"""Give the name of the shell."""

		return "Sh-compatible shell"

	def isCompatible(self) -> bool:
		"""Tell if this is a sh-compatibel shell."""

		return os.environ.get("SHELL", "").endswith((
		    "/sh",
		    "/bash",
		))

	def build(self, workspace: pathlib.Path) -> None:
		"""Build the configuration if needed."""

		content = self.renderTemplate(
		    pathlib.Path("tools/shell/sh/srcs/bashrc.btl"), {
		        "always": pathlib.Path("tools/shell/sh/srcs/always"),
		        "interactive": pathlib.Path("tools/shell/sh/srcs/interactive")
		    })
		self.path = workspace / "tools/shell/sh/bashrc.sh"
		self.path.write_text(content)

	def install(self) -> None:
		"""Install the shell."""

		# Look for the current .bashrc
		assert "HOME" in os.environ, "HOME environment variable is not set."
		home = pathlib.Path(os.environ["HOME"])
		assert home.is_dir(), "Directory does not exists."
		hookFile = home / ".bashrc"
		if not hookFile.is_file():
			logging.info(f"Creating {hookFile}.")
			hookFile.touch()

		# Update the hook file.
		content = hookFile.read_text()
		hook = "# bzd-hook-start\n# This is a generate hook from the bzd monorepo, please do not remove.\nsource ~/.bzd/bashrc.sh\n# bzd-hook-end\n"
		updateContent = re.sub(r"^#\s*bzd-hook-start[\s\S]*\n#\s*bzd-hook-end\n",
		                       hook,
		                       content,
		                       count=1,
		                       flags=re.MULTILINE)
		if id(content) == id(updateContent):
			updateContent += "\n" + hook
		if content != updateContent:
			logging.info(f"Updating {hookFile}.")
			hookFile.write_text(updateContent)

		# Replace the content of the directory.
		assert self.path
		self.createEmptyDirectory(home / ".bzd")
		shutil.copy(self.path, home / ".bzd/bashrc.sh")

		logging.info("To take immediate effect, run 'source ~/.bashrc'.")
