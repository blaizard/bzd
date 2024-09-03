import pathlib
import typing
import argparse
import os
import sys
import time
import threading
import tempfile
import itertools

from apps.bootloader.binary import Binary, StablePolicy, ExceptionBinaryAbort
from apps.bootloader.singleton import Singleton
from apps.bootloader.config import STABLE_VERSION, application, updatePath, updatePolicy
from apps.artifacts.api.python.release.release import Release
from bzd.utils.scheduler import Scheduler
from bzd.utils.logging import Logger


def selfTests(cache) -> None:
	"""Perform a sequence of self tests."""

	uid = "_binary_self_test"
	app = "test"

	binary = BinaryForTest(
	    uid, app, cache, {
	        app: [
	            {
	                "name": "first",
	                "binary": b"#!/usr/bin/env bash\nexit 0"
	            },
	            {
	                "name": "second",
	                "binary": b"#!/usr/bin/env bash\nexit 0"
	            },
	            {
	                "name": "third",
	                "binary": b"#!/usr/bin/env bash\nexit 1"
	            },
	        ]
	    })
	binary.clean()

	def assertBinary(name: str) -> None:
		binaryName = binary.get().name
		assert binaryName == name, f"Wrong binary expected, received '{binaryName}' instead of '{name}'"

	def assertUpdate(maybeUpdate: typing.Optional[pathlib.Path], name: typing.Optional[str]) -> None:
		assert (maybeUpdate is not None) or (name is None), f"Expected '{name}' but got no update"
		assert (name is not None) or (maybeUpdate is None), f"Got '{maybeUpdate.name}' but expected no update"
		assert (name is None) or (maybeUpdate.name == name), f"Expected '{name}' but got instead '{maybeUpdate.name}'"

	assert binary.get() is None
	binary.update()
	binary.run()
	assertBinary("first")
	assertUpdate(binary.update(), "second")
	binary.run(stablePolicy=StablePolicy.returnCodeZero)
	assertBinary("second")
	assertUpdate(binary.update(), "third")
	hasFailed = False
	try:
		binary.run()
	except:
		hasFailed = True
	assertBinary("third")
	assert hasFailed, f"The last binary must have failed."
	binary.rollback()
	binary.run()
	assertBinary("second")
	assertUpdate(binary.update(), None)
	assertBinary("second")


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
		                    type=str,
		                    choices=list(map(lambda c: c.value, StablePolicy)),
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


class Lifetime:

	def __init__(self, binary: Binary) -> None:
		self.running = True
		self.binary = binary
		self.newBinary: typing.Optional[pathlib.Path] = None

	def changeBinary(self, newBinary: pathlib.Path) -> None:
		self.newBinary = newBinary
		self.running = False
		self.binary.abort()


def autoUpdateApplication(release, config, lifetime) -> None:
	"""Check if there is an update available."""

	# Check for updates, this can take up to several minutes.
	maybeUpdate = release.fetch(path=config.update_path, uid=config.uid, ignore=STABLE_VERSION)
	if maybeUpdate:
		updateFile = tempfile.NamedTemporaryFile(delete=False)
		updatePath = pathlib.Path(updateFile.name)
		maybeUpdate.toFile(updatePath)
		Logger.error(f"Update downloaded to '{updatePath}'.")
		lifetime.changeBinary(updatePath)


def bootloader(args: typing.List[str]) -> int:
	"""Run the bootloader.
	
	Returns:
		The return code.
	"""

	config = BootloaderConfig(args)
	binary = Binary(binary=config.application)
	lifetime = Lifetime(binary=binary)

	Logger.info(f"Bootloader version {STABLE_VERSION} for {config.application}")

	# Make sure only one instance of the bootloader is running at a time.
	#singleton = Singleton(args.cache / "singleton.lock")
	#if not singleton.lock():
	#	sys.exit(0)

	if config.update_path:
		scheduler = Scheduler()
		scheduler.add("application",
		              config.update_interval,
		              autoUpdateApplication,
		              args=[Release(), config, lifetime],
		              immediate=True)
		scheduler.start()

	def stableCallback() -> None:
		# Replace the stable file if any with this one.
		if config.stable_path:
			assert config.self_path, "Path of the current binary is not provided."
			if config.stable_path != config.self_path:
				Logger.info("Updating stable binary.")
				os.replace(config.self_path, config.stable_path)

	lastFailure = 0
	while lifetime.running:

		try:
			binary.run(args=config.argsRest, stablePolicy=StablePolicy.runningPast1h, stableCallback=stableCallback)
			return 0
		except KeyboardInterrupt:
			Logger.info("<<< Keyboard interrupt >>>")
			return 130
		except ExceptionBinaryAbort:
			Logger.info("Application aborted.")
			continue
		except Exception as e:
			Logger.error(e)
			if config.can_rollback:
				Logger.info("Rolling back to a stable binary.")
				lifetime.changeBinary(args.stable_path)
				continue

		currentTime = time.time()
		timeDiffSinceLastFailure = currentTime - lastFailure
		if timeDiffSinceLastFailure > 3600 * 2:
			Logger.error("Retrying immediately.")
		else:
			Logger.error("Retrying in 1h.")
			time.sleep(3600)
		lastFailure = currentTime

	assert lifetime.newBinary, "Only new binary request must exit the while loop."

	args = config.argsForBinary(lifetime.newBinary)
	Logger.info(f"Running {lifetime.newBinary} {' '.join(args)}")
	os.execv(lifetime.newBinary, args)

	assert False, f"Nothing should runs past this."
	return 1


if __name__ == "__main__":

	bootloader(args=["--", "--help"])

	returnCode = bootloader(sys.argv[1:])
	sys.exit(returnCode)
