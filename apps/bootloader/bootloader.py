import pathlib
import typing
import argparse
import os
import sys
import time

from apps.bootloader.binary import Binary, StablePolicy, ExceptionBinaryAbort
from apps.bootloader.config import STABLE_VERSION, application, updatePath, updatePolicy
from apps.bootloader.utils import RollingNamedTemporaryFile
from apps.artifacts.api.python.release.release import Release
from apps.artifacts.api.python.release.mock import ReleaseMock
from apps.artifacts.api.python.node.node import Node
from bzd.utils.scheduler import Scheduler
from bzd.utils.logging import Logger, LoggerBackendInMemory, LoggerBackendStub


class Context:

	def __init__(self, args: typing.List[str], logger: Logger) -> None:
		self.args = args
		self.values, self.rest = Context.parse(args)
		self.running = True
		self.binary: typing.Optional[Binary] = None
		self.newBinary: typing.Optional[pathlib.Path] = None
		self.updateIgnoreOverride: typing.Optional[str] = None
		self.logger = logger
		self.release = Release(uid=self.uid)

	@staticmethod
	def parse(args: typing.List[str]) -> typing.Tuple[argparse.Namespace, typing.List[str]]:
		parser = argparse.ArgumentParser(description="Bootloader.", add_help=False)
		parser.add_argument("--bootloader-help", action="help")
		parser.add_argument("--bootloader-update-interval", type=float, help="Application update interval in seconds.")
		parser.add_argument("--bootloader-update-policy",
		                    type=StablePolicy,
		                    choices=[e.value for e in StablePolicy],
		                    help="Update policy to decide if the application is valid.")
		parser.add_argument("--bootloader-update-path", type=pathlib.Path, help="Path to get the update form remote.")
		parser.add_argument("--bootloader-update-ignore",
		                    type=str,
		                    default=STABLE_VERSION,
		                    help="The string to ignore from the update.")
		parser.add_argument("--bootloader-self-path",
		                    type=pathlib.Path,
		                    default=os.environ.get("BZD_BUNDLE"),
		                    help="Path of the current binary.")
		parser.add_argument(
		    "--bootloader-stable-path",
		    type=pathlib.Path,
		    default=os.environ.get("BZD_BUNDLE"),
		    help="Path of the stable binary, it will be replaced with this one when it is considered stable.")
		parser.add_argument("--bootloader-uid", type=str, help="The unique identifier of this instance.")
		parser.add_argument("--bootloader-application",
		                    type=pathlib.Path,
		                    help="The application to be called by the bootloader.")

		return parser.parse_known_args(args)

	@property
	def application(self) -> pathlib.Path:
		return self.values.bootloader_application or pathlib.Path(application)

	@property
	def updatePolicy(self) -> StablePolicy:
		return StablePolicy(self.values.bootloader_update_policy or updatePolicy)

	@property
	def updateInterval(self) -> float:
		return self.values.bootloader_update_interval or 3600

	@property
	def updatePath(self) -> typing.Optional[pathlib.Path]:
		return typing.cast(typing.Optional[pathlib.Path],
		                   self.values.bootloader_update_path) or pathlib.Path(updatePath)

	@property
	def updateIgnore(self) -> typing.Optional[str]:
		return self.updateIgnoreOverride or typing.cast(typing.Optional[str], self.values.bootloader_update_ignore)

	@property
	def stablePath(self) -> typing.Optional[pathlib.Path]:
		return typing.cast(typing.Optional[pathlib.Path], self.values.bootloader_stable_path)

	@property
	def selfPath(self) -> typing.Optional[pathlib.Path]:
		return typing.cast(typing.Optional[pathlib.Path], self.values.bootloader_self_path)

	@property
	def uid(self) -> typing.Optional[str]:
		return typing.cast(typing.Optional[str], self.values.bootloader_uid)

	def argsForBinary(self, binary: pathlib.Path) -> typing.List[str]:
		"""Recreate the command line."""

		args = ["--bootloader-self-path", str(binary)]
		if self.values.bootloader_update_interval:
			args += ["--bootloader-update-interval", str(self.values.bootloader_update_interval)]
		if self.values.bootloader_update_policy:
			args += ["--bootloader-update-policy", str(self.values.bootloader_update_policy)]
		if self.values.bootloader_update_policy:
			args += ["--bootloader-update-path", str(self.values.bootloader_update_path)]
		if self.values.bootloader_stable_path:
			args += ["--bootloader-stable-path", str(self.values.bootloader_stable_path)]
		if self.values.bootloader_uid:
			args += ["--bootloader-uid", str(self.values.bootloader_uid)]
		if self.values.bootloader_application:
			args += ["--bootloader-application", str(self.values.bootloader_application)]
		args += self.rest
		return args

	def registerBinary(self, binary: Binary) -> None:
		self.binary = binary

	def changeBinary(self, newBinary: pathlib.Path) -> None:
		self.newBinary = newBinary
		self.running = False
		assert self.binary is not None
		self.binary.abort()

	def setStableBinary(self, path: pathlib.Path, stablePath: pathlib.Path) -> None:
		os.replace(path, stablePath)

	def setUpdateIgnore(self, ignore: str) -> None:
		self.updateIgnoreOverride = ignore

	def switchProcessToBinary(self, path: pathlib.Path, args: typing.List[str]) -> None:
		os.execv(path, [str(path), *args])


class ContextForTest(Context):

	def __init__(self, *args: typing.Any, **kwargs: typing.Any) -> None:
		super().__init__(*args, **kwargs)
		self.release = typing.cast(
		    Release,
		    ReleaseMock(
		        data={
		            "noop": [{
		                "name": "first",
		                "binary": b"#!/bin/bash\nexit 0"
		            }],
		            "error": [{
		                "name": "first",
		                "binary": b"#!/bin/bash\nexit 1"
		            }]
		        }))
		self.stableBinary: typing.Optional[pathlib.Path] = None

	def setStableBinary(self, path: pathlib.Path, stablePath: pathlib.Path) -> None:
		self.stableBinary = path

	def switchProcessToBinary(self, path: pathlib.Path, args: typing.List[str]) -> None:
		pass


def autoUpdateApplication(context: Context, path: pathlib.Path, uid: str) -> None:
	"""Check if there is an update available."""

	# Check for updates, this can take up to several minutes.
	maybeUpdate = context.release.fetch(path=str(path), uid=uid, ignore=context.updateIgnore)
	if maybeUpdate:
		# Create a directory with the uid to fetch the last update.
		updatePath = RollingNamedTemporaryFile(namespace=f"bootloader_{uid}", maxFiles=2).get()
		context.logger.info(f"Update found, writing to '{updatePath}'.")
		maybeUpdate.toFile(updatePath)

		binary = Binary(binary=updatePath, logger=Logger("binary").backend(LoggerBackendStub()))
		context.registerBinary(binary=binary)
		try:
			binary.run(args=["."])
		except Exception as e:
			context.logger.error(f"Invalid update failed with error: {e}")
			if maybeUpdate.name:
				context.setUpdateIgnore(maybeUpdate.name)
		else:
			context.logger.info(f"Update validated, switching to '{updatePath}'.")
			context.changeBinary(updatePath)


def updateInfo(node: Node) -> None:
	"""Update long standing information."""

	try:
		node.publish(data={"version": {"bootloader": STABLE_VERSION}})
	except:
		# Ignore any errors, we don't want to crash if something is wrong.
		pass


def bootloader(context: Context) -> int:
	"""Run the bootloader.
	
	Returns:
		The return code.
	"""

	if context.application == pathlib.Path("."):
		context.logger.info("Nothing to run.")
		return 0

	binary = Binary(binary=context.application, logger=context.logger)
	context.registerBinary(binary=binary)
	scheduler = Scheduler()

	context.logger.info(
	    f"Bootloader version {STABLE_VERSION} for application {context.application} with given args: {' '.join(context.args)}"
	)

	# Make sure only one instance of the bootloader is running at a time.
	#singleton = Singleton(args.cache / "singleton.lock")
	#if not singleton.lock():
	#	sys.exit(0)

	if context.uid:
		node = Node(uid=context.uid)
		scheduler.add("info", 3600, updateInfo, args=(node, ), immediate=True)

	if context.uid and context.updatePath:
		scheduler.add("application",
		              context.updateInterval,
		              autoUpdateApplication,
		              args=(
		                  context,
		                  context.updatePath,
		                  context.uid,
		              ),
		              immediate=True)

	if scheduler.nbWorkloads > 0:
		scheduler.start()

	def stableCallback() -> None:
		# Replace the stable file if any with this one.
		if context.stablePath:
			assert context.selfPath, "Path of the current binary is not provided."
			if context.stablePath != context.selfPath:
				context.logger.info("Updating stable binary.")
				context.setStableBinary(context.selfPath, context.stablePath)

	lastFailure = 0.
	while context.running:

		try:
			binary.run(args=context.rest, stablePolicy=context.updatePolicy, stableCallback=stableCallback)
			return 0
		except KeyboardInterrupt:
			context.logger.info("<<< Keyboard interrupt >>>")
			return 130
		except ExceptionBinaryAbort:
			context.logger.info("Application aborted.")
			continue
		except Exception as e:
			context.logger.error(str(e))
			if context.stablePath and context.stablePath != context.selfPath:
				context.logger.info("Rolling back to a stable binary.")
				context.changeBinary(context.stablePath)
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

	args = context.argsForBinary(context.newBinary)
	context.logger.info(f"Switch process to {context.newBinary} {' '.join(args)}")
	context.switchProcessToBinary(context.newBinary, args)

	return 1


def runSelfTests(logger: Logger) -> None:
	"""Running all the self tests."""

	noopBinary = str(pathlib.Path(__file__).parent / "tests/noop")
	errorBinary = str(pathlib.Path(__file__).parent / "tests/error")
	sleepBinary = str(pathlib.Path(__file__).parent / "tests/sleep")

	# Test happy path.
	context = ContextForTest(["--bootloader-application", noopBinary, "hello"], logger)
	assert bootloader(context) == 0, "Happy path test failed."

	# Test rollback.
	context = ContextForTest(
	    ["--bootloader-self-path", "a", "--bootloader-stable-path", "b", "--bootloader-application", errorBinary],
	    logger)
	assert bootloader(context) == 1, "Error path test failed."
	assert context.newBinary is not None
	assert context.newBinary.name == "b"

	# Test stable binary.
	context = ContextForTest([
	    "--bootloader-update-policy", "returnCodeZero", "--bootloader-self-path", "a", "--bootloader-stable-path", "b",
	    "--bootloader-application", noopBinary, "hello"
	], logger)
	assert bootloader(context) == 0, "Error path test failed."
	assert context.stableBinary is not None
	assert context.stableBinary.name == "a"

	# Test update.
	context = ContextForTest(["--bootloader-application", sleepBinary, "3600"], logger)
	autoUpdateApplication(context, pathlib.Path("noop"), "test_uid")
	assert bootloader(context) == 1, "Update test failed."
	assert context.newBinary is not None

	# Test failed update.
	context = ContextForTest(["--bootloader-application", noopBinary, "hello"], logger)
	sleepBinary = str(pathlib.Path(__file__).parent / "tests/sleep")
	autoUpdateApplication(context, pathlib.Path("error"), "test_uid")
	assert bootloader(context) == 0, "Failed update test failed."
	assert context.updateIgnore == "first"


if __name__ == "__main__":

	logger = Logger("bootloader")

	logger.info("Self testing...")
	inMemoryBackend = LoggerBackendInMemory("self-tests")
	try:
		runSelfTests(Logger("self-tests").backend(inMemoryBackend))
	except:
		logger.error("\n".join(inMemoryBackend.data))
		raise

	returnCode = bootloader(Context(sys.argv[1:], logger))
	sys.exit(returnCode)
