import os
import sys
import queue
import multiprocessing
from io import StringIO
from typing import Optional


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
	def __init__(self, task,
		maxWorker: Optional[int] = os.cpu_count()) -> None:
		self.shared = {
			"stop": multiprocessing.Value("i", 0),
			"output": multiprocessing.Queue(),
			"data": multiprocessing.Queue()
		}
		assert maxWorker
		self.workerList = [
			multiprocessing.Process(target=Worker._taskWrapper,
			args=(task, self.shared)) for i in range(maxWorker)
		]

	@staticmethod
	def _taskWrapper(task, shared):
		# Loop unless the workers are notified to be stopped by the master process
		while not (shared["data"].empty() and shared["stop"].value):

			# Wait until there is data available
			data = None
			try:
				data = shared["data"].get(block=True, timeout=.1)
			except queue.Empty:
				continue
			assert data is not None

			# Run the stask
			isSuccess = True
			stdout = StringIO()
			result = None
			try:
				result = task(data, stdout)
			except:
				isSuccess = False
				exceptionType, exceptionValue = sys.exc_info()[:2]
				stdout.write(
					"Failed with exception of type '{}' and message: {}\n".
					format(exceptionType.__name__, exceptionValue))

			shared["output"].put((isSuccess, result, stdout.getvalue()))

	def add(self, data) -> None:
		self.shared["data"].put(data)  # type: ignore

	def start(self) -> None:
		for worker in self.workerList:
			worker.start()

	def isAlive(self):
		for worker in self.workerList:
			if worker.is_alive():
				return True
		return False

	def stop(self, handler=None):
		self.shared["stop"].value = 1
		result = []
		while (not self.shared["output"].empty()) or (self.isAlive()):
			try:
				workerResult = _WorkerResult(
					self.shared["output"].get_nowait())
				result.append(workerResult)
				if handler:
					handler(workerResult)
			except queue.Empty:
				continue

		for worker in self.workerList:
			worker.join()

		return result
