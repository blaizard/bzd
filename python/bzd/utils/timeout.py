import typing
from multiprocessing import Process, Queue
from queue import Empty

ReturnType = typing.TypeVar("ReturnType")
ParameterTypes = typing.ParamSpec("ParameterTypes")


class TimeoutError(Exception):
	pass


class Timeout:
	"""Add a timeout to a function.

	Note that this is the only viable solution I found.
	- Using `signal', will not supported multithreading.
	- Using `threading.Timer`, will not show a proper stack trace.
	- Using `PyThreadState_SetAsyncExc`, is not portable. 
	"""

	def __init__(self, seconds: int) -> None:
		self.seconds = seconds

	def run(self, func: typing.Callable[ParameterTypes, ReturnType], *args: ParameterTypes.args,
	        **kwargs: ParameterTypes.kwargs) -> ReturnType:
		queue: Queue[ReturnType] = Queue()

		def wrapper() -> None:
			"""The target for the new process."""
			result = func(*args, **kwargs)
			queue.put(result)

		process = Process(target=wrapper)
		process.start()

		try:
			return queue.get(timeout=self.seconds)
		except Empty:
			process.terminate()
			process.join()
		allArgs = [repr(arg) for arg in args] + [f"{key}={repr(value)}" for key, value in kwargs.items()]
		raise TimeoutError(f"Function {func.__name__}({', '.join(allArgs)}) timed out after {self.seconds}s")
