import sys
from datetime import datetime

from bzd.logging.handler import Log, LoggerHandler, LoggerHandlerData, LoggerHandlerFlow


class LoggerHandlerStderr(LoggerHandler):
	"""Print to stderr."""

	def format(self, log: Log) -> str:
		return f"[{datetime.fromtimestamp(log.timestamp)}] [{log.level}] [{log.name}] [{log.filename}:{log.line}] {log.message}"

	def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:

		for log in data:
			print(self.format(log), file=sys.stderr)
		sys.stderr.flush()

		flow.next(data=data)
