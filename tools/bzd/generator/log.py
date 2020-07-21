#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import sys

class Log():

	@staticmethod
	def fatal(message: str, e: Exception) -> None:
		sys.stderr.write("[ERROR] {}: {}\n".format(message, str(e)))
		sys.exit(1)
