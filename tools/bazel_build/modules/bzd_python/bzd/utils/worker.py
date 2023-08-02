import os
import sys
import queue
import time
import multiprocessing
import traceback
from io import StringIO
from typing import Optional, Iterable, Any, Tuple, TextIO, Callable, Mapping
import ctypes
import signal
import types


class _WorkerResult:

	def __init__(self, output: Tuple[bool, Any, str, Any]) -> None:
		self.output = output

	def isException(self) -> bool:
		return self.output[0]

	def getResult(self) -> Any:
		return self.output[1]

	def getOutput(self) -> str:
		return self.output[2]

	def getInput(self) -> Any:
		return self.output[3]


class _WorkloadContext:
	"""Context related to a specific work."""

	def __init__(self, data: Any, timeoutS: int) -> None:
		self.data = data
		self.timeoutS = timeoutS


class _Context:
	"""Global context for the worker."""

	def __init__(self) -> None:
		self.stop = multiprocessing.Value(ctypes.c_int, 0)
		self.count = multiprocessing.Value(ctypes.c_int, 0)
		self.output: multiprocessing.queues.Queue[Tuple[bool, Optional[Any], str, Any]] = multiprocessing.Queue()
		self.data: multiprocessing.queues.Queue[_WorkloadContext] = multiprocessing.Queue()


class WorkerContextManager:

	def __init__(self, worker: "Worker", throwOnException: bool) -> None:
		self.worker = worker
		self.throwOnException = throwOnException
		self.isException = False

	def __enter__(self) -> "WorkerContextManager":
		return self

	def __exit__(self, exc_type, exc_value, exc_tb) -> None:
		for _ in self.data():
			pass
		# Stop and join the processes.
		self.worker.context.stop.value = 1  # type: ignore
		for worker in self.worker.workerList:
			worker.join()
		# Throw.
		if self.throwOnException and self.isException:
			raise Exception(f"At least one of the worker failed.")

	def data(self) -> Iterable[_WorkerResult]:
		while self.worker.expectedData > 0:
			result = _WorkerResult(self.worker.context.output.get())
			self.worker.expectedData -= 1
			if self.throwOnException and result.isException():
				self.isException = True
				print(result.getOutput(), end="")
			yield result

	def add(self, data: Any, timeoutS: int = 0) -> None:
		"""Add a new workload to processed.

		Args:
			data: The data to be passed to the workload.
			timeoutS: The timeout for this workload. By default there is no timeout.
		"""
		with self.worker.context.count.get_lock():
			self.worker.context.count.value += 1  # type: ignore
		self.worker.expectedData += 1
		self.worker.context.data.put(_WorkloadContext(data, timeoutS))


class Worker:

	def __init__(self, task: Callable[[Any, TextIO], Any], maxWorker: Optional[int] = os.cpu_count()) -> None:
		self.context = _Context()
		assert maxWorker
		self.workerList = [
		    multiprocessing.Process(target=Worker._taskWrapper, args=(task, self.context)) for i in range(maxWorker)
		]
		self.expectedData = 0

	@staticmethod
	def _taskWrapper(task: Callable[[Any, TextIO], Any], context: _Context) -> None:

		# Loop unless the workers are notified to be stopped by the master process
		while context.count.value > 0 or context.stop.value == 0:  # type: ignore

			# Wait until there is data available or stop is raised
			try:
				workloadContext = context.data.get_nowait()
				with context.count.get_lock():
					context.count.value -= 1  # type: ignore
			except queue.Empty:
				time.sleep(0.001)
				continue

			# Run the stask
			isException = False
			stdout = StringIO()
			result = None

			# Handle timeout if needed
			if workloadContext.timeoutS > 0:

				def _timeoutHandler(signum: int, frame: Optional[types.FrameType]) -> None:
					raise TimeoutError(f"Function timed out after {workloadContext.timeoutS}s.")

				signal.signal(signal.SIGALRM, _timeoutHandler)
				signal.alarm(workloadContext.timeoutS)

			try:
				result = task(workloadContext.data, stdout)
			except:
				signal.alarm(0)
				isException = True
				exceptionType, exceptionValue = sys.exc_info()[:2]
				if isinstance(exceptionType, BaseException):
					exceptionTypeStr = exceptionType.__name__
				else:
					exceptionTypeStr = str(exceptionType)
				stdout.write("Failed with exception of type '{}' and message: {}\n".format(
				    exceptionTypeStr, exceptionValue))
				stdout.write(traceback.format_exc())

			context.output.put((isException, result, stdout.getvalue(), workloadContext.data))

	def start(self, throwOnException: bool = True) -> WorkerContextManager:
		for worker in self.workerList:
			worker.start()
		return WorkerContextManager(self, throwOnException=throwOnException)
