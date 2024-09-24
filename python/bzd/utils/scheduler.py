import typing
import threading
import dataclasses
import time

from bzd.utils.logging import Logger

logger = Logger("scheduler")


@dataclasses.dataclass
class Workload:
	"""Structure for a workload object."""

	name: str
	intervalS: float
	callback: typing.Callable[..., None]
	args: typing.List[typing.Any]
	kwargs: typing.Dict[str, typing.Any]
	# The workload should start immediately.
	immediate: bool
	# Timestamp for the next call.
	timestampS: float = 0


class Scheduler:

	def __init__(self) -> None:
		self.workloads: typing.List[Workload] = []
		self.queue: typing.List[Workload] = []
		self.worker = threading.Thread(target=self._process, daemon=True)
		self.triggerStop = threading.Event()

	def start(self) -> None:
		"""Start the scheduler."""

		assert self.nbWorkloads > 0, "There are no workloads registered."
		assert not self.worker.is_alive(), "The scheduler is already running."

		self.worker.start()

	def stop(self) -> None:
		"""Stop the scheduler."""

		assert self.worker.is_alive(), "The scheduler is not running."

		self.triggerStop.set()
		self.worker.join()

	def add(self,
	        name: str,
	        intervalS: float,
	        callback: typing.Callable[..., None],
	        immediate: bool = False,
	        args: typing.Optional[typing.List[typing.Any]] = None,
	        kwargs: typing.Optional[typing.Dict[str, typing.Any]] = None) -> None:
		"""Add a new workload to be scheduled."""

		assert not self.worker.is_alive(), "Workload cannot be added while the scheduler is running."
		workload = Workload(name=name,
		                    intervalS=intervalS,
		                    callback=callback,
		                    immediate=immediate,
		                    args=args or [],
		                    kwargs=kwargs or {})
		self.workloads.append(workload)

	@property
	def nbWorkloads(self) -> int:
		return len(self.workloads)

	def _enqueue(self, workload: Workload, timestampS: float) -> None:
		"""Push back an element to the queue."""

		self.queue.append(workload)
		workload.timestampS = timestampS + workload.intervalS
		self.queue.sort(key=lambda x: x.timestampS, reverse=True)

	def _process(self) -> None:

		# Enqueue all the workloads
		self.queue = []
		for workload in self.workloads:
			self._enqueue(workload, 0 if workload.immediate else time.time())

		while True:
			if len(self.queue) == 0:
				break
			workload = self.queue.pop()
			timeoutS = max(0, workload.timestampS - time.time())
			if self.triggerStop.wait(timeout=timeoutS):
				break

			try:
				workload.callback(*workload.args, **workload.kwargs)
			except Exception as e:
				logger.error(f"Workload '{workload.name}' failed with error: {str(e)}")
			finally:
				self._enqueue(workload, time.time())

		self.triggerStop.clear()
