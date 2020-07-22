import os
import sys
import queue
import time
import multiprocessing
from io import StringIO
from typing import Optional, Iterable, Any


class _WorkerResult:

	def __init__(self, output) -> None:
		self.output = output

	def isSuccess(self) -> bool:
		return self.output[0]

	def getResult(self):
		return self.output[1]

	def getOutput(self) -> str:
		return self.output[2]


class Worker:

	def __init__(self, task, maxWorker: Optional[int] = os.cpu_count()) -> None:
		self.shared = {
			"stop": multiprocessing.Value("i", 0),
			"count": multiprocessing.Value("i", 0),
			"output": multiprocessing.Queue(),
			"data": multiprocessing.Queue()
		}
		assert maxWorker
		self.workerList = [
			multiprocessing.Process(target=Worker._taskWrapper, args=(i, task, self.shared)) for i in range(maxWorker)
		]
		self.expectedData = 0

	@staticmethod
	def _taskWrapper(i, task, shared):

		# Loop unless the workers are notified to be stopped by the master process
		while shared["count"].value > 0 or shared["stop"].value == 0:

			# Wait until there is data available or stop is raised
			try:
				data = shared["data"].get_nowait()
				with shared["count"].get_lock():
					shared["count"].value -= 1
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
				stdout.write("Failed with exception of type '{}' and message: {}\n".format(
					exceptionType.__name__, exceptionValue))

			shared["output"].put((isSuccess, result, stdout.getvalue()))

	def add(self, data: Any) -> None:
		with self.shared["count"].get_lock():  # type: ignore
			self.shared["count"].value += 1  # type: ignore
		self.expectedData += 1
		self.shared["data"].put(data)  # type: ignore

	def start(self) -> None:
		for worker in self.workerList:
			worker.start()

	def data(self) -> Iterable[_WorkerResult]:
		while self.expectedData > 0:
			workerResult = _WorkerResult(self.shared["output"].get())  # type: ignore
			self.expectedData -= 1
			yield workerResult

	def stop(self) -> None:
		self.shared["stop"].value = 1  # type: ignore
		for worker in self.workerList:
			worker.join()
