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
		worker.start()
		result = list(worker.data())
		worker.stop()
		self.assertEqual(len(result), 0)

	def testSingleWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		worker.start()
		worker.add(12)
		result = list(worker.data())
		worker.stop()
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isSuccess(), True)
		self.assertEqual(result[0].getResult(), 12 * 12)
		self.assertEqual(result[0].getOutput(), "Hello")

	def testMultiWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		for i in range(100):
			worker.add(i)
		self.assertEqual(worker.context.count.value, 100)
		worker.start()
		result = list(worker.data())
		worker.stop()
		self.assertEqual(len(result), 100)

	def testThrowingWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.throwWorkload)
		worker.add(42)
		worker.start()
		result = list(worker.data())
		worker.stop()
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isSuccess(), False)

	def testTimeoutWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.blockingWorkload)
		worker.add(42, timeoutS=1)
		worker.start()
		result = list(worker.data())
		worker.stop()
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isSuccess(), False)


if __name__ == "__main__":
	unittest.main()
