import argparse
import typing
import time
import subprocess
import os

from apps.artifacts.api.python.node.node import Node


def cleanPathToList(data: str) -> typing.List[str]:
	"""CLeanup the given path."""

	segments: typing.List[str] = []
	for segment in data.split("/"):
		segment = segment.strip()
		if not segment or segment == ".":
			continue
		if segment == "..":
			assert len(segments) > 0, f"The path '{data}' is going above the root."
			segments.pop()
		else:
			segments.append(segment)
	return segments


def publish(node: Node, args: argparse.Namespace) -> None:
	"""Publish some data to a remote node."""

	pathData = cleanPathToList(args.data)
	assert len(pathData) > 0, f"There is no data to publish, the data given '{args.data}' evaluates to '.'"
	node.publish(data=pathData[-1], path=pathData[:-1])
	print(f"Published to node {args.uid}:{'/'.join(pathData[:-1])}: {pathData[-1]}")


def timeit(node: Node, args: argparse.Namespace) -> None:
	"""Time the given command and publish it."""

	timeStart = time.time()
	subprocess.run(args.rest, check=True, cwd=os.environ.get("BUILD_WORKSPACE_DIRECTORY", None))
	duration = time.time() - timeStart

	path = cleanPathToList(args.path)
	node.publish(path=path, data=duration)
	print(f"Time elpased {duration}s published to node {args.uid}:{'/'.join(path)}")


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Node binary.")
	parser.add_argument("--uid", help="The UID of this node.", required=True)
	subparsers = parser.add_subparsers(dest="command", help="Commands to be executed.")

	parserPublish = subparsers.add_parser("publish", help="Publish a data point to a node remote.")
	parserPublish.add_argument("data", help="The path + data to publish separated by slashes.")

	parserTimeit = subparsers.add_parser("timeit", help="Time the given command and publish the result to the remote.")
	parserTimeit.add_argument("--path", help="Path of the data to access.", default=".")
	parserTimeit.add_argument("rest", nargs="*")

	args = parser.parse_args()

	node = Node(uid=args.uid)

	if args.command == "publish":
		publish(node=node, args=args)
	elif args.command == "timeit":
		timeit(node=node, args=args)
	else:
		raise NotImplementedError(f"Command '{args.command}' is not implemented.")
