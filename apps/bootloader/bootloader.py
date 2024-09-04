import pathlib
import typing
import argparse
import os
import sys
import time
import threading
import tempfile
import stat
import itertools

from apps.bootloader.binary import Binary, StablePolicy, ExceptionBinaryAbort
from apps.bootloader.singleton import Singleton
from apps.bootloader.config import STABLE_VERSION, application, updatePath, updatePolicy
from apps.artifacts.api.python.release.release import Release
from apps.artifacts.api.python.release.mock import ReleaseMock
from bzd.utils.scheduler import Scheduler
from bzd.utils.logging import Logger


class BootloaderConfig:

	def __init__(self, args: typing.List[str]) -> None:
		self.args = args
		self.argsBootloader = list(itertools.takewhile(lambda x: x != "--", self.args))
		self.argsRest = self.args[len(self.argsBootloader) + 1:]
		self.values = BootloaderConfig.parse(self.argsBootloader)

	@staticmethod
	def parse(args: typing.List[str]) -> None:
		parser = argparse.ArgumentParser(description="Bootloader.")
		parser.add_argument("--update-interval", type=float, help="Application update interval in seconds.")
		parser.add_argument("--update-policy",
		                    type=StablePolicy,
		                    choices=[e.value for e in StablePolicy],
		                    help="Update policy to decide if the application is valid.")
		parser.add_argument("--update-path", type=pathlib.Path, help="Path to get the update form remote.")
		parser.add_argument("--self-path",
		                    type=pathlib.Path,
		                    default=os.environ.get("BZD_BUNDLE"),
		                    help="Path of the current binary.")
		parser.add_argument(
		    "--stable-path",
		    type=pathlib.Path,
		    default=os.environ.get("BZD_BUNDLE"),
		    help="Path of the stable binary, it will be replaced with this one when it is considered stable.")
		parser.add_argument("--uid", type=str, help="The unique identifier of this instance.")
		parser.add_argument("application", type=pathlib.Path, nargs="?", help="The unique identifier of this instance.")

		return parser.parse_args(args)

	def __getattr__(self, key: str) -> typing.Any:
		"""Access the values."""

		if key not in self.values:
			raise KeyError(key)
		return getattr(self.values, key)

	@property
	def application(self) -> pathlib.Path:
		return self.values.application or pathlib.Path(application)

	@property
	def update_policy(self) -> str:
		return self.values.update_policy or updatePolicy

	@property
	def update_interval(self) -> float:
		return self.values.update_interval or 3600

	@property
	def update_path(self) -> pathlib.Path:
		return self.values.update_path or updatePath

	@property
	def can_rollback(self) -> bool:
		"""If the current binary can rollback to a stable binary."""
		return self.stable_path and self.stable_path != self.self_path

	def argsForBinary(self, binary: pathlib.Path) -> None:
		"""Recreate the command line."""

		args = ["--self-path", str(binary)]
		if self.values.update_interval:
			args += ["--update-interval", str(self.values.update_interval)]
		if self.values.update_policy:
			args += ["--update-policy", str(self.values.update_policy)]
		if self.values.update_policy:
			args += ["--update-path", str(self.values.update_path)]
		if self.values.stable_path:
			args += ["--stable-path", str(self.values.stable_path)]
		if self.values.uid:
			args += ["--uid", str(self.values.uid)]
		if self.values.application:
			args += [str(self.values.application)]
		if self.argsRest:
			args += ["--"] + self.argsRest
		return args


class Context:

	def __init__(self) -> None:
		self.running = True
		self.binary: typing.Optional[Binary] = None
		self.newBinary: typing.Optional[pathlib.Path] = None
		self.logger = Logger("bootloader")
		self.release = Release()

	def registerBinary(self, binary: Binary) -> None:
		self.binary = binary

	def changeBinary(self, newBinary: pathlib.Path) -> None:
		self.newBinary = newBinary
		self.running = False
		self.binary.abort()

	def setStableBinary(self, path: pathlib.Path, stablePath: pathlib.Path) -> None:
		os.replace(path, stablePath)

	def switchProcessToBinary(self, path: pathlib.Path, args: typing.List[str]) -> None:
		os.execv(path, args)


class ContextForTest(Context):

	def __init__(self) -> None:
		super().__init__()
		self.release = ReleaseMock(data={"a": [{"name": "first", "binary": b""}]})
		self.stableBinary: typing.Optional[pathlib.Path] = None

	def setStableBinary(self, path: pathlib.Path, stablePath: pathlib.Path) -> None:
		self.stableBinary = path

	def switchProcessToBinary(self, path: pathlib.Path, args: typing.List[str]) -> None:
		pass


def autoUpdateApplication(context: Context, path: pathlib.Path, uid: str) -> None:
	"""Check if there is an update available."""

	# Check for updates, this can take up to several minutes.
	maybeUpdate = context.release.fetch(path=str(path), uid=uid, ignore=STABLE_VERSION)
	if maybeUpdate:
		updateFile = tempfile.NamedTemporaryFile(delete=False)
		updatePath = pathlib.Path(updateFile.name)
		maybeUpdate.toFile(updatePath)
		context.logger.error(f"Update written to '{updatePath}'.")

		# Set execution permission.
		Binary.setPermissions(updatePath)

		context.changeBinary(updatePath)


def bootloader(context: Context, args: typing.List[str]) -> int:
	"""Run the bootloader.
	
	Returns:
		The return code.
	"""

	config = BootloaderConfig(args)
	binary = Binary(binary=config.application)
	context.registerBinary(binary=binary)

	context.logger.info(f"Bootloader version {STABLE_VERSION} for {config.application}")

	# Make sure only one instance of the bootloader is running at a time.
	#singleton = Singleton(args.cache / "singleton.lock")
	#if not singleton.lock():
	#	sys.exit(0)

	if config.update_path:
		scheduler = Scheduler()
		scheduler.add("application",
		              config.update_interval,
		              autoUpdateApplication,
		              args=[context, config.update_path, config.uid],
		              immediate=True)
		scheduler.start()

	def stableCallback() -> None:
		# Replace the stable file if any with this one.
		if config.stable_path:
			assert config.self_path, "Path of the current binary is not provided."
			if config.stable_path != config.self_path:
				context.logger.info("Updating stable binary.")
				context.setStableBinary(config.self_path, config.stable_path)

	lastFailure = 0
	while context.running:

		try:
			binary.run(args=config.argsRest, stablePolicy=config.update_policy, stableCallback=stableCallback)
			return 0
		except KeyboardInterrupt:
			context.logger.info("<<< Keyboard interrupt >>>")
			return 130
		except ExceptionBinaryAbort:
			context.logger.info("Application aborted.")
			continue
		except Exception as e:
			context.logger.error(e)
			if config.can_rollback:
				context.logger.info("Rolling back to a stable binary.")
				context.changeBinary(config.stable_path)
				continue

		currentTime = time.time()
		timeDiffSinceLastFailure = currentTime - lastFailure
		if timeDiffSinceLastFailure > 3600 * 2:
			context.logger.error("Retrying immediately.")
		else:
			context.logger.error("Retrying in 1h.")
			time.sleep(3600)
		lastFailure = currentTime

	assert context.newBinary, "Only new binary request must exit the while loop."

	args = config.argsForBinary(context.newBinary)
	context.logger.info(f"Switch process to {context.newBinary} {' '.join(args)}")
	context.switchProcessToBinary(context.newBinary, args)

	return 1


if __name__ == "__main__":

	# Test happy path.
	context = ContextForTest()
	echoBinary = str(pathlib.Path(__file__).parent / "tests/echo")
	assert bootloader(context, args=[echoBinary, "--", "hello"]) == 0, "Happy path test failed."

	# Test rollback.
	context = ContextForTest()
	errorBinary = str(pathlib.Path(__file__).parent / "tests/error")
	assert bootloader(context, args=["--self-path", "a", "--stable-path", "b",
	                                 errorBinary]) == 1, "Error path test failed."
	assert context.newBinary is not None
	assert context.newBinary.name == "b"

	# Test stable binary.
	context = ContextForTest()
	assert bootloader(
	    context,
	    args=["--update-policy", "returnCodeZero", "--self-path", "a", "--stable-path", "b", echoBinary, "--",
	          "hello"]) == 0, "Error path test failed."
	assert context.stableBinary is not None
	assert context.stableBinary.name == "a"

	# Test update.
	context = ContextForTest()
	sleepBinary = str(pathlib.Path(__file__).parent / "tests/sleep")
	scheduler = Scheduler()
	scheduler.add("application", 1, autoUpdateApplication, args=[context, pathlib.Path("a"), "uid"])
	scheduler.start()
	assert bootloader(context, args=[sleepBinary, "--", "3600"]) == 1, "Update test failed."
	assert context.newBinary is not None
	scheduler.stop()

	returnCode = bootloader(Context(), sys.argv[1:])
	sys.exit(returnCode)
