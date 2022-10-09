import os
import sys
import queue
import time
import multiprocessing
import traceback
from io import StringIO
from typing import Optional, Iterable, Any, Tuple, TextIO, Callable, Mapping
import ctypes


class _WorkerResult:

	def __init__(self, output: Tuple[bool, Any, str, Any]) -> None:
		self.output = output

	def isSuccess(self) -> bool:
		return self.output[0]

	def getResult(self) -> Any:
		return self.output[1]

	def getOutput(self) -> str:
		return self.output[2]

	def getInput(self) -> Any:
		return self.output[3]


class _Context:

	def __init__(self) -> None:
		self.stop = multiprocessing.Value(ctypes.c_int, 0)
		self.count = multiprocessing.Value(ctypes.c_int, 0)
		self.output: multiprocessing.queues.Queue[Tuple[bool, Optional[Any], str, Any]] = multiprocessing.Queue()
		self.data: multiprocessing.queues.Queue[Any] = multiprocessing.Queue()


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
				data = context.data.get_nowait()
				with context.count.get_lock():
					context.count.value -= 1  # type: ignore
			except queue.Empty:
				time.sleep(0.001)
				continue

			# Run the stask
			isSuccess = True
			stdout = StringIO()
			result = None
			try:
				result = task(data, stdout)
			except:
				isSuccess = False
				exceptionType, exceptionValue = sys.exc_info()[:2]
				if isinstance(exceptionType, BaseException):
					exceptionTypeStr = exceptionType.__name__
				else:
					exceptionTypeStr = str(exceptionType)
				stdout.write("Failed with exception of type '{}' and message: {}\n".format(
					exceptionTypeStr, exceptionValue))
				stdout.write(traceback.format_exc())

			context.output.put((isSuccess, result, stdout.getvalue(), data))

	def add(self, data: Any) -> None:
		with self.context.count.get_lock():
			self.context.count.value += 1  # type: ignore
		self.expectedData += 1
		self.context.data.put(data)

	def start(self) -> None:
		for worker in self.workerList:
			worker.start()

	def data(self) -> Iterable[_WorkerResult]:
		while self.expectedData > 0:
			workerResult = _WorkerResult(self.context.output.get())
			self.expectedData -= 1
			yield workerResult

	def stop(self) -> None:
		self.context.stop.value = 1  # type: ignore
		for worker in self.workerList:
			worker.join()
