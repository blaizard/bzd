import unittest

from bzd.utils.timestamp import timestampMs, timestampUs


class TestTimestamp(unittest.TestCase):
	def testTimestampMs(self) -> None:
		before = timestampMs()
		timestamp = timestampMs()
		after = timestampMs()
		self.assertGreaterEqual(timestamp, before)
		self.assertLessEqual(timestamp, after)

	def testTimestampMonotonic(self) -> None:
		samples = [timestampUs() for _ in range(64)]
		# The clock must never go backwards.
		for previous, current in zip(samples, samples[1:]):
			self.assertGreaterEqual(current, previous)

	def testTimestampUsHasSubMillisecondPrecision(self) -> None:
		# The low digits must vary, otherwise this is just a scaled millisecond value.
		samples = {timestampUs() % 1000 for _ in range(100)}
		self.assertGreater(len(samples), 1)


if __name__ == "__main__":
	unittest.main()
