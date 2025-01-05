import argparse
import os
import pathlib
import typing
import json
import threading
import time

from bzd.utils.run import localBazel, localCommand

CompileCommandsType = typing.List[typing.Dict[str, str]]


class CompileCommands:

	def __init__(self, compile_commands_path: pathlib.Path) -> None:
		self.compile_commands_path = compile_commands_path
		self.compile_commands: typing.Optional[CompileCommandsType] = None

	def __enter__(self) -> "CompileCommands":
		try:
			self.compile_commands = json.loads(compile_commands_path.read_text())
		except:
			self.compile_commands = []
		return self

	def __exit__(self, *_: typing.Any) -> None:
		output = json.dumps(self.compile_commands, indent=4)
		self.compile_commands_path.write_text(output)

	def insert(self, fragment: typing.Dict[str, str]) -> None:
		file = fragment["file"]
		assert self.compile_commands is not None
		index = next((i for i, x in enumerate(self.compile_commands) if file == x["file"]), -1)
		if index == -1:
			self.compile_commands.append(fragment)
		else:
			self.compile_commands[index] = fragment


class Queue:

	def __init__(self, trigger: typing.Callable[[], None]) -> None:
		self.trigger = trigger
		self.compile_commands_files: typing.Set[pathlib.Path] = set()

	def empty(self) -> bool:
		"""Check if the queue is empty."""

		return len(self.compile_commands_files) == 0

	def notEmpty(self) -> bool:
		"""Check if the queue is not empty."""

		return not self.empty()

	def next(self) -> typing.Optional[pathlib.Path]:
		"""Get the next file to process."""

		if self.empty():
			return None
		return self.compile_commands_files.pop()

	def add(self, file: pathlib.Path) -> None:
		self.compile_commands_files.add(file)
		self.trigger()


class Manager:

	def __init__(self, workspace_path: pathlib.Path, compile_commands_path: pathlib.Path) -> None:
		self.workspace_path = workspace_path
		self.output_base = Manager.getOuptutBasePath(workspace_path)
		self.external_path = self.output_base / "external"
		self.bazel_out_path = self.output_base / "execroot" / "_main" / "bazel-out"
		self.compile_commands_path = compile_commands_path
		self.queue = Queue(trigger=self.trigger)
		self.cv = threading.Condition()
		self.requestStop = False

	@staticmethod
	def getOuptutBasePath(workspace: pathlib.Path) -> pathlib.Path:
		result = localBazel(["info", "output_base"], cwd=workspace)
		return pathlib.Path(result.getStdout().strip())

	def add(self, file: pathlib.Path) -> None:
		self.queue.add(file)

	def updateFragment(self, fragment: typing.Dict[str, str]) -> None:
		"""Update a fragment."""

		fragment["directory"] = str(self.workspace_path)
		command = []
		for segment in fragment["command"].split(" "):
			segment = segment.strip()
			if segment:
				if segment.startswith("external/"):
					segment = segment.replace("external", str(self.external_path), 1)
				elif segment.startswith("bazel-out/"):
					segment = segment.replace("bazel-out", str(self.bazel_out_path), 1)
				command.append(segment)
		fragment["command"] = " ".join(command)

	def start(self) -> None:
		"""Generate the compile_commands.json file."""

		while True:
			with self.cv:
				self.cv.wait_for(lambda: self.queue.notEmpty() or self.requestStop)
			if self.queue.notEmpty():
				with CompileCommands(self.compile_commands_path) as compile_commands:
					while self.queue.notEmpty():
						f = self.queue.next()
						try:
							assert f is not None
							fragments = json.loads((self.workspace_path / f).read_text())
						except:
							continue
						for fragment in fragments:
							self.updateFragment(fragment)
							compile_commands.insert(fragment)
			if self.requestStop:
				break

	def stop(self) -> None:
		"""Stop the manager."""

		self.requestStop = True
		self.trigger()

	def trigger(self) -> None:
		"""Called each time a new data is inserted in the queue."""

		with self.cv:
			self.cv.notify()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Compile commands server.")

	args = parser.parse_args()

	workspace_path = pathlib.Path(os.environ["BUILD_WORKING_DIRECTORY"])
	compile_commands_path = workspace_path / "compile_commands.json"

	result = localCommand(
	    ["find", "-L", "bazel-bin", "-name", "*compile_commands.json", "-type", "f", "-not", "-path", "*.runfiles/*"],
	    cwd=workspace_path)
	files = [pathlib.Path(f.strip()) for f in result.getOutput().split("\n") if f.strip()]

	manager = Manager(workspace_path=workspace_path, compile_commands_path=compile_commands_path)

	for file in files:
		manager.add(file)

	thread = threading.Thread(target=manager.start)
	thread.start()
	manager.stop()
	thread.join()
