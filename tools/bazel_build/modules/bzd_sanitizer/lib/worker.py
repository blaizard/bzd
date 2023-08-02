import argparse
import pathlib
import typing
import sys

from lib.context import Context
from bzd.utils.worker import Worker


def worker(contextPath: pathlib.Path,
           workload: typing.Callable[[typing.Tuple[str, ...], typing.TextIO], bool],
           args: typing.Optional[typing.List[str]] = None,
           timeoutS: int = 60,
           **filters) -> bool:
	"""Execute in parallel multiple workloads and report errors if needed."""

	context = Context.fromFile(contextPath)
	worker = Worker(workload)
	isFailure = False

	with worker.start(throwOnException=False) as w:
		for f in context.data(**filters):
			w.add([context.workspace, f, context.check] + ([] if args is None else args), timeoutS=timeoutS)
		for result in w.data():
			if (not result.getResult()) or result.isException():
				isFailure = True
				path = str(result.getInput()[1])
				context.printSection(title=path, level=2)
				print(result.getOutput(), end="")

	sys.exit(1 if isFailure else 0)
