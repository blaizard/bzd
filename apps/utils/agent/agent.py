"""Run a headless agent."""

import argparse
import json
import os
import pathlib
import sys
import typing

from apps.utils.json.json import loadAndRepair
from bzd.utils.run import localCommand


def opencode(cwd: pathlib.Path, agent: str, prompt: str, continueSession: bool) -> typing.Optional[str]:
	"""Run opencode."""

	print("==== prompt ================================================", flush=True)
	print(prompt, flush=True)
	print("==== agent =================================================", flush=True)
	result = localCommand(
		[
			"opencode",
			"run",
			"--auto",
			"--agent",
			agent,
			*(["--continue"] if continueSession else []),
			prompt,
		],
		stdout=True,
		stderr=True,
		ignoreFailure=True,
		cwd=cwd,
		timeoutS=None,
	)

	if result.isFailure():
		logDir = pathlib.Path.home() / ".local" / "share" / "opencode" / "log"
		logFiles = sorted(logDir.glob("*.log"), key=lambda p: p.stat().st_mtime, reverse=True)
		print(f"==== opencode failed ({result.getReturnCode()}), dumping last logs ==================", flush=True)
		if logFiles:
			lines = logFiles[0].read_text(errors="replace").splitlines()
			print("\n".join(lines[-100:]), flush=True)
		else:
			print("No log found.", flush=True)
		return None

	output = result.getStdout()
	if len(output.strip()) == 0:
		print("No output.", flush=True)
		return None

	return output


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Run a headless agent.")
	parser.add_argument("--output", type=pathlib.Path, help="Save the output to a file.")
	parser.add_argument(
		"--cwd",
		type=pathlib.Path,
		default=pathlib.Path(os.environ.get("BUILD_WORKSPACE_DIRECTORY", ".")),
		help="The working directory.",
	)
	parser.add_argument("--max-retries", type=int, default=3, help="Maximum number of retries on failure.")
	parser.add_argument("--agent", default="build", help="The type of agent to be used.")
	parser.add_argument(
		"--expect-json",
		action="store_true",
		help="Expect the output to contain a valid JSON object or array, retry otherwise.",
	)
	parser.add_argument("prompt", help="The prompt to be used.")
	args = parser.parse_args()

	continueSession = False
	prompt = args.prompt
	retry = 0
	output = None

	while retry < args.max_retries:
		if retry > 0:
			print(f"retrying {retry}/{args.max_retries}", flush=True)
		output = opencode(
			cwd=args.cwd,
			agent=args.agent,
			prompt=prompt,
			continueSession=continueSession,
		)

		if output is None:
			prompt = "continue"

		# Handle json output
		elif args.expect_json:
			try:
				data = loadAndRepair(output)
				output = json.dumps(data, indent=4)
				break
			except ValueError as e:
				print(str(e), flush=True)
				output = None
			prompt = "A JSON output is expected. Re-output your final answer as a single valid JSON object and nothing else."

		# Handle raw output
		else:
			break

		print("==== error =================================================", flush=True)
		retry += 1
		continueSession = True

	if output is None:
		print("No output.", flush=True)
		sys.exit(1)

	if args.output:
		args.output.write_text(output)

	sys.exit(0)
