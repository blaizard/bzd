import argparse
import pathlib
import sys
import json
import typing
import os

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Update the compile-command.json script.")
	parser.add_argument(
	    "output",
	    type=pathlib.Path,
	    help="The file to be generated by this action.",
	)
	parser.add_argument(
	    "srcs",
	    nargs="+",
	    type=pathlib.Path,
	    help="The files to be processed.",
	)

	# Split the arguments between <cmd_args> -- <extra_args>
	raw_args = sys.argv[1:]
	cmd_args = raw_args[:raw_args.index("--")]
	extra_args = raw_args[raw_args.index("--") + 1:]

	args = parser.parse_args(cmd_args)

	compile_commands = []
	for src in args.srcs:
		compile_commands.append({
		    "command": " ".join(extra_args) + " -c " + str(src),
		    "directory": ".",
		    "file": str(src),
		})

	args.output.write_text(json.dumps(compile_commands))