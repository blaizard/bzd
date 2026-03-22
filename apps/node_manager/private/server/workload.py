import typing
import time
import dataclasses
import threading

from bzd.http.server import RESTServerContext


@dataclasses.dataclass
class Lease:
	# Name for identification purposes.
	name: str
	# Time in seconds until the lease expires.
	expiry: float


class Workload:
	"""Keep track of workload leases and shutdown the server when all leases have been expired."""

	def __init__(
		self,
		terminationGracePeriodS: int = 300,
		terminateFn: typing.Callable[[], None] = lambda: None,
		clockFn: typing.Callable[[], float] = time.monotonic,
	) -> None:
		"""Initialize the workload class.

		Args:
			terminationGracePeriodS: Period in seconds until which the server will be terminated.
			terminateFn: Termination callback.
			clockFn: Time provider.
		"""

		self.terminationGracePeriodS = terminationGracePeriodS
		self.leases: typing.Dict[str, Lease] = {}
		self.uidCouner = 0
		self.terminateFn = terminateFn
		self.clockFn = clockFn
		self.lock = threading.Lock()
		self.terminationTimestamp: typing.Optional[float] = None

	def makeUid(self) -> str:
		self.uidCouner += 1
		return str(self.uidCouner)

	def register(self, name: str, ttl: int) -> str:
		"""Register a new lease."""

		with self.lock:
			leaseId = self.makeUid()
			self.leases[leaseId] = Lease(
				name=name,
				expiry=self.clockFn() + ttl,
			)
		return leaseId

	def heartBeat(self, leaseId: str, ttl: int) -> bool:
		"""Refresh an existing lease. Returns False if the lease does not exists."""

		with self.lock:
			if leaseId not in self.leases:
				return False
			self.leases[leaseId].expiry = self.clockFn() + ttl
		return True

	def release(self, leaseId: str) -> bool:
		"""Remove a lease immediately. Returns False if the lease does not exists."""

		with self.lock:
			if leaseId not in self.leases:
				return False
			del self.leases[leaseId]
		return True

	def hasActiveLease(self) -> bool:
		"""Return True if at least one lease is not yet expired.

		Also garbage collect expired leases.
		"""

		now = self.clockFn()
		with self.lock:
			self.leases = {leaseId: lease for leaseId, lease in self.leases.items() if lease.expiry > now}
			return bool(self.leases)

	def terminationWatcher(self) -> None:
		"""Polling callback that decides when to terminate."""

		# If there is any active lease, dismiss the termination period grace.
		if self.hasActiveLease():
			self.terminationTimestamp = None
			return

		# Set the termination period if unset.
		now = self.clockFn()
		if self.terminationTimestamp is None:
			self.terminationTimestamp = now + self.terminationGracePeriodS

		# If the termination period is expired, call terminate.
		if self.terminationTimestamp <= now:
			self.terminateFn()

	def getActiveLeases(self) -> typing.Dict[str, typing.Any]:
		"""Show the active leases."""

		now = self.clockFn()
		with self.lock:
			return {leaseId: {"name": lease.name, "ttl": max(lease.expiry - now, 0)} for leaseId, lease in self.leases.items()}

	def plannedDownTime(self) -> float:
		"""When downtime is expected."""

		now = self.clockFn()
		if self.terminationTimestamp is None:
			with self.lock:
				maxExpiry = max([now] + [lease.expiry for lease in self.leases.values()])
			expectedDownTime = maxExpiry + self.terminationGracePeriodS
		else:
			expectedDownTime = self.terminationTimestamp
		return max(expectedDownTime - now, 0)

	def handlerRegister(self, context: RESTServerContext) -> None:
		"""Register a lease."""

		data = context.readJson()
		leaseId = self.register(name=data.get("name", "unknown"), ttl=int(data.get("ttl", 900)))
		context.header("Content-Type", "text/plain")
		context.write(leaseId)

	def handlerHeartBeat(self, context: RESTServerContext) -> None:
		"""Refresh the lease."""

		data = context.readJson()
		leaseId = data["id"]
		ok = self.heartBeat(leaseId=leaseId, ttl=int(data.get("ttl", 900)))
		assert ok, f"The lease '{leaseId}' doesn't exists."

	def handlerRelease(self, context: RESTServerContext) -> None:
		"""Release immediately a lease."""

		data = context.readJson()
		leaseId = data["id"]
		ok = self.release(
			leaseId=leaseId,
		)
		assert ok, f"The lease '{leaseId}' doesn't exists."
