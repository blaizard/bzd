import argparse
import pathlib
import json

from bzd.http.client import HttpClient

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
	    description="Upload an artifact. The artifact will be uploaded with the filename <version>-<name>")
	parser.add_argument(
	    "--config",
	    type=pathlib.Path,
	    required=True,
	    help="The configuration in json format.",
	)
	parser.add_argument(
	    "--config-version",
	    type=str,
	    required=True,
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

	# Generate the filename.
	configDict = json.loads(args.config.read_text())
	fileName = f"{configDict[args.config_version]}-{args.artifact.name}"

	url = args.url + "/" + fileName

	print(f"Uploading {url} ({args.artifact.stat().st_size} bytes)...", flush=True, end="")
	HttpClient.put(
	    url=args.url + "/" + fileName,
	    file=args.artifact,
	    timeoutS=30 * 60  # 30min
	)
	print(" done.")
