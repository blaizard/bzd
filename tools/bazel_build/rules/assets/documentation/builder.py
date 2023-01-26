import argparse
import pathlib
import json
import typing
import os
import tempfile
import tarfile

from bzd.template.template import Template
from bzd.utils.run import localCommand


def navigationToMkDocsList(navigation: typing.List[typing.Tuple[str, typing.Any]]) -> typing.List[str]:
	content = []
	for name, path in navigation:
		if isinstance(path, list):
			content.append(f"- {name}:")
			content.extend(navigationToMkDocsList(path))
		else:
			content.append(f"- {name}: {path}")
	return [f"  {line}" for line in content]


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="MkDocs documentation builder.")
	parser.add_argument("-n", "--navigation", type=str, default="[]", help="Navigation as a json string.")
	parser.add_argument("-m", "--mkdocs", type=str, default="mkdocs", help="Path of the mkdocs tool.")
	parser.add_argument("output", type=str, help="Output package name.")

	args = parser.parse_args()

	navigation = json.loads(args.navigation)

	# Create the mkdocs.yml file.
	template = Template.fromPath(pathlib.Path(__file__).parent / "mkdocs.yml.template", indent=True)
	output = template.render({"navigation": "\n".join(navigationToMkDocsList(navigation))})
	pathlib.Path("./mkdocs.yml").write_text(output)

	# Generate the site with mkdocs and package everything.
	with tempfile.TemporaryDirectory() as path:
		localCommand([args.mkdocs, "--", "build", "--strict", "--config-file", "mkdocs.yml", "--site-dir", path])
		with tarfile.open(args.output, "w:") as package:
			package.add(path, arcname="./")
