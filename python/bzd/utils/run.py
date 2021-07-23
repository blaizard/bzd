import os
import subprocess
import sys
import threading
from typing import Any, List, Dict, Optional, Tuple
from pathlib import Path
import selectors


class _ExecuteResultStreamWriter:

	def __init__(self) -> None:
		self.output: List[Tuple[bool, bytes]] = []

	def addStdout(self, data: bytes) -> None:
		if data != b"":
			self.output.append((True, data))

	def addStderr(self, data: bytes) -> None:
		if data != b"":
			self.output.append((False, data))


class _ExecuteResult:

	def __init__(self, stream: _ExecuteResultStreamWriter, returncode: int) -> None:
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


"""
Run a process locally.
"""


def localCommand(cmds: List[str],
	inputs: bytes = b"",
	ignoreFailure: bool = False,
	cwd: Optional[Path] = None,
	env: Optional[Dict[str, str]] = None,
	timeoutS: float = 60.) -> _ExecuteResult:

	sel = selectors.DefaultSelector()
	stream = _ExecuteResultStreamWriter()
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
		stdout, stderr = proc.communicate()
		stream.addStdout(stdout)
		stream.addStderr(stderr)
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


"""
Execute a python script locally.
"""


def localPython(script: str, args: List[str] = [], **kargs: Any) -> _ExecuteResult:
	return localCommand([sys.executable, script] + args, **kargs)


"""
Execute a bash script locally.
"""


def localBash(script: bytes, args: List[str] = [], **kargs: Any) -> _ExecuteResult:
	return localCommand(["bash", "-s", "--"] + args, inputs=script, **kargs)


"""
Execute a bazel binary locally. The caller must run bazel and all runfiles must be already available.
"""


def localBazelBinary(path: str, args: List[str] = [], env: Dict[str, str] = {}, **kargs: Any) -> _ExecuteResult:
	env["RUNFILES_DIR"] = os.environ["RUNFILES_DIR"] if "RUNFILES_DIR" in os.environ else os.path.dirname(os.getcwd())
	return localCommand([path] + args, env=env, **kargs)
