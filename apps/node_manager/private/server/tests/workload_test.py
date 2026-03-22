import unittest
import json

from apps.node_manager.private.server.workload import Workload
from bzd.http.server_mock import makeRESTServerContext


class TestWorkload(unittest.TestCase):
	def setUp(self) -> None:
		self.currentTime = 1000.0
		self.terminated = False
		self.gracePeriod = 300

		def clockFn() -> float:
			return self.currentTime

		def terminateFn() -> None:
			self.terminated = True

		self.workload = Workload(terminationGracePeriodS=self.gracePeriod, clockFn=clockFn, terminateFn=terminateFn)

	def testRegister(self) -> None:
		leaseId = self.workload.register(name="test", ttl=100)
		self.assertEqual(leaseId, "1")
		self.assertTrue(self.workload.hasActiveLease())

		# Advance time just before expiry
		self.currentTime += 99
		self.assertTrue(self.workload.hasActiveLease())

		# Advance time just after expiry
		self.currentTime += 2
		self.assertFalse(self.workload.hasActiveLease())

	def testHeartBeat(self) -> None:
		leaseId = self.workload.register(name="test", ttl=100)
		self.currentTime += 50
		self.assertTrue(self.workload.heartBeat(leaseId, ttl=100))

		# Original expiry was 1100, new should be 1050 + 100 = 1150
		self.currentTime = 1149
		self.assertTrue(self.workload.hasActiveLease())
		self.currentTime = 1151
		self.assertFalse(self.workload.hasActiveLease())

		# Heartbeat for non-existent lease
		self.assertFalse(self.workload.heartBeat("non-existent", ttl=100))

	def testRelease(self) -> None:
		leaseId = self.workload.register(name="test", ttl=100)
		self.assertTrue(self.workload.hasActiveLease())
		self.assertTrue(self.workload.release(leaseId))
		self.assertFalse(self.workload.hasActiveLease())

		# Release for non-existent lease
		self.assertFalse(self.workload.release("non-existent"))

	def testHandlerRegister(self) -> None:
		context = makeRESTServerContext(data=json.dumps({"name": "my-app", "ttl": 50}).encode("utf-8"))
		self.workload.handlerRegister(context)

		# Check response
		self.assertEqual(context.response.data, [b"1"])
		self.assertIn(("Content-Type", "text/plain"), context.response.headers)

		# Check registration
		self.assertTrue(self.workload.hasActiveLease())
		self.currentTime += 51
		self.assertFalse(self.workload.hasActiveLease())

	def testHandlerHeartBeat(self) -> None:
		leaseId = self.workload.register(name="test", ttl=100)
		context = makeRESTServerContext(data=json.dumps({"id": leaseId, "ttl": 200}).encode("utf-8"))
		self.workload.handlerHeartBeat(context)

		# New expiry: 1000 + 200 = 1200
		self.currentTime = 1199
		self.assertTrue(self.workload.hasActiveLease())
		self.currentTime = 1201
		self.assertFalse(self.workload.hasActiveLease())

	def testHandlerRelease(self) -> None:
		leaseId = self.workload.register(name="test", ttl=100)
		context = makeRESTServerContext(data=json.dumps({"id": leaseId}).encode("utf-8"))
		self.workload.handlerRelease(context)

		self.assertFalse(self.workload.hasActiveLease())

	def testGetActiveLeases(self) -> None:
		self.workload.register(name="app1", ttl=100)
		self.currentTime += 20
		self.workload.register(name="app2", ttl=50)

		# Current time is 1020.
		# app1 expiry: 1000 + 100 = 1100. TTL: 80
		# app2 expiry: 1020 + 50 = 1070. TTL: 50

		data = self.workload.getActiveLeases()

		self.assertEqual(data["1"]["name"], "app1")
		self.assertAlmostEqual(data["1"]["ttl"], 80)
		self.assertEqual(data["2"]["name"], "app2")
		self.assertAlmostEqual(data["2"]["ttl"], 50)

	def testTerminationWatcher(self) -> None:
		# Scenario 1: No active leases triggers termination grace period.
		self.workload.terminationWatcher()
		self.assertFalse(self.terminated)

		# Scenario 2: Advancing time within grace period does not trigger termination.
		self.currentTime += self.gracePeriod - 1
		self.workload.terminationWatcher()
		self.assertFalse(self.terminated)

		# Scenario 3: Advancing time beyond grace period triggers termination.
		self.currentTime += 1
		self.workload.terminationWatcher()
		self.assertTrue(self.terminated)

		# Scenario 4: Active lease clears termination.
		self.terminated = False
		self.workload.register(name="test", ttl=100)
		self.workload.terminationWatcher()
		# Termination should not happen even after grace period if lease is active.
		self.currentTime += self.gracePeriod + 1
		self.workload.terminationWatcher()
		self.assertFalse(self.terminated)

		# Scenario 5: Expired lease re-enables termination.
		self.currentTime += 101  # Lease expired.
		self.workload.terminationWatcher()
		self.assertFalse(self.terminated)
		self.currentTime += self.gracePeriod
		self.workload.terminationWatcher()
		self.assertTrue(self.terminated)

	def testPlannedDownTime(self) -> None:
		# Initial state: no leases.
		# plannedDownTime = max(now, maxExpiry) + gracePeriod - now = 0 + 300 = 300
		self.assertEqual(self.workload.plannedDownTime(), 300)

		# Register a lease with TTL 100.
		# plannedDownTime = (1000 + 100) + 300 - 1000 = 400
		self.workload.register(name="test", ttl=100)
		self.assertEqual(self.workload.plannedDownTime(), 400)

		# Advance time by 50.
		# plannedDownTime = 1100 + 300 - 1050 = 350
		self.currentTime += 50
		self.assertEqual(self.workload.plannedDownTime(), 350)

		# Add another lease with longer TTL.
		# app2 expiry: 1050 + 200 = 1250.
		# plannedDownTime = 1250 + 300 - 1050 = 500
		self.workload.register(name="test2", ttl=200)
		self.assertEqual(self.workload.plannedDownTime(), 500)

		# Trigger termination watcher.
		# Since we have active leases, it shouldn't affect the result here.
		self.workload.terminationWatcher()
		self.assertEqual(self.workload.plannedDownTime(), 500)

		# Release all leases.
		self.workload.release("1")
		self.workload.release("2")
		# No leases.
		# plannedDownTime = 1050 + 300 - 1050 = 300
		self.assertEqual(self.workload.plannedDownTime(), 300)

		# Trigger termination watcher.
		# plannedDownTime = 1350 - 1050 = 300
		self.workload.terminationWatcher()
		self.assertEqual(self.workload.plannedDownTime(), 300)

		# Advance time.
		self.currentTime += 100
		# plannedDownTime = 1350 - 1150 = 200
		self.assertEqual(self.workload.plannedDownTime(), 200)

	def testGarbageCollection(self) -> None:
		self.workload.register(name="to-be-expired", ttl=10)
		self.assertEqual(len(self.workload.getActiveLeases()), 1)

		# Advance time beyond TTL
		self.currentTime += 11
		# hasActiveLease should trigger garbage collection
		self.assertFalse(self.workload.hasActiveLease())

		# Verify it's removed from the active leases
		self.assertEqual(len(self.workload.getActiveLeases()), 0)

	def testDefaultTerminationPeriodS(self) -> None:
		def clockFn() -> float:
			return self.currentTime

		def terminateFn() -> None:
			self.terminated = True

		# Initialize with a default termination period of 500s.
		# Current time is 1000.0, so termination should be at 1500.0.
		workload = Workload(
			defaultTerminationPeriodS=500, terminationGracePeriodS=self.gracePeriod, clockFn=clockFn, terminateFn=terminateFn
		)

		self.assertEqual(workload.plannedDownTime(), 500)

		# Advance time to 1499.
		self.currentTime = 1499
		workload.terminationWatcher()
		self.assertFalse(self.terminated)
		self.assertEqual(workload.plannedDownTime(), 1)

		# Advance time to 1501.
		self.currentTime = 1501
		workload.terminationWatcher()
		self.assertTrue(self.terminated)
		self.assertEqual(workload.plannedDownTime(), 0)

	def testDefaultTerminationPeriodSClearedByLease(self) -> None:
		def clockFn() -> float:
			return self.currentTime

		def terminateFn() -> None:
			self.terminated = True

		# Current time is 1000.0.
		workload = Workload(
			defaultTerminationPeriodS=500, terminationGracePeriodS=self.gracePeriod, clockFn=clockFn, terminateFn=terminateFn
		)

		# Register a lease with TTL 100 (expiry at 1100).
		workload.register(name="test", ttl=100)

		# Calling terminationWatcher should clear the default termination period because there's an active lease.
		workload.terminationWatcher()

		# Now plannedDownTime should be based on lease expiry (1100) + grace period (300) = 1400.
		# 1400 - 1000 = 400.
		self.assertEqual(workload.plannedDownTime(), 400)


if __name__ == "__main__":
	unittest.main()
