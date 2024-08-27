import pathlib
import typing
import argparse
import sys
import time
import threading
import tempfile

from apps.bootloader.binary import Binary, BinaryForTest, StablePolicy, ExceptionBinaryAbort
from apps.bootloader.singleton import Singleton
from apps.bootloader.config import bootloaderDefault
from apps.bootloader.scheduler import Scheduler
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


def autoUpdateApplication(binary) -> None:
	"""Check if there is an update available."""

	maybeBinary = binary.update()
	if maybeBinary:
		binary.abort()


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Bootloader.")
	parser.add_argument("--cache",
	                    type=pathlib.Path,
	                    default=pathlib.Path(tempfile.gettempdir()) / f".{pathlib.Path(__file__).name}",
	                    help="Cache directory.")
	parser.add_argument("--bootloader", type=str, default=bootloaderDefault, help="Bootloader application.")
	parser.add_argument("--interval", type=float, default=3600, help="Application and bootloader update interval in seconds.")
	parser.add_argument("uid", type=str, help="The unique identifier of this instance.")
	parser.add_argument("app", type=str, nargs="?", default=None, help="The identifier of the application.")

	args = parser.parse_args()
	assert not args.uid.startswith("_"), f"The UID '{args.uid}' cannot start with '_'."

	# Make sure only one instance of the bootloader is running at a time.
	#singleton = Singleton(args.cache / "singleton.lock")
	#if not singleton.lock():
	#	sys.exit(0)

	selfTests(args.cache)

	binary = Binary(uid=args.uid, app=args.app, root=args.cache)
	#bootloader = Binary(uid="_bootloader", app=args.bootloader, root=args.cache)

	scheduler = Scheduler()
	scheduler.add("application", args.interval, autoUpdateApplication, args=[binary], immediate=True)
	scheduler.start()

	# Exit if no app is intended to run.
	if args.app is None:
		sys.exit(0)

	lastFailure = 0
	while True:
		binary.wait()

		try:
			binary.run(args=[], stablePolicy=StablePolicy.runningPast1h)
			sys.exit(0)
		except KeyboardInterrupt:
			Logger.info("<<< Keyboard interrupt >>>")
			sys.exit(130)
		except ExceptionBinaryAbort:
			Logger.info("Application aborted, restart.")
			continue
		except Exception as e:
			Logger.error(e)
			binary.rollback()

		currentTime = time.time()
		timeDiffSinceLastFailure = currentTime - lastFailure
		if timeDiffSinceLastFailure > 3600 * 2:
			Logger.error("Retrying immediately.")
		else:
			Logger.error("Retrying in 1h.")
			time.sleep(3600)
		lastFailure = currentTime
