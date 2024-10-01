import pathlib
import sys

from apps.bootloader.bootloader import bootloader, ContextForTest
from bzd.logging import Logger

if __name__ == "__main__":

	logger = Logger("update")

	sleepBinary = str(pathlib.Path(__file__).parent / "sleep")
	release = {
	    "print": [{
	        "name": "first",
	        "binary": b"#!/bin/bash\necho $@\nexit 0"
	    }],
	}

	context = ContextForTest(release=release,
	                         args=[
	                             "--bootloader-application", sleepBinary, "--bootloader-update-path", "print",
	                             "--bootloader-uid", "test_uid", "3600"
	                         ],
	                         logger=logger)

	returnCode = bootloader(context)
	sys.exit(returnCode)
