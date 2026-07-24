import argparse
import json
import pathlib

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Test binary.")
	parser.add_argument("--json", type=pathlib.Path, help="Path of the manifest.")
	args = parser.parse_args()

	data = json.loads(args.json.read_text())
	print(json.dumps(data, indent=4, sort_keys=True))
