#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import shlex
import os
import re

COMMAND_LINE = ""

"""
This utility extracts compilation information from the command line.
Build your application with make VERBOSE=1 and copy/paste the targted command line into COMMAND_LINE.
"""
if __name__== "__main__":

	global state
	state = None

	libs = []
	lds = []
	linker = []
	libSearchPathList = ["/"]

	def handleArg(key, arg, callback):
		global state
		if state == key:
			state = None
			callback(arg.strip())
		elif arg.startswith(key):
			if arg == key:
				state = key
			else:
				callback(arg[len(key):].strip())

	def addLib(name):
		fileName = name if re.match(r'lib.*\.a', os.path.basename(name)) else "lib{}.a".format(name)
		for path in libSearchPathList:
			p = os.path.join(path, fileName)
			if os.path.isfile(p):
				libs.append(p)
				m = re.match(r'lib(.*)\.a', os.path.basename(p))
				linker.append("-l{}".format(m.group(1)))
				return
		print("WARNING: cannot locate library {}".format(name))

	def addLd(fileName):
		for path in libSearchPathList:
			p = os.path.join(path, fileName)
			if os.path.isfile(p):
				linker.append("-T{}".format(os.path.basename(fileName)))
				lds.append(p)
				return
		print("WARNING: cannot locate linker script {}".format(fileName))

	for arg in shlex.split(COMMAND_LINE):
		# Lib search path
		handleArg("-L", arg, lambda x : libSearchPathList.append(x))
		# Lib
		handleArg("-l", arg, lambda x : addLib(x))
		if arg.endswith(".a"):
			addLib(arg)
		# Linker script
		handleArg("-T", arg, lambda x : addLd(x))


	print("# Linker scripts")
	for p in lds:
		print("cp \"{}\" .".format(p))
	print("# Libraries")
	for p in libs:
		print("cp \"{}\" .".format(p))
	print("# Linker arguments")
	for p in linker:
		print(p)
