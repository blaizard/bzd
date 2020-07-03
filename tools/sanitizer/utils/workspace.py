#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
import re

class Files:
	def __init__(self, workspace, regexpr = r'.*'):
		self.workspace = workspace
		self.regexpr = re.compile(regexpr)

	def data(self):
		for (dirpath, dirnames, filenames) in os.walk(self.workspace):
			for filename in filenames:
				path = os.path.join(dirpath, filename)
				if re.match(self.regexpr, path):
					yield path
