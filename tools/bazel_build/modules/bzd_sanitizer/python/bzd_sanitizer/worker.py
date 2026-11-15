import dataclasses
import io
import pathlib
import sys
import typing

from bzd.utils.worker import Worker

from bzd_sanitizer.context import Context

Ts = typing.TypeVarTuple("Ts")


@dataclasses.dataclass
class OutputWithPath:
	"""Result when transforming the output to filter errors."""

	path: pathlib.Path
	output: str


def chunkWorker(
	contextPath: pathlib.Path,
	workload: typing.Callable[
		[
			tuple[pathlib.Path, typing.Sequence[pathlib.Path], bool, *Ts],
			typing.TextIO,
		],
		bool,
	],
	args: tuple[*Ts,] | None = None,
	timeoutS: int = 60,
	maxFiles: int = 256,
	stdoutParser: typing.Callable[[str, pathlib.Path], typing.Iterator[OutputWithPath]] | None = None,
	**filters: typing.Any,
) -> bool:
	"""Execute a single workload at a time with multiple files and report errors if needed."""

	context = Context.fromFile(contextPath)

	isFailure = False
	failureOutputs = []
	queue = [f for f in context.data(**filters)]
	for i in range(0, len(queue), maxFiles):
		paths = queue[i : i + maxFiles]
		stdout = io.StringIO()
		if not workload(
			[context.workspace, paths, context.check] + list(args or []),  # type: ignore
			stdout,
		):
			isFailure = True
			stdout.seek(0)
			failureOutputs.append(stdout.read().strip())

	# Print the error if any
	if isFailure:
		if stdoutParser:
			outputsByPath: dict[pathlib.Path, list[str]] = {}
			for output in failureOutputs:
				for item in [*stdoutParser(output, context.workspace)]:
					outputsByPath.setdefault(item.path, []).append(item.output)
			for path, outputs in sorted(outputsByPath.items(), key=lambda x: x[0]):
				try:
					actualPath = path.resolve(strict=True).relative_to(context.workspace)
					context.printSection(title=str(actualPath), level=2)
					print("\n".join(outputs))
				except Exception:  # noqa: BLE001, S110
					pass
		else:
			for output in failureOutputs:
				print(output)

	sys.exit(1 if isFailure else 0)


def worker(
	contextPath: pathlib.Path,
	workload: typing.Callable[[tuple[pathlib.Path, pathlib.Path, bool, *Ts], typing.TextIO], bool],
	args: tuple[*Ts,] | None = None,
	timeoutS: int = 60,
	**filters: typing.Any,
) -> bool:
	"""Execute in parallel multiple workloads and report errors if needed."""

	context = Context.fromFile(contextPath)
	utilsWorker = Worker(workload)
	isFailure = False

	with utilsWorker.start(throwOnException=False) as w:
		for f in context.data(**filters):
			w.add(
				data=[context.workspace, f, context.check] + list(args or []),  # type: ignore
				timeoutS=timeoutS,
			)
		for result in w.data():
			if (not bool(result.getResult())) or result.isException():
				isFailure = True
				path = str(result.getInput()[1])
				context.printSection(title=path, level=2)
				print(result.getOutput().strip())

	sys.exit(1 if isFailure else 0)
