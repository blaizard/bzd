import unittest
import bzd.utils.worker
import time


class TestWorker(unittest.TestCase):

	@staticmethod
	def foo(x, stdout):
		stdout.write("Hello")
		return x * x

	@staticmethod
	def throwWorkload(x, stdout):
		raise Exception("dummy")

	@staticmethod
	def blockingWorkload(x, stdout):
		time.sleep(10000)

	def Empty(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		with worker.start() as w:
			result = list(w.data())
		self.assertEqual(len(result), 0)

	def testSingleWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		with worker.start() as w:
			w.add(12)
			result = list(w.data())
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isException(), False)
		self.assertEqual(result[0].getResult(), 12 * 12)
		self.assertEqual(result[0].getOutput(), "Hello")

	def testMultiWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		with worker.start() as w:
			for i in range(100):
				w.add(i)
			result = list(w.data())
		self.assertEqual(len(result), 100)

	def testThrowingWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.throwWorkload)
		with worker.start(throwOnException=False) as w:
			w.add(42)
			result = list(w.data())
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isException(), True)

	def testTimeoutWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.blockingWorkload)
		with worker.start(throwOnException=False) as w:
			w.add(42, timeoutS=1)
			result = list(w.data())
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isException(), True)


if __name__ == "__main__":
	unittest.main()
