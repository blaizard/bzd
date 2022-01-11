import os
import subprocess
import sys
import threading
from typing import Any, List, Dict, Optional, Tuple
from pathlib import Path
import selectors


class _ExecuteResultStreamWriter:

	def __init__(self, stdout: bool = False, stderr: bool = False, maxSize: int = 1000000) -> None:
		self.output: List[Tuple[bool, bytes]] = []
		self.stdout = stdout
		self.stderr = stderr
		self.size = 0
		self.maxSize = maxSize

	def _addBuffer(self, stdout: bool, data: bytes) -> None:
		self.output.append((stdout, data))
		self.size += len(data)
		while self.size > self.maxSize:
			self.size -= len(self.output[0][1])
			self.output.pop(0)

	def addStdout(self, data: bytes) -> None:
		if data != b"":
			self._addBuffer(True, data)
			if self.stdout:
				sys.stdout.write(data.decode(errors="ignore"))
				sys.stdout.flush()

	def addStderr(self, data: bytes) -> None:
		if data != b"":
			self._addBuffer(False, data)
			if self.stderr:
				sys.stderr.write(data.decode(errors="ignore"))
				sys.stderr.flush()


class _ExecuteResult:

	def __init__(self, stream: _ExecuteResultStreamWriter = _ExecuteResultStreamWriter(), returncode: int = 1) -> None:
		self.output = stream.output
		self.returncode = returncode

	def getStdout(self) -> str:
		return b"".join([entry[1] for entry in self.output if entry[0] == True]).decode(errors="ignore")

	def getStderr(self) -> str:
		return b"".join([entry[1] for entry in self.output if entry[0] == False]).decode(errors="ignore")

	def getOutput(self) -> str:
		return b"".join([entry[1] for entry in self.output]).decode(errors="ignore")

	def getReturnCode(self) -> int:
		return self.returncode


class _NoopTimer:

	def is_alive(self) -> bool:
		return True

	def start(self) -> None:
		pass

	def cancel(self) -> None:
		pass


def localCommand(cmds: List[str],
	ignoreFailure: bool = False,
	cwd: Optional[Path] = None,
	env: Optional[Dict[str, str]] = None,
	timeoutS: float = 60.,
	stdin: bool = False,
	stdout: bool = False,
	stderr: bool = False,
	maxOutputSize: int = 1000000) -> _ExecuteResult:
	"""Run a process locally.
	
	Args:
		cmds: The list of commands to be executed.
		ignoreFailure: If set to True, uppon failure (return code != 0), it will throw.
		cwd: The current working directory.
		env: The set of environment variable to be injected to the process.
		timeoutS: The timeout in seconds until when the command terminates.
		          A value of 0, give an unlimited timeout.
		stdin: If set to True, the input stream will also be streamed to stdin.
		stdout: If set to True, the output will also be streamed to stdout.
		stderr: If set to True, the errors will also be streamed to stderr.
		maxOutputSize: The maximum size of the output, if larger, only the most recent output will be kept.
	"""

	sel = selectors.DefaultSelector()
	stream = _ExecuteResultStreamWriter(stdout, stderr, maxOutputSize)
	proc = subprocess.Popen(cmds,
		cwd=cwd,
		stdout=subprocess.PIPE,
		stdin=None if stdin else subprocess.PIPE,
		stderr=subprocess.PIPE,
		env=env)
	timer: threading.Timer = threading.Timer(timeoutS, proc.kill) if timeoutS > 0.0 else _NoopTimer()  # type: ignore
	sel.register(proc.stdout, events=selectors.EVENT_READ)  # type: ignore
	sel.register(proc.stderr, events=selectors.EVENT_READ)  # type: ignore

	returnCode = 1
	try:
		isRunning = True
		timer.start()
		while isRunning and timer.is_alive():
			for key, _ in sel.select():
				data = key.fileobj.read1(128)  # type: ignore
				if not data:
					isRunning = False
				(stream.addStdout if key.fileobj is proc.stdout else stream.addStderr)(data)
		remainingStdout, remainingStderr = proc.communicate()
		stream.addStdout(remainingStdout)
		stream.addStderr(remainingStderr)

		if not timer.is_alive():
			stream.addStderr("Execution of '{}' timed out after {}s, terminating process.\n".format(
				" ".join(cmds), timeoutS).encode())
		else:
			returnCode = proc.returncode
			if returnCode == None:
				stream.addStderr(b"Process did not complete.\n")

	finally:
		timer.cancel()

	result = _ExecuteResult(stream=stream, returncode=returnCode)

	assert ignoreFailure or returnCode == 0, "Return code {}\n{}".format(result.getReturnCode(), result.getOutput())

	return result


def localPython(script: str, args: List[str] = [], **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a python script locally.
	"""
	return localCommand([sys.executable, script] + args, **kwargs)


def localBash(script: bytes, args: List[str] = [], **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a bash script locally.
	"""
	return localCommand(["bash", "-s", "--"] + args, **kwargs)


def localBazelBinary(path: str, args: List[str] = [], env: Dict[str, str] = {}, **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a bazel binary locally. The caller must run bazel and all runfiles must be already available.
	"""
	env["RUNFILES_DIR"] = os.environ["RUNFILES_DIR"] if "RUNFILES_DIR" in os.environ else os.path.dirname(os.getcwd())
	return localCommand([path] + args, env=env, **kwargs)


def localDocker(args: List[str] = [], **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a docker-compose run service.
	"""
	return localCommand(["docker"] + args, **kwargs)
