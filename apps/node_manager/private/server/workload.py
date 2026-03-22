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

		self.terminationGracePeriodS_ = terminationGracePeriodS
		self.leases_: typing.Dict[str, Lease] = {}
		self.uidCounter_ = 0
		self.terminateFn_ = terminateFn
		self.clockFn_ = clockFn
		self.lock_ = threading.Lock()
		self.terminationTimestamp_: typing.Optional[float] = None

	def makeUid_(self) -> str:
		self.uidCounter_ += 1
		return str(self.uidCounter_)

	def register(self, name: str, ttl: int) -> str:
		"""Register a new lease."""

		with self.lock_:
			leaseId = self.makeUid_()
			self.leases_[leaseId] = Lease(
				name=name,
				expiry=self.clockFn_() + ttl,
			)
		return leaseId

	def heartBeat(self, leaseId: str, ttl: int) -> bool:
		"""Refresh an existing lease. Returns False if the lease does not exists."""

		with self.lock_:
			if leaseId not in self.leases_:
				return False
			self.leases_[leaseId].expiry = self.clockFn_() + ttl
		return True

	def release(self, leaseId: str) -> bool:
		"""Remove a lease immediately. Returns False if the lease does not exists."""

		with self.lock_:
			if leaseId not in self.leases_:
				return False
			del self.leases_[leaseId]
		return True

	def hasActiveLease(self) -> bool:
		"""Return True if at least one lease is not yet expired.

		Also garbage collect expired leases.
		"""

		now = self.clockFn_()
		with self.lock_:
			self.leases_ = {leaseId: lease for leaseId, lease in self.leases_.items() if lease.expiry > now}
			return bool(self.leases_)

	def terminationWatcher(self) -> None:
		"""Polling callback that decides when to terminate."""

		# If there is any active lease, dismiss the termination period grace.
		if self.hasActiveLease():
			self.terminationTimestamp_ = None
			return

		# Set the termination period if unset.
		now = self.clockFn_()
		if self.terminationTimestamp_ is None:
			self.terminationTimestamp_ = now + self.terminationGracePeriodS_

		# If the termination period is expired, call terminate.
		if self.terminationTimestamp_ <= now:
			self.terminateFn_()

	def getActiveLeases(self) -> typing.Dict[str, typing.Any]:
		"""Show the active leases."""

		now = self.clockFn_()
		with self.lock_:
			return {leaseId: {"name": lease.name, "ttl": max(lease.expiry - now, 0)} for leaseId, lease in self.leases_.items()}

	def plannedDownTime(self) -> float:
		"""When downtime is expected."""

		now = self.clockFn_()
		if self.terminationTimestamp_ is None:
			with self.lock_:
				maxExpiry = max([now] + [lease.expiry for lease in self.leases_.values()])
			expectedDownTime = maxExpiry + self.terminationGracePeriodS_
		else:
			expectedDownTime = self.terminationTimestamp_
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
