import os
import subprocess
import sys
import threading
from typing import Any, List, Dict, Optional, Tuple, TextIO, Union
from pathlib import Path
import selectors


class ExecuteResultStreamWriter:

	def __init__(
	    self,
	    stdout: Union[bool, TextIO] = False,
	    stderr: Union[bool, TextIO] = False,
	    maxSize: int = 1000000,
	) -> None:
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
			if self.stdout is not False:
				output = sys.stdout if isinstance(self.stdout, bool) else self.stdout
				output.write(data.decode(errors="ignore"))
				output.flush()

	def addStderr(self, data: bytes) -> None:
		if data != b"":
			self._addBuffer(False, data)
			if self.stderr is not False:
				output = sys.stderr if isinstance(self.stderr, bool) else self.stderr
				output.write(data.decode(errors="ignore"))
				output.flush()


class ExecuteResult:

	def __init__(
	    self,
	    stream: ExecuteResultStreamWriter,
	    returncode: int = 1,
	) -> None:
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

	def isSuccess(self) -> bool:
		return self.returncode == 0

	def isFailure(self) -> bool:
		return self.returncode != 0

	def __repr__(self) -> str:
		return f"==== return code: {self.getReturnCode()}, output:\n" + self.getOutput()


class _NoopTimer:

	def is_alive(self) -> bool:
		return True

	def start(self) -> None:
		pass

	def cancel(self) -> None:
		pass


def localCommand(
    cmds: List[str],
    ignoreFailure: bool = False,
    cwd: Optional[Path] = None,
    env: Optional[Dict[str, str]] = None,
    timeoutS: float = 60.0,
    stdin: bool = False,
    stdout: Union[bool, TextIO] = False,
    stderr: Union[bool, TextIO] = False,
    maxOutputSize: int = 1000000,
) -> ExecuteResult:
	"""Run a process locally.

    Args:
            cmds: The list of commands to be executed.
            ignoreFailure: If set to True, upon failure (return code != 0), it will throw.
            cwd: The current working directory.
            env: The set of environment variable to be injected to the process.
            timeoutS: The timeout in seconds until when the command terminates.
                      A value of 0, give an unlimited timeout.
            stdin: If set to True, the input stream will also be streamed to stdin.
            stdout: If set to True, the output will also be streamed to stdout.
            stderr: If set to True, the errors will also be streamed to stderr.
            maxOutputSize: The maximum size of the output, if larger, only the most recent output will be kept.
    """

	print(f"Executing {' '.join(cmds)}", flush=True)

	sel = selectors.DefaultSelector()
	stream = ExecuteResultStreamWriter(stdout, stderr, maxOutputSize)
	proc = subprocess.Popen(
	    cmds,
	    cwd=cwd,
	    stdout=subprocess.PIPE,
	    stdin=None if stdin else subprocess.PIPE,
	    stderr=subprocess.PIPE,
	    env=env,
	)
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
				print(data.decode(errors="ignore"), end="", flush=True)
				(stream.addStdout if key.fileobj is proc.stdout else stream.addStderr)(data)
		remainingStdout, remainingStderr = proc.communicate()
		stream.addStdout(remainingStdout)
		stream.addStderr(remainingStderr)

		if not timer.is_alive():
			stream.addStderr(
			    f"Execution of '{' '.join(cmds)}' timed out after {timeoutS}s, terminating process.\n".encode())
		else:
			returnCode = proc.returncode
			if returnCode == None:
				stream.addStderr(b"Process did not complete.\n")

	finally:
		timer.cancel()

	result = ExecuteResult(stream=stream, returncode=returnCode)

	assert (ignoreFailure or returnCode == 0), f"Return code {result.getReturnCode()}\n{result.getOutput()}"

	return result


def localPython(script: str, args: Optional[List[str]] = None, **kwargs: Any) -> ExecuteResult:
	"""Execute a python script locally."""

	return localCommand([sys.executable, script] + (args or []), **kwargs)


def localBash(script: bytes, args: Optional[List[str]] = None, **kwargs: Any) -> ExecuteResult:
	"""Execute a bash script locally."""

	return localCommand(["bash", "-s", "--"] + (args or []), **kwargs)


def localBazelBinary(path: str,
                     args: Optional[List[str]] = None,
                     env: Optional[Dict[str, str]] = None,
                     **kwargs: Any) -> ExecuteResult:
	"""Execute a bazel binary locally. The caller must run bazel and all runfiles must be already available."""

	env = env or {}
	env["RUNFILES_DIR"] = (os.environ["RUNFILES_DIR"] if "RUNFILES_DIR" in os.environ else os.path.dirname(os.getcwd()))
	return localCommand([path] + (args or []), env=env, **kwargs)


def localBazelTarget(target: str,
                     args: Optional[List[str]] = None,
                     env: Optional[Dict[str, str]] = None,
                     **kwargs: Any) -> ExecuteResult:
	"""Execute a bazel target locally.

    Note, the environment variable is passed to ensure the current are not propagated to the environment.
    """

	defaultEnv = {
	    # PATH env variable is by default passed, this is needed to find tools with rctx.which for example.
	    "PATH": os.environ.get("PATH", "")
	}
	defaultEnv.update(env or {})

	return localCommand(
	    [
	        os.environ.get("BAZEL_REAL", "bazel"),
	        "run",
	        "--ui_event_filters=-info,-stdout,-stderr",
	        "--noshow_progress",
	        target,
	        "--",
	    ] + (args or []),
	    env=defaultEnv,
	    **kwargs,
	)


def localDocker(args: Optional[List[str]] = None, **kwargs: Any) -> ExecuteResult:
	"""Execute a command via docker."""

	return localCommand(["docker"] + (args or []), **kwargs)
