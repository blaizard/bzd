import argparse
import pathlib
import typing
import sys
import time

from bzd.utils.run import localCommand

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Test an executable.")
	parser.add_argument(
	    "--expected-returncode",
	    type=int,
	    help="The expected return code of the executable.",
	)
	parser.add_argument(
	    "--expected-output",
	    type=str,
	    help="The executable outputs the given string.",
	)
	parser.add_argument(
	    "--min-duration",
	    type=float,
	    help="The minimum execution duration in seconds of the executable.",
	)
	parser.add_argument(
	    "--max-duration",
	    type=float,
	    help="The maximum execution duration in seconds of the executable.",
	)
	parser.add_argument(
	    "executable",
	    type=pathlib.Path,
	    help="The executable to be tested.",
	)
	parser.add_argument(
	    "rest",
	    nargs=argparse.REMAINDER,
	    default=[],
	    help="The extra arguments to be passed to the executable.",
	)
	args = parser.parse_args()

	startTime = time.time()
	result = localCommand(cmds=[str(args.executable)] + args.rest, ignoreFailure=True, stdout=True, stderr=True)
	duration = time.time() - startTime

	nbChecks = 0

	if args.expected_returncode is not None:
		nbChecks += 1
		assert result.getReturnCode(
		) == args.expected_returncode, f"The return code {result.getReturnCode()} is different from the expected return code {args.expected_returncode}."

	if args.expected_output is not None:
		nbChecks += 1
		assert args.expected_output in result.getStdout() or args.expected_output in result.getStderr(
		), f"The string '{args.expected_output}' cannot be found in the output of the executable."

	if args.min_duration is not None:
		nbChecks += 1
		assert duration >= args.min_duration, f"The execution time was less than the expected minimal duration {args.min_duration}s vs {duration}s."

	if args.max_duration is not None:
		nbChecks += 1
		assert duration <= args.max_duration, f"The execution time was more than the expected maximal duration {args.max_duration}s vs {duration}s."

	attributes = ["expected_returncode", "expected_output", "min_duration", "max_duration"]
	assert nbChecks > 0, f"Please set at least one of the attributes: {', '.join(attributes)}."

	sys.exit(0)
