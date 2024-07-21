import argparse
import pathlib
import json
import ssl

from bzd.http.client import HttpClient

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
	    description="Release an artifact. The artifact will be uploaded with the filename <version>-<name>")
	parser.add_argument(
	    "--config",
	    type=pathlib.Path,
	    help="The configuration in json format.",
	)
	parser.add_argument(
	    "--config-version",
	    type=str,
	    help="The key of the version field within the configuration.",
	)
	parser.add_argument(
	    "artifact",
	    type=pathlib.Path,
	    help="The artifact to be released.",
	)
	parser.add_argument(
	    "url",
	    type=str,
	    help="The url where to release.",
	)

	args = parser.parse_args()

	fileName = args.artifact.name
	if args.config:
		configDict = json.loads(args.config.read_text())
		if args.config_version:
			fileName = f"{configDict[args.config_version]}-{fileName}"

	url = args.url + "/" + fileName

	print(f"Uploading {url}...", flush=True, end="")
	HttpClient.put(
	    url=args.url + "/" + fileName,
	    file=args.artifact,
	)
	print(" done.")
