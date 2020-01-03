#!/usr/bin/python3
# -*- coding: iso-8859-1 -*-

import json5
from log import Log

"""
Open a manifest file and convert it to a python dict.
"""
def manifestToDict(path):
	try:
		f = open(path)
	except Exception as e:
		Log.fatal("Unable to open manifest '{}'".format(path), e)
	try:
		return json5.load(f)
	except Exception as e:
		Log.fatal("Unable to parse manifest '{}'".format(path), e)
	finally:
		f.close()
