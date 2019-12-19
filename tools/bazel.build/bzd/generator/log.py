#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import sys

class Log():

	@staticmethod
	def fatal(message, e):
		sys.stderr.write("{}: {}".format(str(message), str(e)))
		sys.exit(1)
