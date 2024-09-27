import typing

from bzd.logging.handler import LoggerHandler, LoggerHandlerData, LoggerHandlerFlow, Log


class LoggerHandlerInMemory(LoggerHandler):
	"""Log the content in-memory."""

	def __init__(self, maxBufferSize: int = 10000) -> None:
		self.maxBufferSize = maxBufferSize
		self.size = 0
		self.records: typing.List[Log] = []

	def handler(self, data: LoggerHandlerData, flow: LoggerHandlerFlow) -> None:

		for log in data:
			self.size += len(log.message)
			self.records.append(log)

		# Resize the buffer.
		while self.size > self.maxBufferSize:
			sizeToBeRemoved = self.size - self.maxBufferSize
			sizeNextEntry = len(self.records[0].message)
			sizeRemove = min(sizeNextEntry, sizeToBeRemoved)
			self.size -= sizeRemove
			self.records[0].message = self.records[0].message[sizeRemove:]
			if len(self.records[0].message) == 0:
				self.records.pop(0)

		flow.next(data=data)

	def __iter__(self) -> typing.Iterator[Log]:
		"""Accessor for the in-memory data."""

		for log in self.records:
			yield log
