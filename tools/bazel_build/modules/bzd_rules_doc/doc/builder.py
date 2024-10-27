import argparse
import pathlib
import json
import typing
import tempfile
import tarfile
import subprocess


def navigationToMkDocsList(root: pathlib.Path, navigation: typing.List[typing.Tuple[str, typing.Any]]) -> typing.List[str]:
	content = []
	for name, path in navigation:
		if isinstance(path, list):
			content.append(f"- {name}:")
			content.extend(navigationToMkDocsList(root=root, navigation=path))
		else:
			content.append(f"- {name}: {pathlib.Path(path).relative_to(root)}")
	return [f"  {line}" for line in content]


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="MkDocs documentation builder.")
	parser.add_argument("--navigation", type=str, default="[]", help="Navigation as a json string.")
	parser.add_argument("--mkdocs", type=pathlib.Path, default="mkdocs", help="Path of the mkdocs tool.")
	parser.add_argument(
	    "--root",
	    type=pathlib.Path,
	    default=".",
	    help="Root directory for the documentation.",
	)
	parser.add_argument("output", type=str, help="Output package name.")

	args = parser.parse_args()

	navigation = json.loads(args.navigation)

	# Create the mkdocs.yml file.
	template = (pathlib.Path(__file__).parent / "mkdocs.yml.template").read_text()
	output = template.format(
	    navigation="\n".join(navigationToMkDocsList(root=args.root, navigation=navigation)),
	    root=args.root.as_posix(),
	)
	pathlib.Path("./mkdocs.yml").write_text(output)

	# Generate the site with mkdocs and package everything.
	with tempfile.TemporaryDirectory() as path:
		subprocess.run(
		    [
		        args.mkdocs,
		        "--",
		        "build",
		        "--strict",
		        "--config-file",
		        "mkdocs.yml",
		        "--site-dir",
		        path,
		    ],
		    capture_output=True,
		)
		with tarfile.open(args.output, "w:") as package:
			package.add(path, arcname="./")
