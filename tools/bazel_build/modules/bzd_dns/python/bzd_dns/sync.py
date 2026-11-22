import argparse
import pathlib
import sys
import tempfile
import typing

import yaml
from bzd.utils.secret import maybeDecrypt
from octodns.cmds.sync import main as octodnsMain

_SECRET_KEYS = ("token", "key_secret")


def prepare(config: dict[str, typing.Any]) -> dict[str, typing.Any]:
	provider = config["providers"]["provider"]
	for key in _SECRET_KEYS:
		if key in provider:
			provider[key] = maybeDecrypt(provider[key])
	return config


def main() -> None:
	parser = argparse.ArgumentParser(add_help=False, allow_abbrev=False)
	parser.add_argument("--config", type=pathlib.Path, required=True)
	args, remaining = parser.parse_known_args(sys.argv[1:])

	config = prepare(yaml.safe_load(args.config.read_text()))
	with tempfile.TemporaryDirectory() as tmp:
		prepared = pathlib.Path(tmp) / "config.yaml"
		prepared.write_text(yaml.dump(config))

		sys.argv = [sys.argv[0], f"--config-file={prepared}", *remaining]
		octodnsMain()


if __name__ == "__main__":
	main()
