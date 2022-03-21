import bzd.env
import os
import logging
import pathlib
import re
import shutil


def copyTree(source: pathlib.Path, destination: pathlib.Path, symlinks: bool = False) -> None:
	"""Copy a files into a directory."""

	for item in os.listdir(source):
		s = source / item
		d = destination / item
		if s.is_dir():
			shutil.copytree(s, d, symlinks)
		else:
			shutil.copy2(s, d)


def replaceContent(directory: pathlib.Path, replaceWith: pathlib.Path) -> None:
	"""Replace the content of a directory with another.
    Create the directory if it does not exists.
    """

	# If the directory already exists, remove it.
	if directory.is_dir():
		shutil.rmtree(directory)
	directory.mkdir()

	# Copy the content
	copyTree(replaceWith, directory, symlinks=True)


def shellSh() -> None:
	"""Sh type of shell."""

	logging.info("Installing SH config.")

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
	replaceContent(home / ".bzd", pathlib.Path("tools/shell/sh"))

	logging.info("To take immediate effect, run 'source ~/.bashrc'.")


if __name__ == "__main__":

	# Look for the type of shell used on the current platform.
	if os.environ.get("SHELL", "").endswith((
		"/sh",
		"/bash",
	)):
		shellSh()

	else:
		logging.error("Unsupported shell type.")
