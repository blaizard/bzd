import unittest
import bzd.utils.worker


class TestWorker(unittest.TestCase):
	@staticmethod
	def foo(x, stdout):
		stdout.write("Hello")
		return x * x

	def testEmpty(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		worker.start()
		result = worker.stop()
		self.assertEqual(len(result), 0)

	def testSingleWorkload(self) -> None:
		worker = bzd.utils.worker.Worker(TestWorker.foo)
		worker.start()
		worker.add(12)
		result = worker.stop()
		self.assertEqual(len(result), 1)
		self.assertEqual(result[0].isSuccess(), True)
		self.assertEqual(result[0].getResult(), 12 * 12)
		self.assertEqual(result[0].getOutput(), "Hello")


if __name__ == '__main__':
	unittest.main()
