import json
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

	def __init__(self, clockFn: typing.Callable[[], float] = time.monotonic) -> None:
		self.leases: typing.Dict[str, Lease] = {}
		self.uidCouner = 0
		self.clockFn = clockFn
		self.lock = threading.Lock()

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
		"""Return True if at least one lease is not yet expired."""

		now = self.clockFn()
		with self.lock:
			return any(lease.expiry > now for lease in self.leases.values())

	def shutdownWatcher(self) -> None:
		"""Start the background polling scheduler."""
		pass

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

	def handlerMonitor(self, context: RESTServerContext) -> None:
		"""Show the active leases."""

		now = self.clockFn()
		with self.lock:
			data = {leaseId: {"name": lease.name, "ttl": max(lease.expiry - now, 0)} for leaseId, lease in self.leases.items()}

		context.header("Content-type", "application/json")
		context.write(json.dumps(data))
