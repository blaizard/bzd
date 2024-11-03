import argparse
import typing
import time
import subprocess
import os
import sys

from apps.artifacts.api.python.node.node import Node, NodePublishNoRemote
from bzd.logging import Logger


class Utility:

	def __init__(self, uid: str, logger: Logger) -> None:
		self.uid = uid
		self.node = Node(uid=self.uid, logger=logger)
		self.logger = logger

	def fail(self, message: str, errorCode: int = 1) -> None:
		self.logger.error(message)
		sys.exit(errorCode)

	def assertTrue(self, condition: bool, message: str, errorCode: int = 1) -> None:
		if not condition:
			self.fail(message, errorCode)

	def cleanPathToList(self, data: str) -> typing.List[str]:
		"""CLeanup the given path."""

		segments: typing.List[str] = []
		for segment in data.split("/"):
			segment = segment.strip()
			if not segment or segment == ".":
				continue
			if segment == "..":
				self.assertTrue(len(segments) > 0, f"The path '{data}' is going above the root.")
				segments.pop()
			else:
				segments.append(segment)
		return segments

	def _publish(self, path: typing.List[str], data: typing.Any) -> None:
		"""Publish to a remote and ignore failure if any."""

		try:
			self.node.publish(data=data, path=path)
			self.logger.info(f"Published to node {self.uid}:{'/'.join(path)}: {data}")

		except NodePublishNoRemote as e:
			self.logger.error(str(e))

	def publish(self, data: str) -> None:
		"""Publish some data to a remote node."""

		pathData = self.cleanPathToList(data)
		self.assertTrue(len(pathData) > 0, f"There is no data to publish, the data given '{data}' evaluates to '.'")
		self._publish(data=pathData[-1], path=pathData[:-1])

	def timeit(self, path: str, args: typing.List[str]) -> None:
		"""Time the given command and publish it."""

		timeStart = time.time()

		result = subprocess.run(args, cwd=os.environ.get("BUILD_WORKSPACE_DIRECTORY", None))
		self.assertTrue(result.returncode == 0, f"Process failed with return code {result.returncode}",
		                result.returncode)

		duration = time.time() - timeStart

		pathAsList = self.cleanPathToList(path)
		self._publish(path=pathAsList, data=duration)
		self.logger.info(f"Time elapsed {duration}s")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Node binary.")
	parser.add_argument("--uid", help="The UID of this node.", required=True)
	subparsers = parser.add_subparsers(dest="command", help="Commands to be executed.")

	parserPublish = subparsers.add_parser("publish", help="Publish a data point to a node remote.")
	parserPublish.add_argument("data", help="The path + data to publish separated by slashes.")

	parserTimeit = subparsers.add_parser("timeit", help="Time the given command and publish the result to the remote.")
	parserTimeit.add_argument("--path", help="Path of the data to access.", default=".")
	parserTimeit.add_argument("rest", nargs=argparse.REMAINDER)

	args = parser.parse_args()

	logger = Logger("node")
	utility = Utility(uid=args.uid, logger=logger)

	if args.command == "publish":
		utility.publish(data=args.data)
	elif args.command == "timeit":
		utility.timeit(args=args.rest, path=args.path)
	else:
		raise NotImplementedError(f"Command '{args.command}' is not implemented.")
