import argparse
import pathlib
import json
import tarfile
import tempfile

from bzd.http.client import HttpClient


def upload(url: str, artifact: pathlib.Path) -> None:
	print(f"Uploading {url} ({artifact.stat().st_size} bytes)...", flush=True, end="")
	HttpClient.put(
	    url=url,
	    file=artifact,
	    timeoutS=30 * 60  # 30min
	)
	print(" done.")


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

	if not args.artifact.exists():
		raise FileNotFoundError(args.artifact)
	elif args.artifact.is_dir():
		print(f"Creating tar.gz from directory {args.artifact}")
		with tempfile.NamedTemporaryFile() as fp:
			with tarfile.open(fp.name, "w:gz") as tar:
				tar.add(args.artifact)
			upload(url, pathlib.Path(fp.name))
	else:
		upload(url, args.artifact)
