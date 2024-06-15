import argparse
import pathlib
import typing
import sys
import io
import dataclasses

from bzd_sanitizer.context import Context
from bzd.utils.worker import Worker


@dataclasses.dataclass
class OutputWithPath:
	"""Result when transforming the output to filter errors."""

	path: pathlib.Path
	output: str


def chunkWorker(contextPath: pathlib.Path,
                workload: typing.Callable[[typing.Tuple[str, ...], typing.TextIO], bool],
                args: typing.Optional[typing.List[str]] = None,
                timeoutS: int = 60,
                maxFiles: int = 256,
                stdoutParser: typing.Optional[typing.Callable[[str, pathlib.Path], typing.List[OutputWithPath]]] = None,
                **filters: typing.Any) -> bool:
	"""Execute a single workload at a time with multiple files and report errors if needed."""

	context = Context.fromFile(contextPath)

	isFailure = False
	failureOutputs = []
	queue = [f for f in context.data(**filters)]
	for i in range(0, len(queue), maxFiles):
		paths = queue[i:i + maxFiles]
		stdout = io.StringIO()
		if not workload([context.workspace, paths, context.check] + (args or []), stdout):
			isFailure = True
			stdout.seek(0)
			failureOutputs.append(stdout.read().strip())

	# Print the error if any
	if isFailure:
		if stdoutParser:
			outputsByPath = {}
			for output in failureOutputs:
				for item in [*stdoutParser(output, context.workspace)]:
					outputsByPath.setdefault(item.path, []).append(item.output)
			for path, outputs in sorted(outputsByPath.items(), key=lambda x: x[0]):
				context.printSection(title=str(path), level=2)
				print("\n".join(outputs))
		else:
			for output in failureOutputs:
				print(output)

	sys.exit(1 if isFailure else 0)


def worker(contextPath: pathlib.Path,
           workload: typing.Callable[[typing.Tuple[str, ...], typing.TextIO], bool],
           args: typing.Optional[typing.List[str]] = None,
           timeoutS: int = 60,
           **filters: typing.Any) -> bool:
	"""Execute in parallel multiple workloads and report errors if needed."""

	context = Context.fromFile(contextPath)
	utilsWorker = Worker(workload)
	isFailure = False

	with utilsWorker.start(throwOnException=False) as w:  # type: ignore
		for f in context.data(**filters):
			w.add(
			    [context.workspace, f, context.check] + (args or []),
			    timeoutS=timeoutS,
			)
		for result in w.data():
			if (not bool(result.getResult())) or result.isException():
				isFailure = True
				path = str(result.getInput()[1])
				context.printSection(title=path, level=2)
				print(result.getOutput().strip())

	sys.exit(1 if isFailure else 0)
