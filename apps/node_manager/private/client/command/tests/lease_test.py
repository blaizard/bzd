import unittest
from unittest.mock import MagicMock, patch
import threading
import typing

from apps.node_manager.private.client.command.lease import commandLease
from bzd.http.client_mock import HttpClientMock


class TestLeaseCommand(unittest.TestCase):
	def test_lease_success(self) -> None:
		calls = []

		def httpClientCallback(method: str, url: str, **kwargs: typing.Any) -> str:
			calls.append((method, url))
			if url.endswith("/workload/register"):
				return "lease-123"
			return ""

		httpClient = HttpClientMock(callback=httpClientCallback)

		mockProcess = MagicMock()
		mockProcess.__enter__.return_value = mockProcess
		mockProcess.wait.return_value = None
		mockProcess.returncode = 0

		with patch("subprocess.Popen", return_value=mockProcess) as mockPopen:
			exitCode = commandLease(
				server="localhost:1234",
				name="test-workload",
				ttl=60,
				command=["echo", "hello"],
				httpClient=httpClient,
			)

			self.assertEqual(exitCode, 0)
			mockPopen.assert_called_once_with(["echo", "hello"])

		# Verify http calls: register and release
		# (The heartbeat might not have run because we patched Event.wait to return True immediately)
		self.assertTrue(any(call == ("POST", "http://localhost:1234/workload/register") for call in calls))
		self.assertTrue(any(call == ("POST", "http://localhost:1234/workload/release") for call in calls))

	def test_lease_heartbeat(self) -> None:
		calls = []
		heartbeatCalled = threading.Event()

		def httpClientCallback(method: str, url: str, **kwargs: typing.Any) -> str:
			calls.append((method, url))
			if url.endswith("/workload/register"):
				return "lease-123"
			if url.endswith("/workload/heartbeat"):
				heartbeatCalled.set()
			return ""

		httpClient = HttpClientMock(callback=httpClientCallback)

		mockProcess = MagicMock()
		mockProcess.__enter__.return_value = mockProcess
		mockProcess.wait.side_effect = lambda: heartbeatCalled.wait(timeout=5)
		mockProcess.returncode = 0

		with patch("subprocess.Popen", return_value=mockProcess):
			exitCode = commandLease(
				server="localhost:1234",
				name="test-workload",
				ttl=1,  # Small ttl to trigger wait
				command=["echo", "hello"],
				httpClient=httpClient,
			)

			self.assertEqual(exitCode, 0)

		self.assertTrue(any(call == ("POST", "http://localhost:1234/workload/register") for call in calls))
		self.assertTrue(any(call == ("POST", "http://localhost:1234/workload/heartbeat") for call in calls))
		self.assertTrue(any(call == ("POST", "http://localhost:1234/workload/release") for call in calls))


if __name__ == "__main__":
	unittest.main()
