#!/usr/bin/env -S python3 -B
import argparse

from sandbox import SandboxContainer

if __name__ == "__main__":
	parser = argparse.ArgumentParser(
		formatter_class=argparse.ArgumentDefaultsHelpFormatter,
		description="Opencode.",
	)
	parser.add_argument("--id", type=int, default=1, help="The agent identifier (0-9).")
	parser.add_argument("--build", action="store_true", help="Re-build/clean the container if set.")
	parser.add_argument(
		"--build-force",
		action="store_true",
		help="Force re-build/clean the container if set.",
	)

	args, remaining = parser.parse_known_args()
	sandboxArgs = []
	if args.build:
		sandboxArgs += ["--build"]
	if args.build_force:
		sandboxArgs += ["--build-force"]
	SandboxContainer.fromCLI(
		["--preset", f"agent{args.id}", "--prefix", f"opencode{args.id}", *sandboxArgs, "opencode", *remaining]
	)
