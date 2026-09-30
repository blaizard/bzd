import time

# Capture the wall clock at import time so that every subsequent call stays
# monotonic (immune to clock adjustments) while remaining comparable to the
# Unix epoch. This mirrors the high-resolution monotonic clock used on the
# Node.js side.
_timeOriginNs = time.time_ns()
_monotonicOriginNs = time.monotonic_ns()


def _timestampNs() -> int:
	"""Return a monotonic timestamp in nanoseconds since the Unix epoch."""
	return _timeOriginNs + (time.monotonic_ns() - _monotonicOriginNs)


def timestampMs() -> int:
	"""Return the current timestamp in milliseconds.

	Returns:
	        The number of milliseconds elapsed since the Unix epoch.
	"""
	return _timestampNs() // 1000000


def timestampUs() -> int:
	"""Return the current timestamp in microseconds.

	Returns:
	        The number of microseconds elapsed since the Unix epoch.
	"""
	return _timestampNs() // 1000
