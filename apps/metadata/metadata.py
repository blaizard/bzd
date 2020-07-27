#!/usr/bin/python

import argparse
import re
import json
import os
import subprocess
from typing import Tuple, List, Mapping, Any


def printTable(data: List[List[str]], headers: List[str]) -> None:

	# Calculate column sizes
	columnSizes: List[int] = []
	for row in [headers] + data:
		for index, item in enumerate(row):
			if index < len(columnSizes):
				columnSizes[index] = max(columnSizes[index], len(str(item)))
			else:
				columnSizes.append(len(str(item)))

	# Print the data
	for row in [headers] + data:
		line = []
		for index, item in enumerate(row):
			fmt = "{:>" + str(columnSizes[index]) + "}"
			line.append(fmt.format(str(item)))
		print("  " + " | ".join(line))


def printMetadataCC(data: Mapping[str, Any]) -> None:
	print(" - Compilers: {}".format(", ".join(data.get("compilers", []))))

	for groupName, group in data.get("size_groups", {}).items():
		print(" - Size ({})".format(groupName))
		groups: List[List[str]] = []
		size = 0
		for unit, partSize in group.items():
			groups.append([unit, partSize])
			size += partSize
		size = data.get("size", size)
		groups.sort(key=lambda item: item[1], reverse=True)
		# Update the percentage
		for line in groups:
			line.append("{:.1%}".format(int(line[1]) / size))

		printTable(groups, headers=[groupName, "size", "percentage"])


def printMetadata(metadata: Mapping[str, Any]) -> None:
	for name, data in metadata.get("fragments", {}).items():
		print("Name: {}".format(name))
		if data.get("type") == "cc":
			printMetadataCC(data)


if __name__ == "__main__":

	parser = argparse.ArgumentParser(description="Metadata visualizer.")
	parser.add_argument("--bazel", default="bazel", help="Bazel binary path.")
	parser.add_argument("path", help="Path of the root or file to be visualized.")

	args = parser.parse_args()

	metadataList = []

	# If directory, look for all metadata
	if os.path.isdir(args.path):

		proc = subprocess.run([args.bazel, "query", "kind(bzd_package, //...)"], cwd=args.path, stdout=subprocess.PIPE)
		targets = proc.stdout.decode("ascii").strip().split("\n")
		for target in targets:
			m = re.match(r'^//(.*):(.*)$', target)
			assert m, "Unable to match: {}".format(target)
			metadataList.append("{}/bazel-bin/{}/{}.metadata.manifest".format(args.path, m.group(1), m.group(2)))

	# If file
	elif os.path.isfile(args.path):
		metadataList.append(args.path)

	else:
		raise Exception("Invalid path: {}".format(args.path))

	print("Monitoring metadata manifest(s):\n{}".format("\n".join(["  - " + metadata for metadata in metadataList])))
	for metadata in metadataList:
		try:
			with open(metadata, "r") as f:
				data = json.loads(f.read())
				print(metadata)
				printMetadata(data)
		except Exception as e:
			print("{}: {}".format(metadata, e))
