import pathlib
import typing
import argparse
import os
import dateutil.parser
import datetime

from bzd.logging import Logger
from apps.artifacts.api.python.node.node import Node

logger = Logger("apps.plugin.nodes")


def stringToTimestampMs(string: str) -> int:
	"""Convert a time string into a timestamp in milliseconds."""

	date = dateutil.parser.parse(string)  #  type: ignore
	return int(date.timestamp() * 1000)


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Export node data.")
	parser.add_argument("--children", type=int, default=99, help="Number of children to be used.")
	parser.add_argument("--format", type=str, default="csv", choices=["csv"], help="Output format.")
	parser.add_argument("--after", type=str, help="Initial date from when to export.")
	parser.add_argument("--before", type=str, help="Final date from when to export.")
	parser.add_argument("--output",
	                    type=pathlib.Path,
	                    default=pathlib.Path("output"),
	                    help="The output file or directory where to store the result.")
	parser.add_argument(
	    "--rename",
	    type=str,
	    help=
	    "Rename the output using the format-like schema. 'key' corresponds to the key as a list, while 'stem' to the expected file stem as a string."
	)
	parser.add_argument(
	    "--level",
	    type=int,
	    default=0,
	    help=
	    "At which level from the given path the data should be extracted, by default everything will be a single file. If > 0, multiple files are created."
	)
	parser.add_argument("url", help="The URL to be exported.")

	args = parser.parse_args()

	node = Node.fromUrl(args.url)
	after = None if args.after is None else stringToTimestampMs(args.after)
	before = None if args.before is None else stringToTimestampMs(args.before)

	toProcess = []
	if args.level > 0:
		result = node.get(children=args.level - 1, keys=True)
		for info in result["data"]:
			# Ignore keys that are not at the specified level.
			if len(info["key"]) != args.level:
				continue
			key = info["key"]
			stem = "-".join(key)
			stem = stem if args.rename is None else args.rename.format(key=key, stem=stem)
			toProcess.append([key, (args.output / stem).with_suffix(f".{args.format}")])
	else:
		toProcess = [[[], args.output.with_suffix(f".{args.format}")]]

	logger.info(f"Exporting {len(toProcess)} file(s) using {str(node)}")
	if after is not None:
		logger.info(f"Starting at {str(datetime.datetime.fromtimestamp(after / 1000., tz=datetime.timezone.utc))}")
	if before is not None:
		logger.info(f"Ending at {str(datetime.datetime.fromtimestamp(before / 1000., tz=datetime.timezone.utc))}")

	for index, [key, path] in enumerate(toProcess):
		logger.info(f"[{index + 1}/{len(toProcess)}] exporting {path}")
		content = node.export(path=key, children=args.children, format=args.format, before=before, after=after)
		path.parent.mkdir(parents=True, exist_ok=True)
		path.write_bytes(content)
