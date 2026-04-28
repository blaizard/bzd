import argparse
import pathlib
import tarfile
import json

def packageNameFromKey(key: str) -> str:
	return key.rsplit("@", 1)[0]

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description = "Generate a node_modules directory.")
	parser.add_argument("dependencies", type = pathlib.Path, help = "The dependencies.json file containing the list of packages to be installed.")
	parser.add_argument("node_modules", type = pathlib.Path, help = "The output node_modules directory to be populated.")
	args = parser.parse_args()

	dependencies = json.loads(args.dependencies.read_text())
	tars = [[package, pathlib.Path(path)] for packages in dependencies.values() for package, path in packages.items()]
	tars.sort(key = lambda entry: entry[0])

	alreadyVisited = set()
	for package, path in tars:
		name = packageNameFromKey(package)
		if name in alreadyVisited:
			continue
		alreadyVisited.add(name)
		with tarfile.open(path, "r") as tar:
			for member in tar.getmembers():
				parts = pathlib.Path(member.name).parts
				if len(parts) <= 1:
					continue
				member.name = str(pathlib.Path(*parts[1:]))
				tar.extract(member, args.node_modules / name, filter="data")
