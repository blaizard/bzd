import os
import subprocess
import sys
import threading
from typing import Any, List, Dict, Optional, Tuple
from pathlib import Path
import selectors


class _ExecuteResultStreamWriter:

	def __init__(self, stdout: bool = False, stderr: bool = False) -> None:
		self.output: List[Tuple[bool, bytes]] = []
		self.stdout = stdout
		self.stderr = stderr

	def addStdout(self, data: bytes) -> None:
		if data != b"":
			self.output.append((True, data))
			if self.stdout:
				sys.stdout.write(data.decode(errors="ignore"))
				sys.stdout.flush()

	def addStderr(self, data: bytes) -> None:
		if data != b"":
			self.output.append((False, data))
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


def localCommand(cmds: List[str],
	inputs: bytes = b"",
	ignoreFailure: bool = False,
	cwd: Optional[Path] = None,
	env: Optional[Dict[str, str]] = None,
	timeoutS: float = 60.,
	stdout: bool = False,
	stderr: bool = False) -> _ExecuteResult:
	"""Run a process locally."""

	sel = selectors.DefaultSelector()
	stream = _ExecuteResultStreamWriter(stdout, stderr)
	proc = subprocess.Popen(cmds,
		cwd=cwd,
		stdout=subprocess.PIPE,
		stdin=subprocess.PIPE,
		stderr=subprocess.PIPE,
		env=env)
	timer = threading.Timer(timeoutS, proc.kill)
	sel.register(proc.stdout, events=selectors.EVENT_READ)  # type: ignore
	sel.register(proc.stderr, events=selectors.EVENT_READ)  # type: ignore

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
		if proc.returncode == None:
			stream.addStderr(b"Process did not complete.\n")
	finally:
		timer.cancel()

	result = _ExecuteResult(stream=stream, returncode=proc.returncode)

	assert ignoreFailure or proc.returncode == 0, "Return code {}\n{}".format(result.getReturnCode(),
		result.getOutput())

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
	return localCommand(["bash", "-s", "--"] + args, inputs=script, **kwargs)


def localBazelBinary(path: str, args: List[str] = [], env: Dict[str, str] = {}, **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a bazel binary locally. The caller must run bazel and all runfiles must be already available.
	"""
	env["RUNFILES_DIR"] = os.environ["RUNFILES_DIR"] if "RUNFILES_DIR" in os.environ else os.path.dirname(os.getcwd())
	return localCommand([path] + args, env=env, **kwargs)


def localDockerComposeBinary(config: Path, **kwargs: Any) -> _ExecuteResult:
	"""
	Execute a docker-compose base docker binary.
	"""
	command = ["docker-compose", "--file", config.as_posix()]
	try:
		return localCommand(command + ["up", "--force-recreate"], **kwargs)
	except:
		# This speeds up the shutdown process of the container(s) when CTRL+C is pressed.
		localCommand(command + ["kill"], timeoutS=5, ignoreFailure=True)
	return _ExecuteResult()
