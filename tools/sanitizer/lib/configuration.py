#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

from lib.utility import executeCommand

class Configuration():

	def __init__(self, workspace, config = None):
		self.workspace = workspace
		self.config = config
		self.execRoot = executeCommand(["bazel", "info", "execution_root"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.bazelBin = executeCommand(["bazel", "info", "bazel-bin"] + self.getBazelExtraArgs(), self.workspace).splitlines()[0]
		self.ignoreList = [
			"external/",
			"bazel-out/"
		]

	def getBazelExtraArgs(self):
		return ["--config", self.config] if self.config else []

	def getExecRoot(self):
		return self.execRoot

	def getBazelBin(self):
		return self.bazelBin

	def getWorkspace(self):
		return self.workspace

	def isValidPath(self, path):
		for ignorePath in self.ignoreList:
			if path.startswith(ignorePath):
				return False
		return True
