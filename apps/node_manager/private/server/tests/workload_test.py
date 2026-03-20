import unittest
import json

from apps.node_manager.private.server.workload import Workload
from bzd.http.server_mock import makeRESTServerContext


class TestWorkload(unittest.TestCase):
	def setUp(self) -> None:
		self.currentTime = 1000.0

		def clockFn() -> float:
			return self.currentTime

		self.workload = Workload(clockFn=clockFn)

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

	def testHandlerMonitor(self) -> None:
		self.workload.register(name="app1", ttl=100)
		self.currentTime += 20
		self.workload.register(name="app2", ttl=50)

		# Current time is 1020.
		# app1 expiry: 1000 + 100 = 1100. TTL: 80
		# app2 expiry: 1020 + 50 = 1070. TTL: 50

		context = makeRESTServerContext()
		self.workload.handlerMonitor(context)

		self.assertEqual(len(context.response.data), 1)
		data = json.loads(context.response.data[0].decode("utf-8"))

		self.assertEqual(data["1"]["name"], "app1")
		self.assertAlmostEqual(data["1"]["ttl"], 80)
		self.assertEqual(data["2"]["name"], "app2")
		self.assertAlmostEqual(data["2"]["ttl"], 50)


if __name__ == "__main__":
	unittest.main()
